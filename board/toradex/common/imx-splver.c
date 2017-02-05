/*
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <mmc.h>

#define SPL_IMX_MAX_SIZE (CONFIG_SPL_MAX_SIZE + 4096)
#ifdef CONFIG_TDX_CFG_BLOCK_IS_IN_MMC
static void get_spl_version(char *ver, size_t max_len, int dev, uint part)
{
	unsigned int ivt_header;
	struct mmc *mmc;
	char * pch;
	char *spl;
	size_t spl_size;
	char* spl_start;
	const char * ver_start = "U-Boot SPL";

	/* initialize ver, start with space to fail the min_ver check */
	strcpy(ver, " Unknown");

	spl = memalign(ARCH_DMA_MINALIGN, SPL_IMX_MAX_SIZE);
	if (spl == NULL) {
		puts("Could not allocate RAM\n");
		return;
	}

	/* read a maximum SPL into RAM */
	mmc = find_mmc_device(dev);
	if (!mmc) {
		puts("No MMC card found\n");
		goto out;
	}
	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		goto out;
	}
	if (part != mmc->block_dev.hwpart) {
		if (blk_select_hwpart_devnum(IF_TYPE_MMC, dev, part)) {
			puts("MMC partition switch failed\n");
			goto out;
		}
	}
	/* Read the eMMC for the maximum SPL size */
	if (blk_dread(mmc_get_blk_desc(mmc), 2, SPL_IMX_MAX_SIZE / 512, spl) !=
		      (SPL_IMX_MAX_SIZE / 512)) {
		puts("MMC read failed\n");
		goto out;
	}

	spl_start = spl;
	(void)memcpy(&ivt_header, &spl[0], sizeof(spl_size));
	(void)memcpy(&spl_size, &spl[9 * 4], sizeof(spl_size));
	/* sanity check, do we see a correct IVT header ? */
	if ((ivt_header & ~(01000000)) != 0x402000d1) {
		puts("Did not find IVT header, no SPL or U-Boot in eMMC\n");
		goto out;
	}

	/* sanity check, SPL can't be > 68k */
	if (spl_size > SPL_IMX_MAX_SIZE){
		puts("Size of bootloader > 68k, likely no SPL installed\n");
		goto out;
	}

	while (spl_size) {
		pch = (char*) memchr(spl_start, ver_start[0], spl_size);
		if (pch!=NULL && pch < spl_start + spl_size) {
			spl_size = spl_size - (pch - spl_start);
			spl_start = pch +1;
			if (!memcmp(ver_start, pch, strlen(ver_start))) {
				strncpy(ver, pch, max_len);
				ver[max_len - 1] = 0;
				spl_size = 0;
				break;
			}
		} else {
			spl_size = 0;
		}
	}
out:
	free(spl);
	spl = NULL;

	/* Switch back to regular eMMC user partition */
	blk_select_hwpart_devnum(IF_TYPE_MMC, 0, 0);
}

static int do_splver(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char ver[256];
	int res = 0;
	ver[0] = 0;
	get_spl_version(ver, ARRAY_SIZE(ver), 0, 1);
	printf ("%s\n", ver);
	if (argc > 1)
		res = strncmp(ver, argv[1], strlen(argv[1]));
	return (res < 0) ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	splver, 2, 0, do_splver,
	"Print and compare the SPL version",
	"Parses the SPL version from eMMC\n"
	"[min_ver] compares against the optional parameter 'min_ver' and\n"
	"returns success if installed version is >= min_ver"
);

#endif
