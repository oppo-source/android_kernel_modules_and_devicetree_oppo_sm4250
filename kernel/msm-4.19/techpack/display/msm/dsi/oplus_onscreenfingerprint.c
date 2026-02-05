/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_onscreenfingerprint.c
** Description : oplus onscreenfingerprint feature
** Version : 1.0
** Date : 2020/04/15
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Guoyifang      2022/01/10        2.0           Build this moudle
**   Qianxu         2020/04/15        1.0           Build this moudle
******************************************************************/
#include "sde_crtc.h"
#include "oplus_onscreenfingerprint.h"
#include "oplus_display_private_api.h"
#include "oplus_display_panel.h"

int oplus_dimlayer_hbm = 0;
int oplus_dimlayer_hbm_count = 0;
int oplus_dimlayer_hbm_vblank_count = 0;
int oplus_dimlayer_fingerprint_failcount = 0;
int oplus_datadimming_vblank_count = 0;

int oplus_dimlayer_dither_threshold = 0;
int oplus_dimlayer_dither_bitdepth = 6;
int oplus_onscreenfp_status = 0;

int oplus_aod_dim_alpha = CUST_A_NO;

atomic_t oplus_dimlayer_hbm_vblank_ref = ATOMIC_INIT(0);
atomic_t oplus_datadimming_vblank_ref = ATOMIC_INIT(0);

struct oplus_brightness_alpha *oplus_brightness_alpha_lut = NULL;
struct oplus_brightness_alpha brightness_alpha_lut[] = {
	{0, 0xff},
	{150, 0xee},
	{190, 0xeb},
	{230, 0xe6},
	{270, 0xe1},
	{310, 0xda},
	{350, 0xd7},
	{400, 0xd3},
	{450, 0xd0},
	{500, 0xcd},
	{600, 0xc3},
	{700, 0xb8},
	{900, 0xa3},
	{1100, 0x90},
	{1300, 0x7b},
	{1400, 0x70},
	{1500, 0x65},
	{1700, 0x4e},
	{1900, 0x38},
	{2047, 0x23},
};

struct oplus_brightness_alpha brightness_alpha_rum_lut[] = {
	{0, 0xff},
	{1, 0xee},
	{2, 0xe8},
	{3, 0xe6},
	{4, 0xe5},
	{6, 0xe4},
	{10, 0xe0},
	{20, 0xd5},
	{30, 0xce},
	{45, 0xc6},
	{70, 0xb7},
	{100, 0xad},
	{150, 0xa0},
	{227, 0x8a},
	{300, 0x80},
	{400, 0x6e},
	{500, 0x5b},
	{600, 0x50},
	{800, 0x38},
	{1023, 0x18},
};

struct oplus_brightness_alpha brightness_alpha_lut_dc[] = {
	{0, 0xff},
	{1, 0xE0},
	{2, 0xd6},
	{3, 0xd5},
	{4, 0xcf},
	{5, 0xcb},
	{6, 0xc9},
	{8, 0xc5},
	{10, 0xc1},
	{15, 0xb6},
	{20, 0xac},
	{30, 0x9d},
	{45, 0x82},
	{70, 0x6c},
	{100, 0x56},
	{120, 0x47},
	{140, 0x3a},
	{160, 0x2f},
	{180, 0x22},
	{200, 0x16},
	{220, 0xe},
	{240, 0x6},
	{260, 0x00},
};

void oplus_set_aod_dim_alpha(int cust)
{
	oplus_aod_dim_alpha = cust;
}

int oplus_get_panel_power_mode(void)
{
	struct dsi_display *display = get_main_display();

	if (!display)
		return 0;

	return display->panel->power_mode;
}

int oplus_display_panel_get_dimlayer_hbm(void *data)
{
	uint32_t *dimlayer_hbm = data;

	(*dimlayer_hbm) = oplus_dimlayer_hbm;

	return 0;
}

int oplus_display_panel_set_dimlayer_hbm(void *data)
{
	struct dsi_display *display = get_main_display();
	struct drm_connector *dsi_connector = display->drm_conn;
	uint32_t *dimlayer_hbm = data;
	int err = 0;
	int value = (*dimlayer_hbm);

	value = !!value;
	if (oplus_dimlayer_hbm == value)
		return 0;

	if (!dsi_connector || !dsi_connector->state || !dsi_connector->state->crtc) {
		pr_err("[%s]: display not ready\n", __func__);
	} else {
		err = drm_crtc_vblank_get(dsi_connector->state->crtc);
		if (err) {
			pr_err("failed to get crtc vblank, error=%d\n", err);
		} else {
			/* do vblank put after 20 frames */
			oplus_dimlayer_hbm_vblank_count = 20;
			atomic_inc(&oplus_dimlayer_hbm_vblank_ref);
		}
	}
	oplus_dimlayer_hbm = value;
	if (!value)
		oplus_dimlayer_hbm_count = value;

#ifdef OPLUS_BUG_STABILITY
	pr_err("debug for oplus_display_set_dimlayer_hbm set oplus_dimlayer_hbm = %d\n", oplus_dimlayer_hbm);
#endif

	return 0;
}

static int bl_to_alpha(int brightness)
{
	struct dsi_display *display = get_main_display();
	struct oplus_brightness_alpha *lut = NULL;
	int count = 0;
	int i = 0;
	int alpha;

	if (!display)
		return 0;

	if (display->panel->ba_seq && display->panel->ba_count) {
		count = display->panel->ba_count;
		lut = display->panel->ba_seq;
	} else {
		if (!display->panel->oplus_priv.brightness_alpha_rum) {
			count = ARRAY_SIZE(brightness_alpha_lut);
			lut = brightness_alpha_lut;
		} else {
			count = ARRAY_SIZE(brightness_alpha_rum_lut);
			lut = brightness_alpha_rum_lut;
		}
	}

	for (i = 0; i < count; i++) {
		if (lut[i].brightness >= brightness)
			break;
	}

	if (i == 0)
		alpha = lut[0].alpha;
	else if (i == count)
		alpha = lut[count - 1].alpha;
	else
		alpha = interpolate(brightness, lut[i-1].brightness,
				    lut[i].brightness, lut[i-1].alpha,
				    lut[i].alpha);

	return alpha;
}

static int bl_to_alpha_dc(int brightness)
{
	int level = ARRAY_SIZE(brightness_alpha_lut_dc);
	int i = 0;
	int alpha;

	for (i = 0; i < ARRAY_SIZE(brightness_alpha_lut_dc); i++) {
		if (brightness_alpha_lut_dc[i].brightness >= brightness)
			break;
	}

	if (i == 0)
		alpha = brightness_alpha_lut_dc[0].alpha;
	else if (i == level)
		alpha = brightness_alpha_lut_dc[level - 1].alpha;
	else
		alpha = interpolate(brightness,
			brightness_alpha_lut_dc[i-1].brightness,
			brightness_alpha_lut_dc[i].brightness,
			brightness_alpha_lut_dc[i-1].alpha,
			brightness_alpha_lut_dc[i].alpha);
	return alpha;
}

static int brightness_to_alpha(int brightness)
{
	int alpha;

	if (brightness == 0 || brightness == 1)
		brightness = oplus_last_backlight;

	if (oplus_dimlayer_hbm)
		alpha = bl_to_alpha(brightness);
	else
		alpha = bl_to_alpha_dc(brightness);

	return alpha;
}

int oplus_get_panel_brightness(void)
{
	struct dsi_display *display = get_main_display();

	if (!display)
		return 0;

	return display->panel->bl_config.bl_level;
}

int oplus_get_panel_brightness_to_alpha(void)
{
	struct dsi_display *display = get_main_display();

	if (!display)
		return 0;
	if (oplus_panel_alpha)
		return oplus_panel_alpha;

	/* force dim layer alpha in AOD scene */
	if (oplus_aod_dim_alpha != CUST_A_NO) {
		if (oplus_aod_dim_alpha == CUST_A_TRANS)
			return 0;
		else if (oplus_aod_dim_alpha == CUST_A_OPAQUE)
			return 255;
	}

	if (hbm_mode)
		return 0;

	if (!oplus_ffl_trigger_finish)
		return brightness_to_alpha(FFL_FP_LEVEL);

	return brightness_to_alpha(display->panel->bl_config.bl_level);
}
