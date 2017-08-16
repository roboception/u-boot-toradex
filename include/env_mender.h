/*
  Copyright (C) 2016  Mender Software

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef HEADER_ENV_MENDER_H
#define HEADER_ENV_MENDER_H

#include <config_mender_defines.h>

#ifdef MENDER_NO_DEFAULT_ALTBOOTCMD
# define MENDER_DEFAULT_ALTBOOTCMD
#else
# define MENDER_DEFAULT_ALTBOOTCMD "altbootcmd=run mender_altbootcmd; " \
    "run bootcmd\0"
#endif

#define MENDER_ENV_SETTINGS                                             \
    MENDER_DEFAULT_ALTBOOTCMD                                           \
    "bootlimit=1\0"                                                     \
    "bootcount=0\0"                                                     \
                                                                        \
    "upgrade_available=0\0"                                             \
                                                                        \
    "mender_boot_part=" __stringify(MENDER_ROOTFS_PART_A_NUMBER) "\0"   \
    "desired_boot_part=" __stringify(MENDER_ROOTFS_PART_A_NUMBER) "\0"   \    
                                                                        \
    "mender_uboot_boot=" MENDER_UBOOT_STORAGE_INTERFACE " " __stringify(MENDER_UBOOT_STORAGE_DEVICE) ":" __stringify(MENDER_BOOT_PART_NUMBER) "\0" \
                                                                        \
    "mender_uboot_if=" MENDER_UBOOT_STORAGE_INTERFACE "\0"              \
                                                                        \
    "mender_uboot_dev=" __stringify(MENDER_UBOOT_STORAGE_DEVICE) "\0"   \
                                                                        \
    "mender_setup="                                                     \
    "setenv mender_kernel_root " MENDER_STORAGE_DEVICE_BASE "${mender_boot_part}; " \
    "setenv mender_uboot_root " MENDER_UBOOT_STORAGE_INTERFACE " " __stringify(MENDER_UBOOT_STORAGE_DEVICE) ":${mender_boot_part}; " \
    "setenv expand_bootargs setenv bootargs ${bootargs}; "              \
    "run expand_bootargs; "                                             \
    "setenv expand_bootargs\0"                                          \
                                                                        \
    "mender_switchpart="                                                \
    "if test ${mender_boot_part} = " __stringify(MENDER_ROOTFS_PART_A_NUMBER) "; "  \
    "then setenv mender_boot_part " __stringify(MENDER_ROOTFS_PART_B_NUMBER) "; "   \
    "else setenv mender_boot_part " __stringify(MENDER_ROOTFS_PART_A_NUMBER) "; "   \
    "fi; "                                                              \
    "echo Switched to partition ${mender_boot_part}; "                  \
    "setenv upgrade_available 0; "                                      \
    "saveenv; saveenv\0"                                                \
                                                                        \
    "mender_altbootcmd="                                                \
    "run mender_switchpart; "                                           \
    "run mender_setup\0"                                                \
                                                                        \
    "mender_try_to_recover="                                            \
    "if test ${upgrade_available} = 1; "                                \
    "then echo try to recover; reset; "                                 \
    "fi\0"

#endif /* HEADER_ENV_MENDER_H */
