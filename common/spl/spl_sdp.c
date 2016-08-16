/*
 * (C) Copyright 2016 Toradex
 * Author: Stefan Agner <stefan.agner@toradex.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spl.h>
#include <usb.h>
#include <g_dnl.h>
#include <sdp.h>

DECLARE_GLOBAL_DATA_PTR;

static int spl_sdp_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	int ret;

	g_dnl_clear_detach();
	g_dnl_register("usb_dnl_sdp");

	ret = sdp_init();
	if (ret) {
		error("SDP init failed: %d", ret);
		return -ENODEV;
	}

	ret = sdp_handle();
	if (ret) {
		error("SDP failed: %d", ret);
		return -ENODEV;
	}

	return 0;
}
SPL_LOAD_IMAGE_METHOD(0, BOOT_DEVICE_SDP, spl_sdp_load_image);
