/*
 * Copyright (c) 2016 Toradex, Inc.
 *
 * Configuration settings for the Toradex Apalis TK1 modules.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

/* enable PMIC */
#define CONFIG_AS3722_POWER

#include "tegra124-common.h"

#define CONFIG_ARCH_MISC_INIT

/* High-level configuration options */
#define CONFIG_DISPLAY_BOARDINFO_LATE	/* Calls show_board_info() */

/* Board-specific serial config */
#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

/* I2C */
#define CONFIG_SYS_I2C_TEGRA

/* SD/MMC support */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_SUPPORT_EMMC_BOOT	/* eMMC specific */

/* Mender support: enable bootcount limit and store it in u-boot environment as big endian */
#define CONFIG_BOOTCOUNT_LIMIT
#define CONFIG_BOOTCOUNT_ENV

/* Environment in eMMC, before config block at the end of 1st "boot sector" */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_SIZE_REDUND CONFIG_ENV_SIZE

/* USB host support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA

/* PCI host support */
#undef CONFIG_PCI_SCAN_SHOW
#define CONFIG_CMD_PCI

/* PCI networking support */
#define CONFIG_E1000_NO_NVM

/* General networking support */
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		16352
#define CONFIG_TFTP_TSIZE

/* Miscellaneous commands */
#define CONFIG_FAT_WRITE

#undef CONFIG_IPADDR
#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTCOMMAND \
	"run bootseq"

#define DFU_ALT_EMMC_INFO	"apalis-tk1.img raw 0x0 0x500 mmcpart 1; " \
				"boot part 0 1 mmcpart 0; " \
				"rootfs part 0 2 mmcpart 0; " \
				"uImage fat 0 1 mmcpart 0; " \
				"tegra124-apalis-eval.dtb fat 0 1 mmcpart 0"

#define EMMC_BOOTCMD \
	"emmcargs=ip=off root=${mender_kernel_root} rw rootfstype=ext3 rootwait\0" \
	"emmcboot=setenv bootargs ${defargs} ${emmcargs} ${setupargs} ${vidargs}; " \
	  "run expand_bootargs; bootm ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"emmcdtbload=setenv dtbparam; load ${mender_uboot_root} ${fdt_addr_r} " \
		"boot/${soc}-apalis-${fdt_board}.dtb && " \
		"setenv dtbparam ${fdt_addr_r}\0" \
	"expand_bootargs=setenv expand setenv emmcargs ${emmcargs};run expand; " \
	  "setenv expand setenv bootargs ${bootargs}; run expand; setenv expand;\0"

#define BOARD_EXTRA_ENV_SETTINGS \
	"boot_file=boot/uImage\0" \
	"console=ttyS0\0" \
	"defargs=lp0_vec=2064@0xf46ff000 core_edp_mv=1150 core_edp_ma=4000 " \
		"usb_port_owner_info=2 lane_owner_info=6 emc_max_dvfs=0 cma=256M panic=60\0" \
	"dfu_alt_info=" DFU_ALT_EMMC_INFO "\0" \
	EMMC_BOOTCMD \
	"fdt_board=eval\0" \
	"fdt_fixup=;\0" \
	"bootseq=run setup; run setup_mender; run pinupdate; run chkbootable; if " \
	  "test ${linuxbootable} = 1; then run emmcboot; else run switchpart; run " \
		"chkbootable; if test ${linuxbootable} = 1; then run emmcboot; else run " \
		"setethupdate; fi; fi;\0" \
	"chkbootable=run chkdtb; run chkkernel; if test ${dtbloaded} = 1 && test " \
	  "${kernelloaded} = 1;then setenv linuxbootable 1; else setenv " \
		"linuxbootable 0; echo Linux not bootable from ${mender_uboot_root}; " \
		"fi;\0" \
	"chkdtb=if load ${mender_uboot_root} ${fdt_addr_r} " \
		"boot/${soc}-apalis-${fdt_board}.dtb; then setenv dtbloaded 1; " \
		"else setenv dtbloaded 0; fi;\0" \
	"chkkernel=if load ${mender_uboot_root} ${kernel_addr_r} ${boot_file}; " \
		"then setenv kernelloaded 1; else setenv kernelloaded 0; fi;\0" \
	"chkpin200=if gpio input A3; then echo 200 A3 low && false; else " \
		"echo 200 A3 high && true; fi;\0" \
  "chkpin204=if gpio input A2; then echo 204 A2 low && false; else " \
		"echo 204 A2 high && true; fi;\0" \
  "pinupdate=if run chkpin200 && run chkpin204 ; then echo update && run " \
		"tftpupdate; else echo no update; fi;\0" \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; pci enum && tftpboot ${loadaddr} " \
		"flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=setenv interface mmc; setenv drive 1; mmc rescan; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img " \
		"|| setenv drive 2; mmc rescan; load ${interface} ${drive}:1 " \
		"${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"setup=setenv setupargs igb_mac=${ethaddr} " \
		"consoleblank=0 no_console_suspend=1 console=tty1 " \
		"console=${console},${baudrate}n8 debug_uartport=lsport,0 " \
		"${memargs}\0" \
	"setup_mender=setenv mender_kernel_root /dev/mmcblk0p${mender_boot_part}; " \
	  "setenv mender_uboot_root mmc 0:${mender_boot_part};\0" \
	"switchpart=if test ${mender_boot_part} = 2; then setenv mender_boot_part 3; " \
	  "echo Switching to partition B; else setenv mender_boot_part 2; echo " \
		"Switching to partition A; fi; run setup_mender;\0" \
  "reload_defaults=env default -a; saveenv;\0" \
	"blink_init=i2c dev 1\0" \
	"blink_white=i2c mw 0x32 0x01 2a 1\0" \
	"blink_red=i2c mw 0x32 0x01 02 1\0" \
	"blink_green=i2c mw 0x32 0x01 20 1\0" \
	"blink_blue=i2c mw 0x32 0x01 08 1\0" \
	"blink_disable=i2c mw 0x32 0x01 00 1\0" \
	"enable_white=run enable_red; run enable_green; run enable_blue;\0" \
	"enable_red=i2c mw 0x32 1c ff\0" \
	"enable_green=i2c mw 0x32 16 ff\0" \
	"enable_blue=i2c mw 0x32 17 ff\0" \
	"disable_white=run disable_red; run disable_green; run disable_blue;\0" \
	"disable_red=i2c mw 0x32 1c 00\0" \
	"disable_green=i2c mw 0x32 16 00\0" \
	"disable_blue=i2c mw 0x32 17 00\0" \
	"tftpconnect=setenv autoload false; if env exists ethaddr; then; else setenv " \
		"ethaddr 00:14:2d:00:00:00; fi; pci enum;\0" \
	"chkupdscr=if tftpboot ${loadaddr} flash_eth.img; then echo update script " \
		"accessible && true; else echo update script not accessible && false; fi;\0" \
	"tftp_connect_state=run blink_init; run blink_white; run tftpconnect; until " \
		"run chkupdscr; do echo try to reconnect && run tftpconnect; done; run " \
		"blink_disable;\0" \
	"tftpupdate=run blink_white; run tftp_connect_state; " \
		"run disable_white; run enable_blue; run blink_blue; run setethupdate && " \
		"run update && run blink_disable && run enable_white && run blink_white && setenv bootcmd " \
		"run reload_defaults && saveenv;\0" \
	"vidargs=video=tegrafb0:640x480-16@60 fbcon=map:1\0"

/* Increase console I/O buffer size */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024

/* Increase arguments buffer size */
#undef CONFIG_SYS_BARGSIZE
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

/* Increase print buffer size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Increase maximum number of arguments */
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		32

#define CONFIG_CMD_TIME

#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#include "tegra-common-usb-gadget.h"
#include "tegra-common-post.h"

/* Reserve top 1M for secure RAM */
#define CONFIG_ARMV7_SECURE_BASE		0xfff00000
#define CONFIG_ARMV7_SECURE_RESERVE_SIZE	0x00100000

#endif /* __CONFIG_H */
