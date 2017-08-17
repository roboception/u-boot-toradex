/*
 * cmd_sdp.c -- sdp command
 *
 * Copyright (C) 2016 Toradex
 * Author: Stefan Agner <stefan.agner@toradex.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <g_dnl.h>
#include <sdp.h>
#include <usb.h>

static int do_sdp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = CMD_RET_SUCCESS;

	if (argc < 2)
		return CMD_RET_USAGE;

	char *usb_controller = argv[1];
	int controller_index = simple_strtoul(usb_controller, NULL, 0);
	board_usb_init(controller_index, USB_INIT_DEVICE);

	g_dnl_clear_detach();
	g_dnl_register("usb_dnl_sdp");

	ret = sdp_init();
	if (ret) {
		error("SDP init failed: %d", ret);
		ret = CMD_RET_FAILURE;
		goto exit;
	}

	ret = sdp_handle();
	if (ret) {
		error("SDP failed: %d", ret);
		ret = CMD_RET_FAILURE;
		goto exit;
	}

exit:
	g_dnl_unregister();

	return ret;
}

U_BOOT_CMD(sdp, 2, 1, do_sdp,
	"Serial Downloader Protocol",
	"<USB_controller>\n"
	"  - serial downloader protocol via <USB_controller>\n"
);
