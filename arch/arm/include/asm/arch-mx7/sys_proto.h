/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/imx-common/sys_proto.h>

void set_wdog_reset(struct wdog_regs *wdog);
enum boot_device get_boot_device(void);
