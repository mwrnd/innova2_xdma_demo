
# Nvidia Mellanox Innova-2 Flex Open Programmable SmartNIC
# MNV303212A-ADLT
# Kintex Ultrascale+ xcku15p-ffve1517-2-i
# https://www.nvidia.com/en-us/networking/ethernet/innova-2-flex/


# LEDs - A6=D19, B6=D18 - A6 and B6 are in Bank 90
set_property PACKAGE_PIN A6 [get_ports {Dout_0[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {Dout_0[0]}]
set_property OFFCHIP_TERM NONE [get_ports {Dout_0[0]}]

set_property PACKAGE_PIN B6 [get_ports {gpio_io_o_0[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_io_o_0[0]}]
set_property OFFCHIP_TERM NONE [get_ports {gpio_io_o_0[0]}]


# PCIe - PCIE4 Location: X0Y2 - GTY Quads 127, 128
# GTY Quad 127 == Quad_X0Y0
# AH36=MGTYRXP0=CHANNEL_X0Y0, AG38=MGTYRXP1=CHANNEL_X0Y1,
# AF36=MGTYRXP2=CHANNEL_X0Y2, AE38=MGTYRXP3=CHANNEL_X0Y3
# GTY Quad 128 == Quad_X0Y1
# AD36=MGTYRXP0=CHANNEL_X0Y4, AC38=MGTYRXP1=CHANNEL_X0Y5,
# AB36=MGTYRXP2=CHANNEL_X0Y6, AA38=MGTYRXP3=CHANNEL_X0Y7
set_property PACKAGE_PIN AA38 [get_ports {pcie_7x_mgt_rtl_0_rxp[7]}]
set_property PACKAGE_PIN AB36 [get_ports {pcie_7x_mgt_rtl_0_rxp[6]}]
set_property PACKAGE_PIN AC38 [get_ports {pcie_7x_mgt_rtl_0_rxp[5]}]
set_property PACKAGE_PIN AD36 [get_ports {pcie_7x_mgt_rtl_0_rxp[4]}]
set_property PACKAGE_PIN AE38 [get_ports {pcie_7x_mgt_rtl_0_rxp[3]}]
set_property PACKAGE_PIN AF36 [get_ports {pcie_7x_mgt_rtl_0_rxp[2]}]
set_property PACKAGE_PIN AG38 [get_ports {pcie_7x_mgt_rtl_0_rxp[1]}]
set_property PACKAGE_PIN AH36 [get_ports {pcie_7x_mgt_rtl_0_rxp[0]}]

# PCIe - Reset - F2 is in Bank 90
set_property PACKAGE_PIN F2 [get_ports reset_rtl_0]
set_property IOSTANDARD LVCMOS33 [get_ports reset_rtl_0]
set_property PULLUP true [get_ports reset_rtl_0]
set_false_path -from [get_ports reset_rtl_0]

# PCIe - Clock - AB27 is MGTREFCLK0 in GTY Quad/Bank 128
set_property PACKAGE_PIN AB27 [get_ports diff_clock_rtl_0_clk_p]
create_clock -name sys_clk -period 10.000 [get_ports diff_clock_rtl_0_clk_p]



# OpenCAPI - PCIE4 Location: X0Y3 - GTY Quads 131, 132
# GTY Quad 131 == Quad_X0Y4
# M36=MGTYRXP0=CHANNEL_X0Y16, L38=MGTYRXP1=CHANNEL_X0Y17,
# K36=MGTYRXP2=CHANNEL_X0Y18, J38=MGTYRXP3=CHANNEL_X0Y19
# GTY Quad 132 == Quad_X0Y5
# H36=MGTYRXP0=CHANNEL_X0Y20, G38=MGTYRXP1=CHANNEL_X0Y21,
# E38=MGTYRXP2=CHANNEL_X0Y22, C38=MGTYRXP3=CHANNEL_X0Y23
#set_property PACKAGE_PIN M36 [get_ports {pcie_7x_mgt_rtl_0_rxp[0]}]
#set_property PACKAGE_PIN L38 [get_ports {pcie_7x_mgt_rtl_0_rxp[1]}]
#set_property PACKAGE_PIN K36 [get_ports {pcie_7x_mgt_rtl_0_rxp[2]}]
#set_property PACKAGE_PIN J38 [get_ports {pcie_7x_mgt_rtl_0_rxp[3]}]
#set_property PACKAGE_PIN H36 [get_ports {pcie_7x_mgt_rtl_0_rxp[4]}]
#set_property PACKAGE_PIN G38 [get_ports {pcie_7x_mgt_rtl_0_rxp[5]}]
#set_property PACKAGE_PIN E38 [get_ports {pcie_7x_mgt_rtl_0_rxp[6]}]
#set_property PACKAGE_PIN C38 [get_ports {pcie_7x_mgt_rtl_0_rxp[7]}]

# OpenCAPI Clock Pins T27 and P27 are connected together
# OpenCAPI - Clock - T27 is MGTREFCLK0 in GTY_Quad/Bank 131 - X0Y4
#set_property PACKAGE_PIN T27 [get_ports diff_clock_rtl_0_clk_p]
#create_clock -name sys_clk -period 10.000 [get_ports diff_clock_rtl_0_clk_p]

# OpenCAPI - Clock - P27 is MGTREFCLK0 in GTY_Quad/Bank 132 - X0Y5
#set_property PACKAGE_PIN P27 [get_ports {diff_clock_rtl_0_clk_p[0]}]
#create_clock -name sys_clk -period 10.000 [get_ports diff_clock_rtl_0_clk_p]

# OpenCAPI - Reset - C4 is in Bank 90
#set_property PACKAGE_PIN C4 [get_ports reset_rtl_0]
#set_property IOSTANDARD LVCMOS33 [get_ports reset_rtl_0]
#set_property PULLUP true [get_ports reset_rtl_0]
#set_false_path -from [get_ports reset_rtl_0]



# Differential System Clock 100MHz - Bank 65
set_property PACKAGE_PIN AR14 [get_ports {sys_clk_100MHz_clk_p[0]}]
set_property IOSTANDARD DIFF_SSTL12 [get_ports {sys_clk_100MHz_clk_p[0]}]
create_clock -period 10.000 -name sys_clk_100mhz [get_ports sys_clk_100MHz_clk_p]



# EMCCLK Clock 150MHz - Bank 65
set_property PACKAGE_PIN AM14 [get_ports {emc_clk_150MHz[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {emc_clk_150MHz[0]}]
set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets emc_clk_150MHz_IBUF[0]_inst/O]


# MNV303212A-ADIT and MNV303212A-ADAT Only:
# SFP - CMAC X0Y0 and X0Y1
# GTY Quad 129 == Quad_X0Y2
# AD36=MGTYRXP0=CHANNEL_X0Y8, AC38=MGTYRXP1=CHANNEL_X0Y9,
# AB36=MGTYRXP2=CHANNEL_X0Y10, AA38=MGTYRXP3=CHANNEL_X0Y11
# Y27=MGTREFCLK0_P, W29=MGTREFCLK1_P
# GTY Quad 130 == Quad_X0Y3
# AD36=MGTYRXP0=CHANNEL_X0Y12, AC38=MGTYRXP1=CHANNEL_X0Y13,
# AB36=MGTYRXP2=CHANNEL_X0Y14, AA38=MGTYRXP3=CHANNEL_X0Y15
# V27=MGTREFCLK0_P, U29=MGTREFCLK1_P

#set_property PACKAGE_PIN V27 [get_ports diff_clock_rtl_2_clk_p]
#create_clock -period 6.206 -name sys_clk [get_ports diff_clock_rtl_2_clk_p]

#set_property PACKAGE_PIN Y27 [get_ports diff_clock_rtl_3_clk_p]
#create_clock -period 6.206 -name sys_clk [get_ports diff_clock_rtl_3_clk_p]


# Memory Configuration File Settings
set_property CONFIG_MODE SPIx8 [current_design]
set_property CONFIG_VOLTAGE 1.8 [current_design]
set_property CFGBVS GND [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 127.5 [current_design]
set_property BITSTREAM.CONFIG.CONFIGFALLBACK DISABLE [current_design]
set_property BITSTREAM.CONFIG.EXTMASTERCCLK_EN DISABLE [current_design]
set_property BITSTREAM.CONFIG.NEXT_CONFIG_REBOOT DISABLE [current_design]
set_property BITSTREAM.CONFIG.OVERTEMPSHUTDOWN ENABLE [current_design]
set_property BITSTREAM.CONFIG.SPI_32BIT_ADDR YES [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 8 [current_design]
set_property BITSTREAM.CONFIG.SPI_FALL_EDGE YES [current_design]
set_property BITSTREAM.CONFIG.UNUSEDPIN PULLUP [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.GENERAL.CRC ENABLE [current_design]


set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets clk]

