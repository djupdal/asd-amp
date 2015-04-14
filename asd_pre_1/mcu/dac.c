///////////////////////////////////////////////////////////////////////////////
//
// DAC
//
// Asbjørn Djupdal 2014
//
///////////////////////////////////////////////////////////////////////////////

#include "asd_pre_1.h"

#include <stdio.h>

#define REG_CHIP_ID         1
#define REG_MODE_CTRL       2
#define REG_VOL_MIX_INV     3
#define REG_MUTE_CTRL       4
#define REG_VOLUME_A        5
#define REG_VOLUME_B        6
#define REG_RAMP_FILTER     7
#define REG_MISC_CTRL       8
#define REG_MISC_CTRL_2     9

#define CHIP_ID 0x70
#define REV_MASK 0x7

///////////////////////////////////////////////////////////////////////////////

static void dacWriteReg(uint8_t reg, uint8_t value) {
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef ret;
  uint8_t buf[] = {reg, value};

  i2cTransfer.addr          = DAC_ADDRESS;
  i2cTransfer.flags         = I2C_FLAG_WRITE;
  i2cTransfer.buf[0].data   = buf;
  i2cTransfer.buf[0].len    = 2;
  i2cTransfer.buf[1].data   = NULL;
  i2cTransfer.buf[1].len    = 0;

  ret = I2C_TransferInit(I2C0, &i2cTransfer);
  while (ret == i2cTransferInProgress) {
    ret = I2C_Transfer(I2C0);
  }
}

static uint8_t dacReadReg(uint32_t reg) {
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef ret;
  uint8_t buf = reg;

  i2cTransfer.addr          = DAC_ADDRESS;
  i2cTransfer.flags         = I2C_FLAG_WRITE;
  i2cTransfer.buf[0].data   = &buf;
  i2cTransfer.buf[0].len    = 1;
  i2cTransfer.buf[1].data   = NULL;
  i2cTransfer.buf[1].len    = 0;
  ret = I2C_TransferInit(I2C0, &i2cTransfer);
  while (ret == i2cTransferInProgress) {
    ret = I2C_Transfer(I2C0);
  }

  buf = 0;

  i2cTransfer.addr          = DAC_ADDRESS;
  i2cTransfer.flags         = I2C_FLAG_READ;
  i2cTransfer.buf[0].data   = &buf;
  i2cTransfer.buf[0].len    = 1;
  i2cTransfer.buf[1].data   = NULL;
  i2cTransfer.buf[1].len    = 0;

  ret = I2C_TransferInit(I2C0, &i2cTransfer);
  while (ret == i2cTransferInProgress) {
    ret = I2C_Transfer(I2C0);
  }

  return buf;
}

///////////////////////////////////////////////////////////////////////////////

void dacSetVolume(uint8_t vol) {
  dacWriteReg(REG_VOLUME_A, vol);
}

void dacMute(bool on) {
  if(on) {
    dacWriteReg(REG_MUTE_CTRL, 0xf8);
  } else {
    dacWriteReg(REG_MUTE_CTRL, 0xe0);
  }
}

void dacInit(uint8_t startVol) {
  CMU_ClockEnable(cmuClock_I2C0, true);

  GPIO_PinModeSet(nRST_DAC_PORT, nRST_DAC_BIT, gpioModePushPull, 1);
  GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_BIT, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_BIT, gpioModeWiredAndPullUpFilter, 1);

  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
  I2C0->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN;
  I2C_Init(I2C0, &i2cInit);
  
  // set CPEN bit to enable control port
  dacWriteReg(REG_MISC_CTRL, 0xc0);

  // setup dac
  uint8_t version = dacReadReg(REG_CHIP_ID);
  if((version & ~REV_MASK) != CHIP_ID) {
    panic("Can't find DAC");
  }
  printf("DAC revision %d\n", version & REV_MASK);
  dacWriteReg(REG_MUTE_CTRL, 0xe0);
  dacWriteReg(REG_VOL_MIX_INV, 0x89);

  printf("Setting initial volume to %d\n", startVol);
  dacSetVolume(startVol);

  // clear PEN bit to end power-up sequence
  dacWriteReg(REG_MISC_CTRL, 0x40);
}
