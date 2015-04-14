#include "asd_pre_1.h"

#include <stdio.h>
#include <em_usart.h>

#define CMD_MAGIC 0x01
#define CMD_SOURCE 0x02

#define MAGIC 0xad

void fpgaSelectSource(uint8_t source) {
  // assert CS */
  GPIO_PinOutClear(FPGA_CS_PORT, FPGA_CS_BIT);

  USART_SpiTransfer(FPGA_USART, CMD_SOURCE);
  USART_SpiTransfer(FPGA_USART, source);

  // release CS
  GPIO_PinOutSet(FPGA_CS_PORT, FPGA_CS_BIT);
}

static uint8_t readMagic(void) {
  // assert CS
  GPIO_PinOutClear(FPGA_CS_PORT, FPGA_CS_BIT);

  USART_SpiTransfer(FPGA_USART, CMD_MAGIC);
  uint8_t rx = USART_SpiTransfer(FPGA_USART, 0);

  // release CS
  GPIO_PinOutSet(FPGA_CS_PORT, FPGA_CS_BIT);

  return rx;
}

void fpgaInit(uint32_t source) {
  // set up pins
  GPIO_PinModeSet(INT_FPGA_PORT, INT_FPGA_BIT, gpioModeInput, 0);
  GPIO_PinModeSet(PROGRAM_B_PORT, PROGRAM_B_BIT, gpioModePushPull, 1);
  GPIO_PinModeSet(DONE_PORT, DONE_BIT, gpioModeInput, 0);

  // wait until the FPGA is configured
  int done;
  do {
    done = GPIO_PinInGet(DONE_PORT, DONE_BIT);
  } while(!done);

  /////////////////////////////////////////////////////////////////////////////
  // setup spi
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  // configure spi pins
  GPIO_PinModeSet(FPGA_TX_PORT, FPGA_TX_BIT, gpioModePushPull, 1);
  GPIO_PinModeSet(FPGA_RX_PORT, FPGA_RX_BIT, gpioModeInput, 0);
  GPIO_PinModeSet(FPGA_CLK_PORT, FPGA_CLK_BIT, gpioModePushPull, 0);
  GPIO_PinModeSet(FPGA_CS_PORT, FPGA_CS_BIT, gpioModePushPull, 1);

  USART_Reset(FPGA_USART);

  // enable clock
  CMU_ClockEnable(FPGA_USART_CLK, true);

  // configure
  init.baudrate = 12000000;
  init.msbf     = true;
  USART_InitSync(FPGA_USART, &init);  
  
  FPGA_USART->ROUTE = (FPGA_USART->ROUTE & ~_USART_ROUTE_LOCATION_MASK) |
                      FPGA_USART_LOC;

  FPGA_USART->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN |
                       USART_ROUTE_CLKPEN;

  //////////////////////////////////////////////////////////////////////////////
  // read magic

  uint8_t magic;
  magic = readMagic();
  printf("Got FPGA magic number\n");
  if(magic != MAGIC) panic("Incorrect FPGA magic number '%x'", magic);

  fpgaSelectSource(source);
}
