###############################################################################
# Timing constraints
###############################################################################

create_clock -period 20.000 -name mcu_clk [get_ports MCU_OSC]
create_clock -period 88.000 -name audio_clk [get_ports AUDIO_OSC]
create_clock -period 41.000 -name spi_clk [get_ports SCLK]
create_clock -period 40.000 -name spdif_sck [get_ports SCKI1]
create_clock -period 162.000 -name spdif_bck [get_ports BCKI1]
create_clock -period 162.000 -name adc_bck [get_ports BCKI2]

###############################################################################
# Clock groups
###############################################################################

set_clock_groups -asynchronous -group mcu_clk -group audio_clk -group spi_clk -group spdif_sck -group spdif_bck -group adc_bck

###############################################################################
# IO standard
###############################################################################

set_property IOSTANDARD LVCMOS33 [get_ports *]

###############################################################################
# Location constraints
###############################################################################

# MCU SPI
#set_property PACKAGE_PIN K13 [get_ports INT]
#set_property PACKAGE_PIN P16 [get_ports nSS]
set_property PACKAGE_PIN P16 [get_ports INT]
set_property PACKAGE_PIN K13 [get_ports nSS]
set_property PACKAGE_PIN L14 [get_ports MOSI]
set_property PACKAGE_PIN M16 [get_ports MISO]
set_property PACKAGE_PIN N13 [get_ports SCLK]

# Oscillators
set_property PACKAGE_PIN N14 [get_ports MCU_OSC]
set_property PACKAGE_PIN N11 [get_ports AUDIO_OSC]

# SPDIF routing
set_property PACKAGE_PIN T8 [get_ports SPDIF]
set_property PACKAGE_PIN T9 [get_ports SPDIF_COAX]
set_property PACKAGE_PIN T10 [get_ports SPDIF_TOSLINK]

# SPDIF input
set_property PACKAGE_PIN T7 [get_ports EMPH]
set_property PACKAGE_PIN H13 [get_ports ERROR]
set_property PACKAGE_PIN D13 [get_ports SCKI1]
set_property PACKAGE_PIN H11 [get_ports LRCKI1]
set_property PACKAGE_PIN E11 [get_ports BCKI1]
set_property PACKAGE_PIN H12 [get_ports DIN1]

# ADC input
set_property PACKAGE_PIN B16 [get_ports nOVFL]
set_property PACKAGE_PIN A14 [get_ports SCKO2]
set_property PACKAGE_PIN A13 [get_ports LRCKI2]
set_property PACKAGE_PIN E12 [get_ports BCKI2]
set_property PACKAGE_PIN C14 [get_ports DIN2]

# DAC output
set_property PACKAGE_PIN B11 [get_ports SCKO]
set_property PACKAGE_PIN A10 [get_ports LRCKO]
set_property PACKAGE_PIN A9 [get_ports BCKO]
set_property PACKAGE_PIN A8 [get_ports DOUT]

###############################################################################
# Bitfile constraints
###############################################################################

set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
set_property CONFIG_MODE SPIx4 [current_design]
