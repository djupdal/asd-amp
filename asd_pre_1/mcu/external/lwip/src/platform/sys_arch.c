#include "lwip/debug.h"

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

extern volatile uint32_t msTicks;

u32_t sys_now(void) {
  return msTicks;
}
