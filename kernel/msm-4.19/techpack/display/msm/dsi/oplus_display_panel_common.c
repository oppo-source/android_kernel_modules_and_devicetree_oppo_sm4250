/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_display_panel_common.c
** Description : oplus display panel common feature
** Version : 1.0
** Date : 2020/06/13
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**  Li.Sheng       2020/06/13        1.0           Build this moudle
******************************************************************/
#include <linux/notifier.h>
#include <linux/msm_drm_notify.h>

#include "oplus_display_panel_common.h"
#include "oplus_display_panel.h"

/* Add for solve sau issue*/
int lcd_closebl_flag = 0;
/* Add for fingerprint silence*/
int lcd_closebl_flag_fp = 0;

extern int oplus_debug_max_brightness;

int oplus_display_panel_get_max_brightness(void *buf)
{
	uint32_t *max_brightness = buf;
	struct dsi_display *display = get_main_display();

	if (!display || !display->panel) {
		pr_err("%s failed to get display\n", __func__);
		return -EINVAL;
	}

	if (oplus_debug_max_brightness == 0) {
		(*max_brightness) = display->panel->bl_config.brightness_normal_max_level;
	} else {
		(*max_brightness) = oplus_debug_max_brightness;
	}

	return 0;
}

int oplus_display_panel_set_max_brightness(void *buf)
{
	uint32_t *max_brightness = buf;

	oplus_debug_max_brightness = (*max_brightness);

	return 0;
}

int oplus_display_panel_get_brightness(void *buf)
{
	uint32_t *brightness = buf;
	struct dsi_display *display = get_main_display();

	if (!display || !display->panel) {
		pr_err("%s failed to get display\n", __func__);
		return -EINVAL;
	}

	(*brightness) = display->panel->bl_config.bl_level;

	return 0;
}

int oplus_display_panel_get_vendor(void *buf)
{
	struct panel_info *p_info = buf;
	struct dsi_display *display = NULL;
	char *vendor = NULL;
	char *manu_name = NULL;

	display = get_main_display();
	if (!display || !display->panel ||
	    !display->panel->oplus_priv.vendor_name ||
	    !display->panel->oplus_priv.manufacture_name) {
		pr_err("failed to config lcd proc device");
		return -EINVAL;
	}

	vendor = (char *)display->panel->oplus_priv.vendor_name;
	manu_name = (char *)display->panel->oplus_priv.manufacture_name;

	memcpy(p_info->version, vendor, strlen(vendor) > 31?31:(strlen(vendor)+1));
	memcpy(p_info->manufacture, manu_name, strlen(manu_name) > 31?31:(strlen(manu_name)+1));

	return 0;
}

int oplus_display_panel_dump_info(void *data) {
	int ret = 0;
	struct dsi_display * temp_display;
	struct display_timing_info *timing_info = data;

	temp_display = get_main_display();

	if (temp_display == NULL) {
		printk(KERN_INFO "oplus_display_dump_info and main display is null");
		ret = -1;
		return ret;
	}

	if(temp_display->modes == NULL) {
		printk(KERN_INFO "oplus_display_dump_info and display modes is null");
		ret = -1;
		return ret;
	}

	timing_info->h_active = temp_display->modes->timing.h_active;
	timing_info->v_active = temp_display->modes->timing.v_active;
	timing_info->refresh_rate = temp_display->modes->timing.refresh_rate;
	timing_info->clk_rate_hz_l32 = (uint32_t)(temp_display->modes->timing.clk_rate_hz & 0x00000000FFFFFFFF);
	timing_info->clk_rate_hz_h32 = (uint32_t)(temp_display->modes->timing.clk_rate_hz >> 32);

	return 0;
}

int oplus_display_get_softiris_color_status(void *data)
{
	struct softiris_color *iris_color_status = data;
	bool color_vivid_status = false;
	bool color_srgb_status = false;
	bool color_softiris_status = false;
	struct dsi_parser_utils *utils = NULL;
	struct dsi_panel *panel = NULL;

	struct dsi_display *display = get_main_display();
	if (!display) {
		pr_err("failed for: %s %d\n", __func__, __LINE__);
		return -EINVAL;
	}

	panel = display->panel;
	if (!panel) {
		pr_err("failed for: %s %d\n", __func__, __LINE__);
		return -EINVAL;
	}

	utils = &panel->utils;
	if (!utils) {
		pr_err("failed for: %s %d\n", __func__, __LINE__);
		return -EINVAL;
	}

	color_vivid_status = utils->read_bool(utils->data, "oplus,color_vivid_status");
	DSI_INFO("oplus,color_vivid_status: %s", color_vivid_status ? "true" : "false");

	color_srgb_status = utils->read_bool(utils->data, "oplus,color_srgb_status");
	DSI_INFO("oplus,color_srgb_status: %s", color_srgb_status ? "true" : "false");

	color_softiris_status = utils->read_bool(utils->data, "oplus,color_softiris_status");
	DSI_INFO("oplus,color_softiris_status: %s", color_softiris_status ? "true" : "false");

	iris_color_status->color_vivid_status = (uint32_t)color_vivid_status;
	iris_color_status->color_srgb_status = (uint32_t)color_srgb_status;
	iris_color_status->color_softiris_status = (uint32_t)color_softiris_status;

	return 0;
}

int oplus_display_panel_get_closebl_flag(void *data)
{
	uint32_t *closebl_flag = data;

	(*closebl_flag) = lcd_closebl_flag;
	pr_info("oplus_display_get_closebl_flag = %d\n", lcd_closebl_flag);

	return 0;
}

int oplus_display_panel_set_closebl_flag(void *data)
{
	uint32_t *closebl = data;

	pr_info("lcd_closebl_flag = %d\n", (*closebl));
	if (1 != (*closebl))
		lcd_closebl_flag = 0;
	pr_info("oplus_display_set_closebl_flag = %d\n", lcd_closebl_flag);

	return 0;
}

