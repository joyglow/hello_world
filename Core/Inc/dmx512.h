/**
  ******************************************************************************
  * @file    dmx512.h
  * @brief   DMX512 Receiver Driver Header File
  ******************************************************************************
  */

#ifndef __DMX512_H
#define __DMX512_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
    DMX_STATE_IDLE = 0,
    DMX_STATE_BREAK,
    DMX_STATE_START_CODE,
    DMX_STATE_DATA,
    DMX_STATE_ERROR
} DMX_StateTypeDef;

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t dmx_data[513];      // DMX data buffer (0-512 channels, index 0 = start code)
    uint16_t channel_count;     // Number of received channels
    uint16_t current_channel;   // Current channel being received
    DMX_StateTypeDef state;     // Current DMX state
    uint32_t break_timestamp;   // Timestamp for break detection
    uint8_t frame_complete;     // Flag indicating complete frame reception
    uint8_t break_detected;     // Flag indicating break detection
} DMX_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
#define DMX_MAX_CHANNELS        512
#define DMX_START_CODE          0x00
#define DMX_BREAK_MIN_TIME      88      // Minimum break time in microseconds
#define DMX_MAB_MIN_TIME        8       // Minimum Mark After Break time in microseconds

/* Exported macro ------------------------------------------------------------*/
#define DMX_GET_CHANNEL(hdmx, channel) ((channel > 0 && channel <= DMX_MAX_CHANNELS) ? \
                                        hdmx->dmx_data[channel] : 0)

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef DMX_Init(DMX_HandleTypeDef *hdmx, UART_HandleTypeDef *huart);
HAL_StatusTypeDef DMX_Start_Receive(DMX_HandleTypeDef *hdmx);
HAL_StatusTypeDef DMX_Stop_Receive(DMX_HandleTypeDef *hdmx);
void DMX_UART_RxCpltCallback(DMX_HandleTypeDef *hdmx);
void DMX_UART_ErrorCallback(DMX_HandleTypeDef *hdmx);
uint8_t DMX_IsFrameComplete(DMX_HandleTypeDef *hdmx);
uint8_t DMX_GetChannelData(DMX_HandleTypeDef *hdmx, uint16_t channel);
void DMX_ResetFrame(DMX_HandleTypeDef *hdmx);

#ifdef __cplusplus
}
#endif

#endif /* __DMX512_H */