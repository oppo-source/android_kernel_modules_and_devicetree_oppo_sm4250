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
#ifndef _OPLUS_DISPLAY_PANEL_COMMON_H_
#define _OPLUS_DISPLAY_PANEL_COMMON_H_

#include <linux/err.h>
#include "dsi_display.h"
#include "dsi_panel.h"
#include "dsi_ctrl.h"
#include "dsi_ctrl_hw.h"
#include "dsi_drm.h"
#include "dsi_clk.h"
#include "dsi_pwr.h"
#include "sde_dbg.h"

#define PANEL_REG_MAX_LENS 28

/* Add for solve sau issue*/
extern int lcd_closebl_flag;
/* Add for fingerprint silence*/
extern int lcd_closebl_flag_fp;

struct panel_id
{
	uint32_t DA;
	uint32_t DB;
	uint32_t DC;
};

struct panel_info{
	char version[32];
	char manufacture[32];
};

struct display_timing_info {
	uint32_t h_active;
	uint32_t v_active;
	uint32_t refresh_rate;
	uint32_t clk_rate_hz_h32;  /* the high 32bit of clk_rate_hz */
	uint32_t clk_rate_hz_l32;  /* the low 32bit of clk_rate_hz */
};

enum {
	NONE_TYPE = 0,
	LCM_DC_MODE_TYPE,
	LCM_BRIGHTNESS_TYPE,
	MAX_INFO_TYPE,
};

struct panel_reg_get {
	uint32_t reg_rw[PANEL_REG_MAX_LENS];
	uint32_t lens; /*reg_rw lens, lens represent for u32 to user space*/
};

struct panel_reg_rw {
	uint32_t rw_flags; /*1 for read, 0 for write*/
	uint32_t cmd;
	uint32_t lens;     /*lens represent for u8 to kernel space*/
	uint32_t value[PANEL_REG_MAX_LENS]; /*for read, value is empty, just user get function for read the value*/
};

int oplus_display_panel_get_max_brightness(void *buf);
int oplus_display_panel_set_max_brightness(void *buf);
int oplus_display_panel_get_brightness(void *buf);
int oplus_display_panel_get_vendor(void *buf);
int oplus_display_panel_dump_info(void *data);
int oplus_display_get_softiris_color_status(void *data);
int oplus_display_panel_get_closebl_flag(void *data);
int oplus_display_panel_set_closebl_flag(void *data);

#endif /*_OPLUS_DISPLAY_PANEL_COMMON_H_*/
