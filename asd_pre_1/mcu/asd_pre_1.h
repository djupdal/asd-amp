#ifndef ASD_PRE_1_H
#define ASD_PRE_1_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "efm32gg232f1024.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "em_system.h"
#include "em_chip.h"
 
#define LED_ON  0
#define LED_OFF 1

#define DAC_ADDRESS 0x98

//

#define LED0_BIT 3
#define LED0_PORT gpioPortF
#define LED1_BIT 4
#define LED1_PORT gpioPortF
 
#define PROGRAM_B_BIT 4
#define PROGRAM_B_PORT gpioPortA
#define DONE_BIT 5
#define DONE_PORT gpioPortA
#define FPGA_USART USART2
#define FPGA_USART_CLK cmuClock_USART2
#define FPGA_USART_LOC USART_ROUTE_LOCATION_LOC0
#define FPGA_TX_BIT 2
#define FPGA_TX_PORT gpioPortC
#define FPGA_RX_BIT 3
#define FPGA_RX_PORT gpioPortC
#define FPGA_CLK_BIT 4
#define FPGA_CLK_PORT gpioPortC
//#define FPGA_CS_BIT 5
//#define FPGA_CS_PORT gpioPortC
//#define INT_FPGA_BIT 3
//#define INT_FPGA_PORT gpioPortA
#define FPGA_CS_BIT 3
#define FPGA_CS_PORT gpioPortA
#define INT_FPGA_BIT 5
#define INT_FPGA_PORT gpioPortC

#define nRST_SPDIF_BIT 8
#define nRST_SPDIF_PORT gpioPortB

#define nRST_DAC_BIT 15
#define nRST_DAC_PORT gpioPortE

#define nRST_ADC_BIT 14
#define nRST_ADC_PORT gpioPortE
#define nHPF_BIT 13
#define nHPF_PORT gpioPortE
#define M0_BIT 12
#define M0_PORT gpioPortE

#define WRITE_PROTECT_BIT 6
#define WRITE_PROTECT_PORT gpioPortC
#define CARD_DETECT_BIT 7
#define CARD_DETECT_PORT gpioPortC
#define SD_USART USART0
#define SD_USART_CLK cmuClock_USART0
#define SD_USART_LOC USART_ROUTE_LOCATION_LOC2
#define SD_TX_BIT 11
#define SD_TX_PORT gpioPortC
#define SD_RX_BIT 10
#define SD_RX_PORT gpioPortC
#define SD_CLK_BIT 9
#define SD_CLK_PORT gpioPortC
#define SD_CS_BIT 8
#define SD_CS_PORT gpioPortC

#define MUTE_BIT 12
#define MUTE_PORT gpioPortC

#define nINT_ETH_BIT 4
#define nINT_ETH_PORT gpioPortD
#define ETH_USART USART1
#define ETH_USART_CLK cmuClock_USART1
#define ETH_USART_LOC USART_ROUTE_LOCATION_LOC1
#define ETH_TX_BIT 0
#define ETH_TX_PORT gpioPortD
#define ETH_RX_BIT 1
#define ETH_RX_PORT gpioPortD
#define ETH_CLK_BIT 2
#define ETH_CLK_PORT gpioPortD
#define ETH_CS_BIT 3
#define ETH_CS_PORT gpioPortD

#define VOL_A_BIT 14
#define VOL_A_PORT gpioPortC
#define VOL_B_BIT 15
#define VOL_B_PORT gpioPortC

#define GPIO_0_BIT 6
#define GPIO_0_PORT gpioPortD
#define GPIO_1_BIT 7
#define GPIO_1_PORT gpioPortD
#define GPIO_2_BIT 8
#define GPIO_2_PORT gpioPortD
#define GPIO_3_BIT 13
#define GPIO_3_PORT gpioPortC

#define I2C_SCL_BIT 1
#define I2C_SCL_PORT gpioPortA
#define I2C_SDA_BIT 0
#define I2C_SDA_PORT gpioPortA

///////////////////////////////////////////////////////////////////////////////

#define INT_ETH 0x10
#define INT_GPIO_0 0x40
#define INT_SD 0x80
#define INT_VOL_EVEN 0x4000
#define INT_VOL_ODD 0x8000

#define FPGA_SOURCE_COAX 0
#define FPGA_SOURCE_TOSLINK 1
#define FPGA_SOURCE_ADC 2

void swoInit(void);

void fpgaInit(uint32_t source);
void fpgaSelectSource(uint8_t source);

void dacInit(uint8_t startVol);

void dacMute(bool on);
void dacSetVolume(uint8_t vol);

void setLed(int led);
void clearLed(int led);

void msleep(uint32_t ms);
void usleep(uint16_t us);

void configInit(void);
void flushConfig(void);
uint32_t getUint32(char *id);
char *getString(char *id);
void setUint32(char *id, uint32_t val);
void setString(char *id, char *s);

void panic(const char *fmt, ...);

#endif
