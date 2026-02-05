/***************************************************************
** Copyright (C),  2018,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_display_private_api.h
** Description : oplus display private api implement
** Version : 1.0
** Date : 2018/03/20
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Hu.Jie          2018/03/20        1.0           Build this moudle
******************************************************************/
#ifndef _OPLUS_DISPLAY_PRIVATE_API_H_
#define _OPLUS_DISPLAY_PRIVATE_API_H_

#include <linux/list.h>
#include <linux/of.h>
#include <linux/err.h>
#include "msm_drv.h"
#include "sde_connector.h"
#include "msm_mmu.h"
#include "dsi_display.h"
#include "dsi_panel.h"
#include "dsi_ctrl.h"
#include "dsi_ctrl_hw.h"
#include "dsi_drm.h"
#include "dsi_clk.h"
#include "dsi_pwr.h"
#include "sde_dbg.h"
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <drm/drm_mipi_dsi.h>
#include "dsi_oplus_support.h"

extern int oplus_onscreenfp_status;
extern int oplus_display_mode;
extern int oplus_dimlayer_bl_enable_v3_real;
extern u32 oplus_last_backlight;

int interpolate(int x, int xa, int xb, int ya, int yb);
bool is_dsi_panel(struct drm_crtc *crtc);
#endif /* _OPLUS_DISPLAY_PRIVATE_API_H_ */
