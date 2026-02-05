/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_dc_diming.c
** Description : oplus dc_diming feature
** Version : 1.0
** Date : 2020/04/15
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Qianxu         2020/04/15        1.0           Build this moudle
******************************************************************/

#include "sde_trace.h"
#include "dsi_defs.h"

#include "oplus_dc_diming.h"
#include "oplus_display_private_api.h"
#include "oplus_aod.h"

int oplus_panel_alpha = 0;
int oplus_underbrightness_alpha = 0;
int oplus_dc2_alpha = 0;
int oplus_dimlayer_bl_enable = 0;
int oplus_dimlayer_bl_enable_v2 = 0;
int oplus_dimlayer_bl_enable_v3 = 0;
int oplus_dimlayer_bl_enabled = 0;
int oplus_dimlayer_bl_enable_v3_real = 0;
int oplus_dimlayer_bl_alpha = 1110;
int oplus_dimlayer_bl_alpha_value = 1110;
int oplus_dimlayer_bl_enable_real = 0;
int oplus_dimlayer_bl_delay = 500;
int oplus_dimlayer_bl_delay_after = 0;
bool oplus_dc_v2_on = false;
int oplus_boe_dc_min_backlight = 175;
int oplus_boe_dc_max_backlight = 3350;

static struct oplus_brightness_alpha brightness_seed_alpha_lut_dc[] = {
	{0, 1020},
	{1, 1020},
	{165, 1001},
	{195, 993},
	{235, 980},
	{305, 960},
	{400, 920},
	{500, 880},
	{580, 840},
	{591, 800},
	{660, 720},
	{720, 640},
	{780, 560},
	{825, 480},
	{865, 400},
	{900, 320},
	{963, 240},
	{1000, 160},
	{1050, 80},
	{1100, 0},
};

struct oplus_brightness_alpha brightness_seed_alpha_rum_lut_dc[] = {
	{0, 0xff},
	{1, 0xfc},
	{2, 0xfb},
	{3, 0xfa},
	{4, 0xf9},
	{5, 0xf8},
	{6, 0xf7},
	{8, 0xf6},
	{10, 0xf4},
	{15, 0xf0},
	{20, 0xea},
	{30, 0xe0},
	{45, 0xd0},
	{70, 0xbc},
	{100, 0x98},
	{120, 0x80},
	{140, 0x70},
	{160, 0x58},
	{180, 0x48},
	{200, 0x30},
	{220, 0x20},
	{240, 0x10},
	{260, 0x00},
};

int oplus_seed_bright_to_alpha(int brightness)
{
	struct dsi_display *display = get_main_display();
	struct oplus_brightness_alpha *lut = NULL;
	int i = 0;
	int alpha, level;

	lut = brightness_seed_alpha_lut_dc;
	level = ARRAY_SIZE(brightness_seed_alpha_lut_dc);

	if (!display)
		return 0;

	if (display->panel->oplus_priv.brightness_alpha_rum) {
		lut = brightness_seed_alpha_rum_lut_dc;
		level = ARRAY_SIZE(brightness_seed_alpha_rum_lut_dc);
	}

	if (oplus_panel_alpha)
		return oplus_panel_alpha;
	for (i = 0; i < level; i++) {
		if (lut[i].brightness >= brightness)
			break;
	}

	if (i == 0)
		alpha = lut[0].alpha;
	else if (i == level)
		alpha = lut[level - 1].alpha;
	else
		alpha = interpolate(brightness, lut[i-1].brightness,
			lut[i].brightness, lut[i-1].alpha, lut[i].alpha);

	return alpha;
}

int oplus_display_panel_get_dim_alpha(void *buf)
{
	unsigned int *temp_alpha = buf;
	struct dsi_display *display = get_main_display();

	if (!display->panel->is_hbm_enabled ||
		(get_oplus_display_power_status() != OPLUS_DISPLAY_POWER_ON)) {
		(*temp_alpha) = 0;
		return 0;
	}

	(*temp_alpha) = oplus_underbrightness_alpha;
	return 0;
}

int oplus_display_panel_set_dim_alpha(void *buf)
{
	unsigned int *temp_alpha = buf;

	(*temp_alpha) = oplus_panel_alpha;

	return 0;
}

int oplus_display_panel_get_dim_dc_alpha(void *buf)
{
	int ret = 0;
	unsigned int *temp_dim_alpha = buf;
	struct dsi_display *display = get_main_display();

	if (!display || !display->panel) {
		pr_err("%s main display is NULL\n", __func__);
		(*temp_dim_alpha) = 0;
		return 0;
	}

	if (display->panel->is_hbm_enabled ||
		get_oplus_display_power_status() != OPLUS_DISPLAY_POWER_ON) {
		ret = 0;
	}
	if (oplus_dc2_alpha != 0) {
		ret = oplus_dc2_alpha;
	} else if (oplus_underbrightness_alpha != 0) {
		ret = oplus_underbrightness_alpha;
	} else if (oplus_dimlayer_bl_enable_v3_real) {
		ret = 1;
	}

	(*temp_dim_alpha) = ret;
	return 0;
}

int oplus_display_panel_set_dimlayer_enable(void *data)
{
	struct dsi_display *display = NULL;
	struct drm_connector *dsi_connector = NULL;
	uint32_t *dimlayer_enable = data;

	display = get_main_display();
	if (!display) {
		return -EINVAL;
	}

	dsi_connector = display->drm_conn;
	if (display && display->name) {
		int enable = (*dimlayer_enable);
		int err = 0;

		mutex_lock(&display->display_lock);
		if (!dsi_connector || !dsi_connector->state || !dsi_connector->state->crtc) {
			pr_err("[%s]: display not ready\n", __func__);
		} else {
			err = drm_crtc_vblank_get(dsi_connector->state->crtc);
			if (err) {
				pr_err("failed to get crtc vblank, error=%d\n", err);
			} else {
				/* do vblank put after 7 frames */
				oplus_datadimming_vblank_count = 7;
				atomic_inc(&oplus_datadimming_vblank_ref);
			}
		}

		usleep_range(17000, 17100);
		if (!strcmp(display->panel->oplus_priv.vendor_name, "ANA6706")) {
			oplus_dimlayer_bl_enable = enable;
		} else if (!strcmp(display->name, "qcom,mdss_dsi_oplus19101boe_nt37800_1080_2400_cmd")) {
			oplus_dimlayer_bl_enable_v3 = enable;
		}
		else {
			oplus_dimlayer_bl_enable_v2 = enable;
		}

		mutex_unlock(&display->display_lock);
	}

	return 0;
}

int oplus_display_panel_get_dimlayer_enable(void *data)
{
	uint32_t *dimlayer_bl_enable = data;

	(*dimlayer_bl_enable) = oplus_dimlayer_bl_enable_v2;

	return 0;
}
