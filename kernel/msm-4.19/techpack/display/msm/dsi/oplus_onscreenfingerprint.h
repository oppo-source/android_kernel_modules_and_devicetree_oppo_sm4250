/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** OPLUS_BUG_STABILITY
** File : oplus_onscreenfingerprint.h
** Description : oplus onscreenfingerprint feature
** Version : 1.0
** Date : 2020/04/15
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Qianxu         2020/04/15        1.0           Build this moudle
******************************************************************/
#ifndef _OPLUS_ONSCREENFINGERPRINT_H_
#define _OPLUS_ONSCREENFINGERPRINT_H_

#include <drm/drm_crtc.h>
#include "dsi_panel.h"
#include "dsi_defs.h"
#include "dsi_parser.h"
#include "sde_encoder_phys.h"

#define FFL_FP_LEVEL 150

enum CUST_ALPHA_ENUM{
	CUST_A_NO = 0,
	CUST_A_TRANS,  /* alpha = 0, transparent */
	CUST_A_OPAQUE, /* alpha = 255, opaque */
};

extern int oplus_dimlayer_hbm;
extern int oplus_dimlayer_hbm_count;
extern int oplus_dimlayer_hbm_vblank_count;
extern int oplus_dimlayer_dither_bitdepth;
extern int oplus_dimlayer_dither_threshold;
extern int oplus_dimlayer_fingerprint_failcount;
extern int oplus_panel_alpha;
extern int hbm_mode;
extern bool oplus_ffl_trigger_finish;
extern atomic_t oplus_dimlayer_hbm_vblank_ref;

int oplus_display_panel_set_dimlayer_hbm(void *data);
int oplus_display_panel_get_dimlayer_hbm(void *data);

int oplus_get_panel_brightness(void);
int oplus_get_panel_brightness_to_alpha(void);

int oplus_get_panel_power_mode(void);
void oplus_set_aod_dim_alpha(int cust);

#endif /*_OPLUS_ONSCREENFINGERPRINT_H_*/
