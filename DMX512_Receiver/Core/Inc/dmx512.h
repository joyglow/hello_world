/**
  ******************************************************************************
  * @file           : dmx512.h
  * @brief          : DMX512 receiver header file
  ******************************************************************************
  */

#ifndef __DMX512_H
#define __DMX512_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
    DMX_STATE_IDLE,
    DMX_STATE_BREAK,
    DMX_STATE_MAB,
    DMX_STATE_START_CODE,
    DMX_STATE_DATA
} DMX_State_t;

typedef struct {
    DMX_State_t state;
    uint16_t channel_count;
    uint8_t dmx_data[DMX_CHANNELS];
    uint8_t new_dmx_data[DMX_CHANNELS];
    uint8_t data_received;
    uint32_t last_break_time;
    uint8_t break_detected;
} DMX_Handle_t;

/* Exported constants --------------------------------------------------------*/
#define DMX_BAUDRATE 250000
#define DMX_UART_TIMEOUT 1000

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void DMX512_Init(UART_HandleTypeDef *huart);
void DMX512_Process(void);
uint8_t DMX512_GetChannelValue(uint16_t channel);
void DMX512_UART_RxCallback(uint8_t data);
void DMX512_UART_ErrorCallback(void);
void DMX512_CheckBreak(void);

/* External variables --------------------------------------------------------*/
extern DMX_Handle_t dmx_handle;

#ifdef __cplusplus
}
#endif

#endif /* __DMX512_H */