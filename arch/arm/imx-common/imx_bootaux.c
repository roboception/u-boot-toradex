/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/sys_proto.h>
#include <common.h>
#include <command.h>
#include <elf.h>

/* Allow for arch specific config before we boot */
static int __arch_auxiliary_core_up(u32 core_id, u32 stack, u32 pc)
{
	/* please define platform specific arch_auxiliary_core_up() */
	return CMD_RET_FAILURE;
}

int arch_auxiliary_core_up(u32 core_id, u32 stack, u32 pc)
	__attribute__((weak, alias("__arch_auxiliary_core_up")));

/* Allow for arch specific config before we boot */
static int __arch_auxiliary_core_check_up(u32 core_id)
{
	/* please define platform specific arch_auxiliary_core_check_up() */
	return 0;
}

int arch_auxiliary_core_check_up(u32 core_id)
	__attribute__((weak, alias("__arch_auxiliary_core_check_up")));

const __weak struct memorymap hostmap[] = { };

/*
 * Get memory map by auxiliary core memory address
 */
static const struct memorymap *get_host_mapping(unsigned long auxcore)
{
	const struct memorymap *mmap = hostmap;

	while (mmap) {
		if (mmap->auxcore <= auxcore &&
		    mmap->auxcore + mmap->size > auxcore)
			return mmap;
		mmap++;
	}

	return NULL;
}

/*
 * A very simple elf loader, assumes the image is valid, returns the
 * entry point address.
 */
static unsigned long load_elf_image_phdr(unsigned long addr)
{
	Elf32_Ehdr *ehdr; /* Elf header structure pointer */
	Elf32_Phdr *phdr; /* Program header structure pointer */
	int i;

	ehdr = (Elf32_Ehdr *)addr;
	phdr = (Elf32_Phdr *)(addr + ehdr->e_phoff);

	/* Load each program header */
	for (i = 0; i < ehdr->e_phnum; ++i, ++phdr) {
		const struct memorymap *mmap = get_host_mapping(phdr->p_paddr);
		void *dst, *src;

		if (phdr->p_type != PT_LOAD)
			continue;

		if (!mmap) {
			error("Invalid aux core address: %08x", phdr->p_paddr);
			return 0;
		}

		dst = (void *)(phdr->p_paddr - mmap->auxcore) + mmap->host;
		src = (void *)addr + phdr->p_offset;
		debug("Loading phdr %i to 0x%p (%i bytes)\n",
		      i, dst, phdr->p_filesz);
		if (phdr->p_filesz)
			memcpy(dst, src, phdr->p_filesz);
		if (phdr->p_filesz != phdr->p_memsz)
			memset(dst + phdr->p_filesz, 0x00,
			       phdr->p_memsz - phdr->p_filesz);
		flush_cache((unsigned long)dst & ~(CONFIG_SYS_CACHELINE_SIZE-1),
			    ALIGN(phdr->p_filesz, CONFIG_SYS_CACHELINE_SIZE));
	}

	return ehdr->e_entry;
}

/* Supports firmware files in binary and elf format (using autodetection) */
int do_bootaux(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 stack, pc;
	ulong addr;
	int ret, up;

	if (argc < 2)
		return CMD_RET_USAGE;

	up = arch_auxiliary_core_check_up(0);
	if (up) {
		printf("## Auxiliary core is already up\n");
		return CMD_RET_SUCCESS;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	if (!addr)
		return CMD_RET_FAILURE;

	if (valid_elf_image(addr)) {
		stack = 0x0;
		pc = load_elf_image_phdr(addr);
		if (!pc)
			return CMD_RET_FAILURE;

	} else {
		/*
		 * Assume binary file with vector table at the beginning.
		 * Cortex-M4 vector tables start with the stack pointer (SP)
		 * and reset vector (initial PC).
		 */
		stack = *(u32 *)addr;
		pc = *(u32 *)(addr + 4);
	}

	printf("## Starting auxiliary core at 0x%08X ...\n", pc);

	ret = arch_auxiliary_core_up(0, stack, pc);
	if (ret)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	bootaux, CONFIG_SYS_MAXARGS, 1,	do_bootaux,
	"Start auxiliary core",
	"<addr>\n"
	"Boot firmware at 'addr' on auxiliary core. Firmware formats:\n"
	"  - bin: 'addr' must be the address the fw has been linked to\n"
	"  - elf: 'addr' can be anywhere, relocating according to elf headers\n"
);
