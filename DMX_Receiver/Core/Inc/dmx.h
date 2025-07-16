#ifndef _DMX_H_
#define _DMX_H_

#include "stm32f1xx_hal.h"

#define DMX_CHANNEL 2u
#define DMX_UNIVERSE_SIZE 513u

void DMX_Init(UART_HandleTypeDef *huart);
uint8_t DMX_GetChannelLevel(uint16_t ch);

#endif /* _DMX_H_ */