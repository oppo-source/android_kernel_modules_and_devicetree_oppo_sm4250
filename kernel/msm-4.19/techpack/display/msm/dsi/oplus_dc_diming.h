/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_dc_diming.h
** Description : oplus dc_diming feature
** Version : 1.0
** Date : 2020/04/15
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Qianxu         2020/04/15        1.0           Build this moudle
******************************************************************/
#ifndef _OPLUS_DC_DIMING_H_
#define _OPLUS_DC_DIMING_H_

#include <drm/drm_connector.h>

#include "dsi_panel.h"
#include "dsi_defs.h"

extern bool oplus_dc_v2_on;
extern int oplus_dc2_alpha;
extern int oplus_panel_alpha;
extern int oplus_dimlayer_bl_delay;
extern int oplus_dimlayer_bl_enable;
extern int oplus_dimlayer_bl_enable_v2;
extern int oplus_dimlayer_bl_enable_v3;
extern int oplus_dimlayer_bl_alpha;
extern int oplus_dimlayer_bl_alpha_value;
extern int oplus_dimlayer_bl_delay_after;
extern int oplus_datadimming_vblank_count;
extern int oplus_underbrightness_alpha;

extern atomic_t oplus_datadimming_vblank_ref;

int oplus_seed_bright_to_alpha(int brightness);

int oplus_display_panel_get_dim_alpha(void *buf);
int oplus_display_panel_set_dim_alpha(void *buf);
int oplus_display_panel_get_dim_dc_alpha(void *buf);
int oplus_display_panel_set_dimlayer_enable(void *data);
int oplus_display_panel_get_dimlayer_enable(void *data);

#endif /*_OPLUS_DC_DIMING_H_*/
