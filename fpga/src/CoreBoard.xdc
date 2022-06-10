#### SYSTEM
set_property IOSTANDARD LVCMOS33 [get_ports sys_clk]
set_property PACKAGE_PIN U22 [get_ports sys_clk]
set_property IOSTANDARD LVCMOS33 [get_ports sys_rst_n]
set_property PACKAGE_PIN P4 [get_ports sys_rst_n]
#set_property IOSTANDARD LVCMOS33 [get_ports {led_1}]
#set_property PACKAGE_PIN T23 [get_ports {led_1}]
#set_property IOSTANDARD LVCMOS33 [get_ports {led_2}]
#set_property PACKAGE_PIN R23 [get_ports {led_2}]

#### U2 odd
# GND 1
# 3V3 3
# GND 5
set_property -dict {PACKAGE_PIN D26  IOSTANDARD LVCMOS33} [get_ports PWRITE]
#set_property -dict {PACKAGE_PIN D25  IOSTANDARD LVCMOS33} [get_ports ];          #9
set_property -dict {PACKAGE_PIN G26  IOSTANDARD LVCMOS33} [get_ports PWAIT]
set_property -dict {PACKAGE_PIN E23 IOSTANDARD LVCMOS33} [get_ports PCK]
set_property -dict {PACKAGE_PIN F22 IOSTANDARD LVCMOS33} [get_ports {PWD[0]}]
set_property -dict {PACKAGE_PIN J26  IOSTANDARD LVCMOS33} [get_ports {PWD[1]}]
#set_property -dict {PACKAGE_PIN G21 IOSTANDARD LVCMOS33} [get_ports ];           #19
#set_property -dict {PACKAGE_PIN H22 IOSTANDARD LVCMOS33} [get_ports ];           #21
set_property -dict {PACKAGE_PIN J21 IOSTANDARD LVCMOS33} [get_ports {PRD[1]}]
set_property -dict {PACKAGE_PIN K26 IOSTANDARD LVCMOS33} [get_ports {PRD[0]}]
#set_property -dict {PACKAGE_PIN K23  IOSTANDARD LVCMOS33} [get_ports ];          #27
set_property -dict {PACKAGE_PIN M26  IOSTANDARD LVCMOS33} [get_ports RXD]
set_property -dict {PACKAGE_PIN L23  IOSTANDARD LVCMOS33} [get_ports TXD]
#set_property -dict {PACKAGE_PIN P26  IOSTANDARD LVCMOS33} [get_ports ];          #33
#set_property -dict {PACKAGE_PIN M25  IOSTANDARD LVCMOS33} [get_ports ];          #35
#set_property -dict {PACKAGE_PIN N22  IOSTANDARD LVCMOS33} [get_ports ];          #37
#set_property -dict {PACKAGE_PIN P24 IOSTANDARD LVCMOS33}  [get_ports ];          #39
#set_property -dict {PACKAGE_PIN P25 IOSTANDARD LVCMOS33}  [get_ports ];          #41
#set_property -dict {PACKAGE_PIN T25 IOSTANDARD LVCMOS33}  [get_ports ];          #43
#set_property -dict {PACKAGE_PIN V21  IOSTANDARD LVCMOS33} [get_ports ];          #45
#set_property -dict {PACKAGE_PIN W23  IOSTANDARD LVCMOS33} [get_ports ];          #47
#set_property -dict {PACKAGE_PIN Y23  IOSTANDARD LVCMOS33} [get_ports ];          #49
#set_property -dict {PACKAGE_PIN AA25 IOSTANDARD LVCMOS33} [get_ports ];          #51
#set_property -dict {PACKAGE_PIN AC24 IOSTANDARD LVCMOS33} [get_ports ];          #53
#set_property -dict {PACKAGE_PIN Y21  IOSTANDARD LVCMOS33} [get_ports ];          #55
#set_property -dict {PACKAGE_PIN Y26  IOSTANDARD LVCMOS33} [get_ports ];          #57
#set_property -dict {PACKAGE_PIN AC26 IOSTANDARD LVCMOS33} [get_ports ];          #59
# GND 61
# VIN 63
