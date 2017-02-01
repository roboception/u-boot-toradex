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

#ifndef HEADER_CONFIG_MENDER_H
#define HEADER_CONFIG_MENDER_H

#include <config_mender_defines.h>

#ifndef CONFIG_ENV_IS_IN_MMC
# error CONFIG_ENV_IS_IN_MMC is required for Mender to work
#endif

#ifndef CONFIG_BOOTCOUNT_LIMIT
# error CONFIG_BOOTCOUNT_LIMIT is required for Mender to work
#endif

/* Currently Mender needs bootcount to reside in environment. */
#ifndef CONFIG_BOOTCOUNT_ENV
# error CONFIG_BOOTCOUNT_ENV is required for Mender to work
#endif

#ifdef CONFIG_SYS_MMC_ENV_DEV
# error CONFIG_SYS_MMC_ENV_DEV should not be defined explicitly (will be auto-configured).
#endif
#ifdef CONFIG_SYS_MMC_ENV_PART
# error CONFIG_SYS_MMC_ENV_PART should not be defined explicitly (will be auto-configured).
#endif
#ifdef CONFIG_ENV_OFFSET
# error CONFIG_ENV_OFFSET should not be defined explicitly (will be auto-configured).
#endif
#ifdef CONFIG_ENV_OFFSET_REDUND
# error CONFIG_ENV_OFFSET_REDUND should not be defined explicitly (will be auto-configured).
#endif

#if MENDER_BOOTENV_SIZE != CONFIG_ENV_SIZE
# error 'CONFIG_ENV_SIZE' define must be equal to bitbake variable 'BOOTENV_SIZE' set in U-Boot build recipe.
#endif

#define CONFIG_SYS_MMC_ENV_DEV   MENDER_UBOOT_STORAGE_DEVICE
/* Use MMC partition zero to select whole user area of memory card. */
#define CONFIG_SYS_MMC_ENV_PART  0
#define CONFIG_ENV_OFFSET        MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_1
#define CONFIG_ENV_OFFSET_REDUND MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_2

#endif /* HEADER_CONFIG_MENDER_H */
