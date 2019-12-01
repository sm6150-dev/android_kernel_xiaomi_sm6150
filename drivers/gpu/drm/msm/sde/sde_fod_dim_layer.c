/*
 * Copyright (C) 2020 The LineageOS Project
 * Copyright (C) 2020 Pig <pig.priv@gmail.com>
 *
 */

#include "dsi_defs.h"
#include "dsi_display.h"
#include "dsi_drm.h"
#include "dsi_panel.h"
#include "dsi_panel_mi.h"
#include "sde_connector.h"
#include "sde_crtc.h"
#include "sde_encoder.h"
#include "sde_fod_dim_layer.h"
#include "sde_plane.h"
#include <linux/msm_drm_notify.h>

#define to_dsi_bridge(x)  container_of((x), struct dsi_bridge, base)

int dsi_display_hbm_set_disp_param(struct drm_connector *connector,
				u32 op_code)
{
	int rc;
	struct sde_connector *c_conn;

	c_conn = to_sde_connector(connector);

	pr_debug("%s fod hbm command:0x%x \n", __func__, op_code);

	if (op_code == DISPPARAM_HBM_FOD_ON) {
		rc = dsi_display_set_disp_param(connector, DISPPARAM_HBM_FOD_ON);
	} else if (op_code == DISPPARAM_HBM_FOD_OFF) {
		/* close HBM and restore DC */
		rc = dsi_display_set_disp_param(connector, DISPPARAM_HBM_FOD_OFF);
	} else if(op_code == DISPPARAM_DIMMING_OFF) {
		rc = dsi_display_set_disp_param(connector, DISPPARAM_DIMMING_OFF);
	} else if (op_code == DISPPARAM_HBM_BACKLIGHT_RESEND) {
		rc = dsi_display_set_disp_param(connector, DISPPARAM_HBM_BACKLIGHT_RESEND);
	}

	return rc;
}

int sde_connector_hbm_ctl(struct drm_connector *connector, uint32_t op_code)
{
	int ret = 0;

	ret = dsi_display_hbm_set_disp_param(connector, op_code);
	return ret;
}

int sde_connector_pre_hbm_ctl(struct drm_connector *connector)
{
	int ret;
	/* close dimming */
	ret = dsi_display_set_disp_param(connector, DISPPARAM_DIMMING_OFF);
	return ret;
}

void sde_crtc_fod_ui_ready(struct dsi_display *display, int type, int value)
{
	int event;
	struct msm_drm_notifier notify_data;
	event = FOD_FINGERDOWN;
	notify_data.data = &event;

	if (!display)
		return;

	if (type == 1) /* HBM */
	{
		if (value == 0)
			display->panel->mi_cfg.fod_ui_ready &= ~0x01;
		else if (value == 1)
			display->panel->mi_cfg.fod_ui_ready |= 0x01;
	}

	if (type == 2) /* ICON */
	{
		if (value == 0)
			display->panel->mi_cfg.fod_ui_ready &= ~0x02;
		else if (value == 1)
			display->panel->mi_cfg.fod_ui_ready |= 0x02;

	}

	SDE_INFO("fod_ui_ready notify=%d", display->panel->mi_cfg.fod_ui_ready);
	sysfs_notify(&display->drm_conn->kdev->kobj, NULL, "fod_ui_ready");
	msm_drm_notifier_call_chain(MSM_DRM_EVENT_FOD, &notify_data);
}

int _sde_connector_mi_dimlayer_hbm_fence(struct drm_connector *connector)
{
	int rc = 0;
	struct sde_connector *c_conn;
	struct dsi_display *dsi_display;
	bool skip = false;
	bool hbm_overlay;
	static bool last_fod_unlock_success;
	bool skip_delay_after_fod_hbm_on = false;
#if 0
	bool icon;
	static bool last_icon = false;
#endif
	if (!connector) {
		SDE_ERROR("invalid argument\n");
		return -EINVAL;
	}

	c_conn = to_sde_connector(connector);

	if (c_conn->connector_type != DRM_MODE_CONNECTOR_DSI)
		return 0;

	dsi_display = (struct dsi_display *) c_conn->display;
	if (!dsi_display || !dsi_display->panel) {
		SDE_ERROR("invalid display/panel\n");
		return -EINVAL;
	}

	dsi_display->panel->mi_cfg.layer_fod_unlock_success =
			c_conn->mi_dimlayer_state.mi_dimlayer_type & MI_FOD_UNLOCK_SUCCESS;

	if (last_fod_unlock_success != dsi_display->panel->mi_cfg.layer_fod_unlock_success)
		SDE_INFO("layer_fod_unlock_success = %d\n",
					dsi_display->panel->mi_cfg.layer_fod_unlock_success);

	last_fod_unlock_success = dsi_display->panel->mi_cfg.layer_fod_unlock_success;


	hbm_overlay = c_conn->mi_dimlayer_state.mi_dimlayer_type & MI_DIMLAYER_FOD_HBM_OVERLAY;
	if (hbm_overlay) {
		/* TODO: mutex_lock(&panel->panel_lock); */
		if (dsi_display->panel->mi_cfg.fod_hbm_layer_enabled == false) {
			/* in AOD, first frame should be skipped for hardware limitation */
			if (dsi_display->panel->power_mode == SDE_MODE_DPMS_LP1
				|| dsi_display->panel->power_mode == SDE_MODE_DPMS_LP2) {

				SDE_INFO("fod_frame_count=%d\n", c_conn->fod_frame_count);
				if (c_conn->fod_frame_count == 0)
					skip = true;

				c_conn->fod_frame_count++;
			} else {
				c_conn->fod_frame_count = 0;
			}

			if (skip == false) {
				/* dimming before hbm ctl */
				if (dsi_display->panel->mi_cfg.prepare_before_fod_hbm_on) {
					sde_connector_pre_hbm_ctl(connector);
					sde_encoder_wait_for_event(c_conn->encoder, MSM_ENC_VBLANK);
				}
				if (dsi_display->panel->mi_cfg.delay_before_fod_hbm_on)
					sde_encoder_wait_for_event(c_conn->encoder, MSM_ENC_VBLANK);
				sde_connector_hbm_ctl(connector, DISPPARAM_HBM_FOD_ON);

				/* lmi panel need to skip delay to avoid flash high brightness
				 * if DC on (MIUI-1755728) */
				if (dsi_display->panel->mi_cfg.dc_type == 2 &&
						dsi_display->panel->mi_cfg.dc_enable) {
					skip_delay_after_fod_hbm_on = true;
				}

				if (!skip_delay_after_fod_hbm_on &&
						dsi_display->panel->mi_cfg.delay_after_fod_hbm_on) {
					sde_encoder_wait_for_event(c_conn->encoder, MSM_ENC_VBLANK);
				}

				dsi_display->panel->mi_cfg.fod_hbm_layer_enabled = true;
				/*sde_crtc_fod_ui_ready(dsi_display, 1, 1);*/
			}
		}
	} else {
		if (dsi_display->panel->mi_cfg.fod_hbm_layer_enabled == true) {
			SDE_INFO("layer_fod_unlock_success = %d, sysfs_fod_unlock_success = %d\n",
					dsi_display->panel->mi_cfg.layer_fod_unlock_success,
					dsi_display->panel->mi_cfg.sysfs_fod_unlock_success);
			if (dsi_display->panel->mi_cfg.delay_before_fod_hbm_off)
				sde_encoder_wait_for_event(c_conn->encoder, MSM_ENC_VBLANK);
			sde_connector_hbm_ctl(connector, DISPPARAM_HBM_FOD_OFF);
			if (dsi_display->panel->mi_cfg.dc_type)
				sysfs_notify(&c_conn->bl_device->dev.kobj, NULL, "brightness_clone");
			if (dsi_display->panel->mi_cfg.delay_after_fod_hbm_off)
				sde_encoder_wait_for_event(c_conn->encoder, MSM_ENC_VBLANK);

			dsi_display->panel->mi_cfg.fod_hbm_layer_enabled = false;
			/*sde_crtc_fod_ui_ready(dsi_display, 1, 0);*/
		}
	}
#if 0
	icon = c_conn->mi_dimlayer_state.mi_dimlayer_type & MI_DIMLAYER_FOD_ICON;
	if (last_icon != icon) {
		if (icon) {
			sde_crtc_fod_ui_ready(dsi_display, 2, 1);
		} else {
			if (last_icon)
				sde_crtc_fod_ui_ready(dsi_display, 2, 0);
		}
	}
	last_icon = icon;
#endif
	return rc;
}

void sde_connector_fod_notify(struct drm_connector *conn)
{
	struct sde_connector *c_conn;
	bool icon, hbm_state;
	static bool last_icon = false;
	static bool last_hbm_state = false;
	struct dsi_display *dsi_display;

	if (!conn) {
		SDE_ERROR("invalid params\n");
		return;
	}

	c_conn = to_sde_connector(conn);
	if (c_conn->connector_type != DRM_MODE_CONNECTOR_DSI) {
		SDE_ERROR("not DRM_MODE_CONNECTOR_DSIl\n");
		return;
	}

	dsi_display = (struct dsi_display *) c_conn->display;
	if (!dsi_display || !dsi_display->panel) {
		SDE_ERROR("invalid display/panel\n");
		return;
	}

	icon = c_conn->mi_dimlayer_state.mi_dimlayer_type & MI_DIMLAYER_FOD_ICON;
	if (last_icon != icon) {
		if (icon) {
			sde_crtc_fod_ui_ready(dsi_display, 2, 1);
		} else {
			sde_crtc_fod_ui_ready(dsi_display, 2, 0);
		}
	}
	last_icon = icon;

	hbm_state = dsi_display->panel->mi_cfg.fod_hbm_layer_enabled;
	if (last_hbm_state != hbm_state) {
		if (hbm_state) {
			sde_crtc_fod_ui_ready(dsi_display, 1, 1);
		} else {
			sde_crtc_fod_ui_ready(dsi_display, 1, 0);
		}
	}
	last_hbm_state = hbm_state;
}

static uint32_t interpolate(uint32_t x, uint32_t xa, uint32_t xb, uint32_t ya, uint32_t yb)
{
	uint32_t bf;

	bf = ya - (ya - yb) * (x - xa) / (xb - xa);

	SDE_DEBUG("backlight brightness:%d, [i-1]bl:%d, [i]bl:%d, [i-1]alpha:%d, [i]alpha:%d, bf:%d",
			x, xa, xb, ya, yb, bf);

	return bf;
}

static uint32_t brightness_to_alpha(struct dsi_panel_mi_cfg *mi_cfg, uint32_t brightness)
{
	int i;
	int level = mi_cfg->brightnes_alpha_lut_item_count;

	if (brightness == 0x0)
		return mi_cfg->brightness_alpha_lut[0].alpha;

	for (i = 0; i < level; i++){
		if (mi_cfg->brightness_alpha_lut[i].brightness >= brightness)
			break;
	}

	if (i == level)
		return mi_cfg->brightness_alpha_lut[i - 1].alpha;
	else
		return interpolate(brightness,
							mi_cfg->brightness_alpha_lut[i-1].brightness, mi_cfg->brightness_alpha_lut[i].brightness,
							mi_cfg->brightness_alpha_lut[i-1].alpha, mi_cfg->brightness_alpha_lut[i].alpha);
}

void sde_connector_mi_get_current_alpha(struct drm_connector *connector, uint32_t brightness, uint32_t *alpha)
{
	struct dsi_display *display = NULL;
	struct dsi_bridge *c_bridge = NULL;
	struct dsi_panel_mi_cfg *mi_cfg = NULL;

	if (!connector || !connector->encoder || !connector->encoder->bridge) {
		SDE_ERROR("Invalid connector/encoder/bridge ptr\n");
		return;
	}

	c_bridge =  to_dsi_bridge(connector->encoder->bridge);
	display = c_bridge->display;
	if (!display || !display->panel) {
		SDE_ERROR("invalid display/panel ptr\n");
		return;
	}

	mi_cfg = &display->panel->mi_cfg;

	*alpha = brightness_to_alpha(mi_cfg, brightness);
	return;
}

void sde_connector_mi_get_current_backlight(struct drm_connector *connector, uint32_t *brightness)
{
	struct sde_connector *c_conn = to_sde_connector(connector);
	struct dsi_display *display = NULL;
	struct dsi_bridge *c_bridge = NULL;

	if (!connector || !connector->encoder || !connector->encoder->bridge) {
		SDE_ERROR("Invalid connector/encoder/bridge ptr\n");
		return;
	}

	c_bridge =  to_dsi_bridge(connector->encoder->bridge);
	display = c_bridge->display;
	if (!display || !display->panel) {
		SDE_ERROR("invalid display/panel ptr\n");
		return;
	}

	if (display->panel->mi_cfg.in_aod) {
		*brightness = display->panel->mi_cfg.aod_backlight;
		return;
	}

	*brightness = c_conn->mi_dimlayer_state.current_backlight;
}

void sde_connector_mi_update_dimlayer_state(struct drm_connector *connector,
	enum mi_dimlayer_type mi_dimlayer_type)
{
	struct sde_connector *c_conn = to_sde_connector(connector);
	c_conn->mi_dimlayer_state.mi_dimlayer_type = mi_dimlayer_type;
}

