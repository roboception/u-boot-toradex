/*
 * Copyright (c) 2017 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

struct tegra_pcie_port;

int tegra_pcie_port_index_of_port(struct tegra_pcie_port *port);

void tegra_pcie_port_reset(struct tegra_pcie_port *port);
