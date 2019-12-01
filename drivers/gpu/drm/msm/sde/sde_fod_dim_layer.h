/*
 * Copyright (C) 2020 The LineageOS Project
 * Copyright (C) 2020 Pig <pig.priv@gmail.com>
 *
 */

#ifndef _SDE_FOD_DIM_LAYER_H_
#define _SDE_FOD_DIM_LAYER_H_

/**
 * enum sde_crtc_mi_layer_type: type of mi layer
 * @MI_LAYER_FOD_PRESSED_ICON: FOD touched icon layer
 * @MI_LAYER_FOD_ICON: FOD untouch icon layer
 * @MI_LAYER_AOD: AOD layer
 */
enum sde_crtc_mi_layer_type {
	MI_LAYER_NULL = 0x0,
	MI_LAYER_FOD_HBM_OVERLAY = 0x1,
	MI_LAYER_FOD_ICON = 0x2,
	MI_LAYER_AOD = 0x4,
	MI_LAYER_MAX,
};

/**
 * sde_crtc_mi_dc_backlight - mi dc backlight
 * @mi_dc_bl_state: dc backlihgt state
 * @mi_dc_backlight_level: last backlight stash
 * @mi_dc_layer_alpha: dc dim layer alpha
 */
typedef struct sde_crtc_mi_dc_backlight
{
	uint8_t mi_dc_bl_state;
	int32_t mi_dc_bl_level;
	int32_t mi_dc_bl_layer_alpha;
} sde_crtc_mi_dc_backlight;

typedef struct sde_crtc_mi_layer
{
	int32_t layer_index;
	enum sde_crtc_mi_layer_type last_state;
} sde_crtc_mi_layer;

/**
 * sde_crtc_mi_state - mi crtc state
 * @mi_dim_layer: dim layer added by Mi
 */
struct sde_crtc_mi_state {
	struct sde_hw_dim_layer *mi_dim_layer;
	struct sde_crtc_mi_layer mi_layer;
	uint32_t dimlayer_backlight_stash;
	uint8_t  dimlayer_alpha_stash;
};

enum mi_dimlayer_type {
	MI_DIMLAYER_NULL = 0x0,
	MI_DIMLAYER_FOD_HBM_OVERLAY = 0x1,
	MI_DIMLAYER_FOD_ICON = 0x2,
	MI_DIMLAYER_AOD = 0x4,
	MI_FOD_UNLOCK_SUCCESS = 0x8,
	MI_DIMLAYER_MAX,
};

struct mi_dimlayer_state
{
	enum mi_dimlayer_type mi_dimlayer_type;
	uint32_t current_backlight;
};

int dsi_display_hbm_set_disp_param(struct drm_connector *connector,
				u32 param_type);

/**
 * sde_connector_hbm_ctl - mi function to control hbm
 * @connector: Pointer to DRM connector object
 * @op_code: hbm operation code
 */
int sde_connector_hbm_ctl(struct drm_connector *connector, uint32_t op_code);

int sde_connector_pre_hbm_ctl(struct drm_connector *connector);

void sde_connector_mi_update_dimlayer_state(struct drm_connector *connector,
	enum mi_dimlayer_type mi_dimlayer_type);

void sde_connector_mi_get_current_backlight(struct drm_connector *connector, uint32_t *brightness);

void sde_connector_mi_get_current_alpha(struct drm_connector *connector, uint32_t brightness, uint32_t *alpha);

void sde_connector_fod_notify(struct drm_connector *connector);

int _sde_connector_mi_dimlayer_hbm_fence(struct drm_connector *connector);

#endif /* SDE_FOD_DIM_LAYER_H */
