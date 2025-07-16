#ifndef _CMSIS_CORE_CM3_H_
#define _CMSIS_CORE_CM3_H_

#include <stdint.h>

#ifndef   __IO
#define   __IO volatile
#endif

/* NVIC simplified stub */
static inline void NVIC_EnableIRQ(int32_t IRQn) { (void)IRQn; }

#endif /* _CMSIS_CORE_CM3_H_ */