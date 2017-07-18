/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <exports.h>

/*
 * Make thumb work by providing a forwarder to the (thumb) entry point
 * compiled for arm instruction set.
 * Don't compile this for thumb only CPUs.
 */
#if defined(__thumb__) && defined(__ARM_ARCH_ISA_ARM)
void __attribute__((unused)) __attribute__ ((naked)) dummy2 (void)
{
asm volatile ( \
"	.code 32\n" \
"	.arm\n" \
"	ldr pc,=hello_world\n" );
}
#endif

int hello_world (int argc, char * const argv[])
{
	int i;

	/* Print the ABI version */
	app_startup(argv);
	printf ("Example expects ABI version %d\n", XF_VERSION);
	printf ("Actual U-Boot ABI version %d\n", (int)get_version());

	printf ("Hello World\n");

	printf ("argc = %d\n", argc);

	for (i=0; i<=argc; ++i) {
		printf ("argv[%d] = \"%s\"\n",
			i,
			argv[i] ? argv[i] : "<NULL>");
	}

	printf ("Hit any key to exit ... ");
	while (!tstc())
		;
	/* consume input */
	(void) getc();

	printf ("\n\n");
	return (0);
}
