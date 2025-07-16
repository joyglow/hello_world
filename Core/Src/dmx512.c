/**
  ******************************************************************************
  * @file    dmx512.c
  * @brief   DMX512 Receiver Driver Implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dmx512.h"
#include "string.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t rx_buffer;

/* Private function prototypes -----------------------------------------------*/
static void DMX_ProcessByte(DMX_HandleTypeDef *hdmx, uint8_t data);
static uint8_t DMX_DetectBreak(DMX_HandleTypeDef *hdmx);

/**
  * @brief  Initialize DMX512 receiver
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @param  huart: pointer to UART_HandleTypeDef structure
  * @retval HAL status
  */
HAL_StatusTypeDef DMX_Init(DMX_HandleTypeDef *hdmx, UART_HandleTypeDef *huart)
{
    if (hdmx == NULL || huart == NULL)
        return HAL_ERROR;
    
    /* Initialize DMX handle */
    hdmx->huart = huart;
    hdmx->state = DMX_STATE_IDLE;
    hdmx->current_channel = 0;
    hdmx->channel_count = 0;
    hdmx->frame_complete = 0;
    hdmx->break_detected = 0;
    hdmx->break_timestamp = 0;
    
    /* Clear DMX data buffer */
    memset(hdmx->dmx_data, 0, sizeof(hdmx->dmx_data));
    
    return HAL_OK;
}

/**
  * @brief  Start DMX512 reception
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval HAL status
  */
HAL_StatusTypeDef DMX_Start_Receive(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL || hdmx->huart == NULL)
        return HAL_ERROR;
    
    hdmx->state = DMX_STATE_IDLE;
    hdmx->frame_complete = 0;
    
    /* Start UART reception in interrupt mode */
    return HAL_UART_Receive_IT(hdmx->huart, &rx_buffer, 1);
}

/**
  * @brief  Stop DMX512 reception
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval HAL status
  */
HAL_StatusTypeDef DMX_Stop_Receive(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL || hdmx->huart == NULL)
        return HAL_ERROR;
    
    hdmx->state = DMX_STATE_IDLE;
    
    /* Stop UART reception */
    return HAL_UART_AbortReceive_IT(hdmx->huart);
}

/**
  * @brief  UART Rx Complete Callback for DMX512
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval None
  */
void DMX_UART_RxCpltCallback(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL)
        return;
    
    /* Process received byte */
    DMX_ProcessByte(hdmx, rx_buffer);
    
    /* Continue reception */
    HAL_UART_Receive_IT(hdmx->huart, &rx_buffer, 1);
}

/**
  * @brief  UART Error Callback for DMX512
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval None
  */
void DMX_UART_ErrorCallback(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL)
        return;
    
    uint32_t error = HAL_UART_GetError(hdmx->huart);
    
    /* Check for break condition (framing error) */
    if (error & HAL_UART_ERROR_FE)
    {
        hdmx->break_detected = 1;
        hdmx->break_timestamp = HAL_GetTick();
        hdmx->state = DMX_STATE_BREAK;
        
        /* Clear the error and continue reception */
        __HAL_UART_CLEAR_FLAG(hdmx->huart, UART_CLEAR_FEF);
    }
    
    /* Handle other errors */
    if (error & (HAL_UART_ERROR_PE | HAL_UART_ERROR_NE | HAL_UART_ERROR_ORE))
    {
        hdmx->state = DMX_STATE_ERROR;
        
        /* Clear errors */
        __HAL_UART_CLEAR_FLAG(hdmx->huart, UART_CLEAR_PEF | UART_CLEAR_NEF | UART_CLEAR_OREF);
    }
    
    /* Restart reception */
    HAL_UART_Receive_IT(hdmx->huart, &rx_buffer, 1);
}

/**
  * @brief  Check if DMX frame is complete
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval 1 if frame complete, 0 otherwise
  */
uint8_t DMX_IsFrameComplete(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL)
        return 0;
    
    return hdmx->frame_complete;
}

/**
  * @brief  Get DMX channel data
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @param  channel: DMX channel number (1-512)
  * @retval Channel data value (0-255)
  */
uint8_t DMX_GetChannelData(DMX_HandleTypeDef *hdmx, uint16_t channel)
{
    if (hdmx == NULL || channel == 0 || channel > DMX_MAX_CHANNELS)
        return 0;
    
    return hdmx->dmx_data[channel];
}

/**
  * @brief  Reset DMX frame reception
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval None
  */
void DMX_ResetFrame(DMX_HandleTypeDef *hdmx)
{
    if (hdmx == NULL)
        return;
    
    hdmx->frame_complete = 0;
    hdmx->current_channel = 0;
    hdmx->channel_count = 0;
    hdmx->state = DMX_STATE_IDLE;
}

/**
  * @brief  Process received byte in DMX state machine
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @param  data: received byte
  * @retval None
  */
static void DMX_ProcessByte(DMX_HandleTypeDef *hdmx, uint8_t data)
{
    switch (hdmx->state)
    {
        case DMX_STATE_IDLE:
            /* Wait for break detection */
            break;
            
        case DMX_STATE_BREAK:
            /* After break, expect start code */
            if (DMX_DetectBreak(hdmx))
            {
                hdmx->state = DMX_STATE_START_CODE;
                hdmx->current_channel = 0;
                hdmx->break_detected = 0;
            }
            else
            {
                hdmx->state = DMX_STATE_IDLE;
            }
            break;
            
        case DMX_STATE_START_CODE:
            /* Receive start code (should be 0x00 for standard DMX) */
            if (data == DMX_START_CODE)
            {
                hdmx->dmx_data[0] = data;
                hdmx->current_channel = 1;
                hdmx->state = DMX_STATE_DATA;
            }
            else
            {
                hdmx->state = DMX_STATE_IDLE;
            }
            break;
            
        case DMX_STATE_DATA:
            /* Receive channel data */
            if (hdmx->current_channel <= DMX_MAX_CHANNELS)
            {
                hdmx->dmx_data[hdmx->current_channel] = data;
                hdmx->current_channel++;
                
                /* Check if we've received enough channels */
                if (hdmx->current_channel > 2) // At least channel 1 and 2
                {
                    hdmx->channel_count = hdmx->current_channel - 1;
                    hdmx->frame_complete = 1;
                }
            }
            else
            {
                /* Too many channels, frame complete */
                hdmx->channel_count = DMX_MAX_CHANNELS;
                hdmx->frame_complete = 1;
                hdmx->state = DMX_STATE_IDLE;
            }
            break;
            
        case DMX_STATE_ERROR:
            /* Error state - reset to idle */
            hdmx->state = DMX_STATE_IDLE;
            break;
            
        default:
            hdmx->state = DMX_STATE_IDLE;
            break;
    }
}

/**
  * @brief  Detect DMX break condition
  * @param  hdmx: pointer to DMX_HandleTypeDef structure
  * @retval 1 if valid break detected, 0 otherwise
  */
static uint8_t DMX_DetectBreak(DMX_HandleTypeDef *hdmx)
{
    if (!hdmx->break_detected)
        return 0;
    
    uint32_t current_time = HAL_GetTick();
    uint32_t break_duration = current_time - hdmx->break_timestamp;
    
    /* Check if break duration is sufficient (minimum 88µs) */
    /* Note: This is a simplified check using millisecond timing */
    /* For precise timing, consider using a timer with microsecond resolution */
    if (break_duration >= 1) // At least 1ms (much longer than 88µs minimum)
    {
        return 1;
    }
    
    return 0;
}