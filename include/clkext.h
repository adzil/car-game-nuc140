#include <stdint.h>

uint32_t DrvSYS_GetHCLKFreq(void);
uint32_t DrvSYS_GetPLLClockFreq(void);

#define __XTAL 12000000
#define __RTC_XTAL 32768
#define __IRC10K 10000
#define __IRC22M 22000000
