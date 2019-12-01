/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DSI_PANEL_MI_H_
#define _DSI_PANEL_MI_H_

#define DEFAULT_FOD_OFF_DIMMING_DELAY	170

#define BACKLIGHT_PLUSE_THRESHOLD	0

enum doze_bkl {
	DOZE_TO_NORMAL = 0,
	DOZE_BRIGHTNESS_HBM,
	DOZE_BRIGHTNESS_LBM,
};

typedef struct brightness_alpha {
	uint32_t brightness;
	uint32_t alpha;
} brightness_alpha;

struct dsi_panel_mi_cfg {
	struct dsi_panel *dsi_panel;
	u32 last_bl_level;

	/* dc read */
	bool dc_update_flag;

	/* white point coordinate info */
	bool wp_read_enabled;
	u32 wp_reg_read_len;
	u32 wp_info_index;
	u32 wp_info_len;

	/* HBM and brightness use 51 reg ctrl */
	bool hbm_51_ctrl_flag;
	u32 hbm_off_51_index;
	u32 fod_off_51_index;
	u32 fod_on_b2_index;
	bool vi_setting_enabled;
	u32 vi_switch_threshold;

	bool dynamic_elvss_enabled;

	/* elvss dimming info */
	bool elvss_dimming_check_enable;
	u32 elvss_dimming_read_len;
	u32 update_hbm_fod_on_index;
	u32 update_hbm_fod_off_index;

	u32 skip_dimmingon;
	u32 panel_on_dimming_delay;
	struct delayed_work dimming_enable_delayed_work;

	struct delayed_work enter_aod_delayed_work;

	bool hbm_enabled;
	bool fod_hbm_enabled;
	bool fod_hbm_layer_enabled;
	u32 doze_brightness_state;
	u32 unset_doze_brightness;
	u32 fod_off_dimming_delay;
	ktime_t fod_backlight_off_time;
	ktime_t fod_hbm_off_time;
	u32 fod_ui_ready;
	bool layer_fod_unlock_success;
	bool sysfs_fod_unlock_success;

	bool fod_backlight_flag;
	u32 fod_target_backlight;
	bool fod_flag;
	/* set doze hbm/lbm only in AOD */
	bool in_aod;
	u32 dc_threshold;
	bool dc_enable;
	u32 dc_type;
	u32 aod_backlight;
	uint32_t doze_brightness;

	bool fod_dimlayer_enabled;
	bool prepare_before_fod_hbm_on;
	bool delay_before_fod_hbm_on;
	bool delay_after_fod_hbm_on;
	bool delay_before_fod_hbm_off;
	bool delay_after_fod_hbm_off;
	uint32_t brightnes_alpha_lut_item_count;
	brightness_alpha *brightness_alpha_lut;
};

enum bkl_dimming_state {
	STATE_NONE,
	STATE_DIM_BLOCK,
	STATE_DIM_RESTORE,
	STATE_ALL
};

enum DISPPARAM_MODE {
	DISPPARAM_WARM = 0x1,
	DISPPARAM_DEFAULT = 0x2,
	DISPPARAM_COLD = 0x3,
	DISPPARAM_PAPERMODE8 = 0x5,
	DISPPARAM_PAPERMODE1 = 0x6,
	DISPPARAM_PAPERMODE2 = 0x7,
	DISPPARAM_PAPERMODE3 = 0x8,
	DISPPARAM_PAPERMODE4 = 0x9,
	DISPPARAM_PAPERMODE5 = 0xA,
	DISPPARAM_PAPERMODE6 = 0xB,
	DISPPARAM_PAPERMODE7 = 0xC,
	DISPPARAM_WHITEPOINT_XY = 0xE,
	DISPPARAM_CE_ON = 0x10,
	DISPPARAM_CE_OFF = 0xF0,
	DISPPARAM_CABCUI_ON = 0x100,
	DISPPARAM_CABCSTILL_ON = 0x200,
	DISPPARAM_CABCMOVIE_ON = 0x300,
	DISPPARAM_CABC_OFF = 0x400,
	DISPPARAM_SKIN_CE_CABCUI_ON = 0x500,
	DISPPARAM_SKIN_CE_CABCSTILL_ON = 0x600,
	DISPPARAM_SKIN_CE_CABCMOVIE_ON = 0x700,
	DISPPARAM_SKIN_CE_CABC_OFF = 0x800,
	DISPPARAM_DIMMING_OFF = 0xE00,
	DISPPARAM_DIMMING = 0xF00,
	DISPPARAM_ACL_L1 = 0x1000,
	DISPPARAM_ACL_L2 = 0x2000,
	DISPPARAM_ACL_L3 = 0x3000,
	DISPPARAM_ACL_OFF = 0xF000,
	DISPPARAM_HBM_ON = 0x10000,
	DISPPARAM_HBM_FOD_ON = 0x20000,
	DISPPARAM_HBM_FOD2NORM = 0x30000,
	DISPPARAM_DC_ON = 0x40000,
	DISPPARAM_DC_OFF = 0x50000,
	DISPPARAM_HBM_FOD_OFF = 0xE0000,
	DISPPARAM_HBM_OFF = 0xF0000,
	DISPPARAM_LCD_HBM_L1_ON = 0xB0000,
	DISPPARAM_LCD_HBM_L2_ON = 0xC0000,
	DISPPARAM_LCD_HBM_OFF = 0xA0000,
	DISPPARAM_NORMALMODE1 = 0x100000,
	DISPPARAM_P3 = 0x200000,
	DISPPARAM_SRGB = 0x300000,
	DISPPARAM_SKIN_CE = 0x400000,
	DISPPARAM_SKIN_CE_OFF = 0x500000,
	DISPPARAM_DOZE_BRIGHTNESS_HBM = 0x600000,
	DISPPARAM_DOZE_BRIGHTNESS_LBM = 0x700000,
	DISPPARAM_DOZE_OFF = 0x800000,
	DISPPARAM_HBM_BACKLIGHT_RESEND = 0xA00000,
	DISPPARAM_FOD_BACKLIGHT = 0xD00000,
	DISPPARAM_CRC_OFF = 0xF00000,
	DISPPARAM_FOD_BACKLIGHT_ON = 0x1000000,
	DISPPARAM_FOD_BACKLIGHT_OFF = 0x2000000,
	DISPPARAM_ELVSS_DIMMING_ON = 0x3000000,
	DISPPARAM_ELVSS_DIMMING_OFF = 0x4000000,
	DISPPARAM_ONE_PLUSE = 0x5000000,
	DISPPARAM_FOUR_PLUSE = 0x6000000,
	DISPPARAM_DEMURA_LEVEL02 = 0x7000000,
	DISPPARAM_DEMURA_LEVEL08 = 0x8000000,
	DISPPARAM_DEMURA_LEVEL0D = 0x9000000,
	DISPPARAM_IDLE_ON = 0xA000000,
	DISPPARAM_IDLE_OFF = 0xB000000,
	DISPPARAM_DFPS_LEVEL1 = 0x10000000,
	DISPPARAM_DFPS_LEVEL2 = 0x20000000,
	DISPPARAM_DFPS_LEVEL3 = 0x30000000,
	DISPPARAM_DFPS_LEVEL4 = 0x40000000,
	DISPPARAM_DFPS_LEVEL5 = 0x50000000,
	DISPPARAM_QSYNC_MIN_FPS_30HZ = 0x60000000,
	DISPPARAM_QSYNC_MIN_FPS_40HZ = 0x70000000,
	DISPPARAM_QSYNC_MIN_FPS_50HZ = 0x80000000,
	DISPPARAM_QSYNC_MIN_FPS_60HZ = 0x90000000,
	DISPPARAM_QSYNC_MIN_FPS_70HZ = 0xA0000000,
	DISPPARAM_QSYNC_MIN_FPS_80HZ = 0xB0000000,
	DISPPARAM_QSYNC_MIN_FPS_90HZ = 0xC0000000,
	DISPPARAM_QSYNC_MIN_FPS_100HZ = 0xD0000000,
	DISPPARAM_QSYNC_MIN_FPS_110HZ = 0xE0000000,
};

int dsi_panel_set_disp_param(struct dsi_panel *panel, u32 param);

int dsi_panel_set_doze_brightness(struct dsi_panel *panel,
				int doze_brightness, bool need_panel_lock);

ssize_t dsi_panel_get_doze_brightness(struct dsi_panel *panel, char *buf);

#endif /* _DSI_PANEL_MI_H_ */
