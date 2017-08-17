/*
 * Copyright (c) 2016-2017 Toradex, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <asm/arch-tegra/ap.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include <pci_tegra.h>
#include <power/as3722.h>
#include <i2c.h>

#include "../common/tdx-common.h"
#include "pinmux-config-apalis-tk1.h"

#define LAN_DEV_OFF_N	TEGRA_GPIO(O, 6)
#define LAN_RESET_N	TEGRA_GPIO(S, 2)
#define LAN_WAKE_N	TEGRA_GPIO(O, 5)
#ifdef CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT
#define PEX_PERST_N	TEGRA_GPIO(DD, 1) /* Apalis GPIO7 */
#define RESET_MOCI_CTRL	TEGRA_GPIO(U, 4)
#endif /* CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT */

#define LEDCTRL_I2CBUS 0x1
#define LEDCTRL_ADDR 0x32

int arch_misc_init(void)
{
  // mapping of leds is as follows:
  // red   | D7
  // blue  | D2
  // green | D1

  // engine firmare for led blinking
  unsigned char fwEng1[] = {0x9d,0x01,0x40,0xFF,0x60,0x00,0x40,0x00,0x60,0x00,0x00,0x00};
  unsigned char fwEng2[] = {0x9d,0x02,0x40,0xFF,0x60,0x00,0x40,0x00,0x60,0x00,0x00,0x00};
  unsigned char fwEng3[] = {0x9d,0x07,0x40,0xFF,0x60,0x00,0x40,0x00,0x60,0x00,0x00,0x00};

  printf("Initializing leds...\n");

  unsigned int oldBus = i2c_get_bus_num();
  i2c_set_bus_num(LEDCTRL_I2CBUS);

  // enable led driver and programmable engines [register: ENABLE ENGINE CNTRL1]
  i2c_reg_write(LEDCTRL_ADDR, 0x00, 0x6a);

  // configure pwm [register: MISC]
  i2c_reg_write(LEDCTRL_ADDR, 0x36, 0x7E);

  // set currents for leds [registers: Current Control]
  i2c_reg_write(LEDCTRL_ADDR, 0x2c, 0x03);
  i2c_reg_write(LEDCTRL_ADDR, 0x26, 0x02);
  i2c_reg_write(LEDCTRL_ADDR, 0x27, 0x04);

  // enable output [register: OUTPUT ON/OFF CONTROL LSB]
  i2c_reg_write(LEDCTRL_ADDR, 0x05, 0xff);

  // switch on all three leds (->white) [registers: PWM]
  i2c_reg_write(LEDCTRL_ADDR, 0x1c, 0xff);
  i2c_reg_write(LEDCTRL_ADDR, 0x16, 0xff);
  i2c_reg_write(LEDCTRL_ADDR, 0x17, 0xff);

  // set programming mode for all engines [registers: ENGINE CNTRL2]
  i2c_reg_write(LEDCTRL_ADDR, 0x01, 0x15);

  // set pc for engine1 to memory page0 [registers: ENGINE1 PC, ENG1 PROG START ADDR]
  i2c_reg_write(LEDCTRL_ADDR, 0x37, 0x00);
  i2c_reg_write(LEDCTRL_ADDR, 0x4c, 0x00);
  // select memory page0 [register: PROG MEM PAGE SEL]
  i2c_reg_write(LEDCTRL_ADDR, 0x4f, 0x00);
  // write firmware for engine1 [register: PROGRAM MEMORY]
  i2c_write(LEDCTRL_ADDR, 0x50, 1, fwEng1, sizeof(fwEng1));

  // set pc for engine2 to memory page2
  i2c_reg_write(LEDCTRL_ADDR, 0x38, 0x20);
  i2c_reg_write(LEDCTRL_ADDR, 0x4d, 0x20);
  // select memory page2
  i2c_reg_write(LEDCTRL_ADDR, 0x4f, 0x02);
  // write firmware for engine2
  i2c_write(LEDCTRL_ADDR, 0x50, 1, fwEng2, sizeof(fwEng2));

  // set pc for engine3 to memory page4
  i2c_reg_write(LEDCTRL_ADDR, 0x38, 0x40);
  i2c_reg_write(LEDCTRL_ADDR, 0x4e, 0x40);
  // select memory page2
  i2c_reg_write(LEDCTRL_ADDR, 0x4f, 0x04);
  // write firmware for engine3
  i2c_write(LEDCTRL_ADDR, 0x50, 1, fwEng3, sizeof(fwEng3));

  // enable led driver and programmable engines
  //i2c_reg_write(LEDCTRL_ADDR, 0x00, 0x6a);
  
  // disable all engines
  i2c_reg_write(LEDCTRL_ADDR, 0x01, 0x00);

  // restore old bus configuration
  i2c_set_bus_num(oldBus);


  if (readl(NV_PA_BASE_SRAM + NVBOOTINFOTABLE_BOOTTYPE) ==
      NVBOOTTYPE_RECOVERY)
  {
    printf("USB recovery mode, disabled autoboot\n");
    setenv("bootdelay", "-1");
  }

  return 0;
}

int checkboard(void)
{
	puts("Model: Toradex Apalis TK1 2GB\n");

	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	return ft_common_board_setup(blob, bd);
}
#endif

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_clear_tristate_input_clamping();

	gpio_config_table(apalis_tk1_gpio_inits,
			  ARRAY_SIZE(apalis_tk1_gpio_inits));

	pinmux_config_pingrp_table(apalis_tk1_pingrps,
				   ARRAY_SIZE(apalis_tk1_pingrps));

	pinmux_config_drvgrp_table(apalis_tk1_drvgrps,
				   ARRAY_SIZE(apalis_tk1_drvgrps));
}

#ifdef CONFIG_PCI_TEGRA
int tegra_pcie_board_init(void)
{
	struct udevice *pmic;
	int err;

	err = as3722_init(&pmic);
	if (err) {
		error("failed to initialize AS3722 PMIC: %d\n", err);
		return err;
	}

	err = as3722_sd_enable(pmic, 4);
	if (err < 0) {
		error("failed to enable SD4: %d\n", err);
		return err;
	}

	err = as3722_sd_set_voltage(pmic, 4, 0x24);
	if (err < 0) {
		error("failed to set SD4 voltage: %d\n", err);
		return err;
	}

	err = as3722_gpio_configure(pmic, 1, AS3722_GPIO_OUTPUT_VDDH |
					     AS3722_GPIO_INVERT);
	if (err < 0) {
		error("failed to configure GPIO#1 as output: %d\n", err);
		return err;
	}

	err = as3722_gpio_direction_output(pmic, 2, 1);
	if (err < 0) {
		error("failed to set GPIO#2 high: %d\n", err);
		return err;
	}

	gpio_request(LAN_DEV_OFF_N, "LAN_DEV_OFF_N");
	gpio_request(LAN_RESET_N, "LAN_RESET_N");
	gpio_request(LAN_WAKE_N, "LAN_WAKE_N");

#ifdef CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT
	gpio_request(PEX_PERST_N, "PEX_PERST_N");
	gpio_request(RESET_MOCI_CTRL, "RESET_MOCI_CTRL");
#endif /* CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT */

	return 0;
}

void tegra_pcie_board_port_reset(struct tegra_pcie_port *port)
{
	int index = tegra_pcie_port_index_of_port(port);
	if (index == 1) { /* I210 Gigabit Ethernet Controller (On-module) */
		struct udevice *pmic;
		int err;

		err = as3722_init(&pmic);
		if (err) {
			error("failed to initialize AS3722 PMIC: %d\n", err);
			return;
		}

		/* Reset I210 Gigabit Ethernet Controller */
		gpio_direction_output(LAN_RESET_N, 0);

		/*
		 * Make sure we don't get any back feeding from DEV_OFF_N resp.
		 * LAN_WAKE_N
		 */
		gpio_direction_output(LAN_DEV_OFF_N, 0);
		gpio_direction_output(LAN_WAKE_N, 0);

		/* Make sure LDO9 and LDO10 are initially enabled @ 0V */
		err = as3722_ldo_enable(pmic, 9);
		if (err < 0) {
			error("failed to enable LDO9: %d\n", err);
			return;
		}
		err = as3722_ldo_enable(pmic, 10);
		if (err < 0) {
			error("failed to enable LDO10: %d\n", err);
			return;
		}
		err = as3722_ldo_set_voltage(pmic, 9, 0x80);
		if (err < 0) {
			error("failed to set LDO9 voltage: %d\n", err);
			return;
		}
		err = as3722_ldo_set_voltage(pmic, 10, 0x80);
		if (err < 0) {
			error("failed to set LDO10 voltage: %d\n", err);
			return;
		}

		/* Make sure controller gets enabled by disabling DEV_OFF_N */
		gpio_set_value(LAN_DEV_OFF_N, 1);

		/*
		 * Enable LDO9 and LDO10 for +V3.3_ETH on patched prototype
		 * V1.0A and sample V1.0B and newer modules
		 */
		err = as3722_ldo_set_voltage(pmic, 9, 0xff);
		if (err < 0) {
			error("failed to set LDO9 voltage: %d\n", err);
			return;
		}
		err = as3722_ldo_set_voltage(pmic, 10, 0xff);
		if (err < 0) {
			error("failed to set LDO10 voltage: %d\n", err);
			return;
		}

		/*
		 * Must be asserted for 100 ms after power and clocks are stable
		 */
		mdelay(100);

		gpio_set_value(LAN_RESET_N, 1);
	} else if (index == 0) { /* Apalis PCIe */
#ifdef CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT
		/*
		 * Reset PLX PEX 8605 PCIe Switch plus PCIe devices on Apalis
		 * Evaluation Board
		 */
		gpio_direction_output(PEX_PERST_N, 0);
		gpio_direction_output(RESET_MOCI_CTRL, 0);

		/*
		 * Must be asserted for 100 ms after power and clocks are stable
		 */
		mdelay(100);

		gpio_set_value(PEX_PERST_N, 1);
		/*
		 * Err_5: PEX_REFCLK_OUTpx/nx Clock Outputs is not Guaranteed
		 * Until 900 us After PEX_PERST# De-assertion
		 */
		mdelay(1);
		gpio_set_value(RESET_MOCI_CTRL, 1);
#endif /* CONFIG_APALIS_TK1_PCIE_EVALBOARD_INIT */
	}
}
#endif /* CONFIG_PCI_TEGRA */
