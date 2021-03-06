#include "asd_pre_1.h"

#include "ff.h"
#include "microsd.h"
#include "diskio.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <em_timer.h>
#include <em_rmu.h>

#define VOL_SHIFT 3
#define VOL_SLACK 100

#define INIT_MUTE_DELAY 2000
#define MUTE_DELAY 1000

int16_t volume = VOL_MAX;
static int last_phase = 0;
static bool dacMuted = false;
bool mute = false;

int source = 0;
int gain = 0;

int muteCounter = 0;

volatile uint32_t msTicks;
static FATFS FatFs;

bool saveSettings = false;

static bool init = false;

///////////////////////////////////////////////////////////////////////////////

void spdifInit(void) {
  GPIO_PinModeSet(nRST_SPDIF_PORT, nRST_SPDIF_BIT, gpioModePushPull, 1);
}

///////////////////////////////////////////////////////////////////////////////

void adcInit(void) {
  GPIO_PinModeSet(nRST_ADC_PORT, nRST_ADC_BIT, gpioModePushPull, 1);
  GPIO_PinModeSet(nHPF_PORT, nHPF_BIT, gpioModePushPull, 0);
  GPIO_PinModeSet(M0_PORT, M0_BIT, gpioModePushPull, 0);
}

///////////////////////////////////////////////////////////////////////////////

#define SD_RETRIES 10

static bool mounted = false;

void mountUnmount(void) {
  bool card = !GPIO_PinInGet(CARD_DETECT_PORT, CARD_DETECT_BIT);

  if(card && mounted) return;
  if(!card && !mounted) return;

  if(card) {
    // why is this necessary?
    for(int i = 0; i < 10; i++) {
      DSTATUS resCard;
      MICROSD_Init();
      resCard = disk_initialize(0);
      if (!resCard) break;
      msleep(1);    
    }

    // mount
    if (f_mount(&FatFs, (TCHAR*)"", 0) != FR_OK) {
      printf("Can not mount card\n");
      mounted = false;
    }

    mounted = true;
    printf("Card mounted\n");

  } else {
    // umount
    mounted = false;
    f_mount(NULL, (TCHAR*)"", 0);
    printf("Card unmounted\n");
  }
}

void sdInit(void) {
  GPIO_PinModeSet(WRITE_PROTECT_PORT, WRITE_PROTECT_BIT,
                  gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(CARD_DETECT_PORT, CARD_DETECT_BIT,
                  gpioModeInputPullFilter, 1);
  GPIO_IntConfig(CARD_DETECT_PORT, CARD_DETECT_BIT, true, true, true);

  mountUnmount();
}

///////////////////////////////////////////////////////////////////////////////

void paInit(void) {
  // setup mute
  GPIO_PinModeSet(MUTE_PORT, MUTE_BIT, gpioModePushPull, 0);

  // setup volume
  GPIO_PinModeSet(VOL_A_PORT, VOL_A_BIT, gpioModeInput, 0);
  GPIO_PinModeSet(VOL_B_PORT, VOL_B_BIT, gpioModeInput, 0);
  GPIO_IntConfig(VOL_A_PORT, VOL_A_BIT, true, true, true);
  GPIO_IntConfig(VOL_B_PORT, VOL_B_BIT, true, true, true);
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // setup buttons
  GPIO_PinModeSet(SOURCE_PORT, SOURCE_BIT, gpioModeInputPullFilter, 1);
  GPIO_IntConfig(SOURCE_PORT, SOURCE_BIT, false, true, true);

  // setup LEDs
  GPIO_PinModeSet(LED0_PORT, LED0_BIT, gpioModePushPull, LED_ON);
  GPIO_PinModeSet(LED1_PORT, LED1_BIT, gpioModePushPull, LED_ON);
  GPIO_PinModeSet(LED2_PORT, LED2_BIT, gpioModePushPull, LED_OFF);
}

void setMute(bool newMute) {
  mute = newMute;

  GPIO_PinModeSet(MUTE_PORT, MUTE_BIT, gpioModePushPull, mute ? 0 : 1);
  if(mute) {
    clearLed(0);
    clearLed(1);
    clearLed(2);
  } else {
    setLed(source);
  }
}

void setVol(int16_t newVolume) {
  volume = newVolume;

  // set volume on DAC
  if(volume <= VOL_MIN) dacSetVolume(VOL_MIN>>VOL_SHIFT);
  else dacSetVolume(volume>>VOL_SHIFT);

  // mute DAC if volume is below limit
  if(volume >= VOL_MAX) {
    dacMute(true);
    dacMuted = true;
  } else if(dacMuted) {
    dacMute(false);
    dacMuted = false;
  }
}

void updateVol(int a, int b) {
  int phase;

  // detect encoder phase
  if(!a && !b) phase = 0;
  if(a && !b) phase = 1;
  if(a && b) phase = 2;
  if(!a && b) phase = 3;

  // update volume based on encoder direction
  if(((last_phase + 1) % 4) == phase) {
    if(volume > (VOL_MIN-VOL_SLACK)) volume--;
  }
  if(((phase + 1) % 4) == last_phase) {
    if(volume < (VOL_MAX+VOL_SLACK)) volume++;
  }

  setVol(volume);

  // save state
  last_phase = phase;
}

void setLed(int led) {
  switch(led) {
    case 0:
      GPIO_PinOutSet(LED2_PORT, LED2_BIT);
      break;
    case 1:
      GPIO_PinOutClear(LED1_PORT, LED1_BIT);
      break;
    case 2:
      GPIO_PinOutClear(LED0_PORT, LED0_BIT);
      break;
  }
}

void clearLed(int led) {
  switch(led) {
    case 0:
      GPIO_PinOutClear(LED2_PORT, LED2_BIT);
      break;
    case 1:
      GPIO_PinOutSet(LED1_PORT, LED1_BIT);
      break;
    case 2:
      GPIO_PinOutSet(LED0_PORT, LED0_BIT);
      break;
  }
}

void setSource(int newSource) {
  setMute(true);
  // make sure relay is stable before continuing
  for(int i = 0; i < 50; i++) usleep(1000); 
  muteCounter = MUTE_DELAY;
  clearLed(source);
  source = newSource % SOURCES;
  fpgaSelectSource(source);
  saveSettings = true;
  setLed(source);
}

void setGain(int newGain) {
  gain = newGain % (MAX_GAIN+1);
  fpgaSetGain(gain);
  saveSettings = true;
}

void cycleSource(void) {
  // debounce button
  usleep(1000);
  GPIO_IntClear(INT_SOURCE); // remove interrupts that arrived while debouncing

  if(!GPIO_PinInGet(SOURCE_PORT, SOURCE_BIT)) {
    setSource(source + 1);
  }
}

///////////////////////////////////////////////////////////////////////////////

void msleep(uint32_t ms) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < ms) ;
}

void usleep(uint16_t us) {
  TIMER_CounterSet(TIMER0, 0);
  while(TIMER_CounterGet(TIMER0) < (us*3));
}

void panic(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf("\nPanic: ");
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  while(true);
}

///////////////////////////////////////////////////////////////////////////////

int main(void) {
  msTicks = 0;
  init = false;

  CHIP_Init();
   
  // setup clocks
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // setup systick
  SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000);

  /* Select timer parameters */
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale16,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };

  /* Configure TIMER */
  TIMER_Init(TIMER0, &timerInit);

  swoInit();

  printf("AsD Pre 1 initializing\n");

  paInit();

  sdInit();
  configInit();

  volume = getUint32("volume");
  if(volume == 0) volume = VOL_MAX;
  if(volume < VOL_MIN) volume = VOL_MIN;
  if(volume > VOL_MAX) volume = VOL_MAX;

  source = getUint32("source") % SOURCES;
  gain = getUint32("gain") % (MAX_GAIN+1);

  printf("Volume: %d Source: %d Gain: %d\n", volume, source, gain);

  fpgaInit(source, gain);
  spdifInit();
  dacInit(volume>>VOL_SHIFT);
  adcInit();
  netInit();

  init = true;

  muteCounter = 0;

  // wait a while before unmuting
  while(msTicks < INIT_MUTE_DELAY);
  setMute(false);

  clearLed(0);
  clearLed(1);
  clearLed(2);
  setLed(source);

  printf("Ready\n");

  // main loop
  while(true) {
    netPeriodic();

    if(saveSettings) {
      saveSettings = false;
      setUint32("volume", volume);
      setUint32("source", source);
      setUint32("gain", gain);
      flushConfig();
    }

    if(muteCounter == 0) {
      setMute(false);
      muteCounter = -1;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// IRQ handlers

void SysTick_Handler(void) {
  msTicks++;
  if(muteCounter > 0) muteCounter--;
}

void GPIO_EVEN_IRQHandler(void) {
  uint32_t ints = GPIO_IntGet();
  GPIO_IntClear(ints);

  if(init) {
    if(ints & INT_VOL_EVEN) {
      int a = GPIO_PinInGet(VOL_A_PORT, VOL_A_BIT);
      int b = GPIO_PinInGet(VOL_B_PORT, VOL_B_BIT);
      updateVol(a, b);
    }
    if(ints & INT_SOURCE) cycleSource();
    if(ints & INT_ETH) netIRQ();
  }
}

void GPIO_ODD_IRQHandler(void) {
  uint32_t ints = GPIO_IntGet();
  GPIO_IntClear(ints);

  if(init) {
    if(ints & INT_VOL_ODD) {
      int a = GPIO_PinInGet(VOL_A_PORT, VOL_A_BIT);
      int b = GPIO_PinInGet(VOL_B_PORT, VOL_B_BIT);
      updateVol(a, b);
    }
    if(ints & INT_SD) mountUnmount();
  }
}
