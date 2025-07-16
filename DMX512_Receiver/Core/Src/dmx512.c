/**
  ******************************************************************************
  * @file           : dmx512.c
  * @brief          : DMX512 receiver implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dmx512.h"
#include "string.h"

/* Private variables ---------------------------------------------------------*/
DMX_Handle_t dmx_handle;
static UART_HandleTypeDef *dmx_uart;

/* Private function prototypes -----------------------------------------------*/
static void DMX512_Reset(void);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize DMX512 receiver
  * @param  huart: UART handle pointer
  * @retval None
  */
void DMX512_Init(UART_HandleTypeDef *huart)
{
    dmx_uart = huart;
    
    /* Initialize DMX handle */
    dmx_handle.state = DMX_STATE_IDLE;
    dmx_handle.channel_count = 0;
    dmx_handle.data_received = 0;
    dmx_handle.break_detected = 0;
    dmx_handle.last_break_time = 0;
    
    /* Clear DMX data buffers */
    memset(dmx_handle.dmx_data, 0, DMX_CHANNELS);
    memset(dmx_handle.new_dmx_data, 0, DMX_CHANNELS);
    
    /* Configure UART for DMX512 */
    huart->Instance->CR1 &= ~USART_CR1_UE;  // Disable UART
    
    /* Set 250kbaud, 8 data bits, 2 stop bits, no parity */
    huart->Init.BaudRate = DMX_BAUDRATE;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_2;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(huart) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Enable UART receive interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
    
    /* Enable framing error interrupt for break detection */
    __HAL_UART_ENABLE_IT(huart, UART_IT_FE);
}

/**
  * @brief  Process DMX512 data
  * @retval None
  */
void DMX512_Process(void)
{
    /* Check for timeout */
    if ((HAL_GetTick() - dmx_handle.last_break_time) > 1000)
    {
        DMX512_Reset();
    }
    
    /* Copy new data to active buffer if complete frame received */
    if (dmx_handle.data_received)
    {
        memcpy(dmx_handle.dmx_data, dmx_handle.new_dmx_data, DMX_CHANNELS);
        dmx_handle.data_received = 0;
    }
}

/**
  * @brief  Get DMX channel value
  * @param  channel: DMX channel number (1-512)
  * @retval Channel value (0-255)
  */
uint8_t DMX512_GetChannelValue(uint16_t channel)
{
    if (channel > 0 && channel <= DMX_CHANNELS)
    {
        return dmx_handle.dmx_data[channel - 1];
    }
    return 0;
}

/**
  * @brief  UART receive callback for DMX data
  * @param  data: Received byte
  * @retval None
  */
void DMX512_UART_RxCallback(uint8_t data)
{
    switch (dmx_handle.state)
    {
        case DMX_STATE_IDLE:
            /* Waiting for break signal */
            if (dmx_handle.break_detected)
            {
                dmx_handle.state = DMX_STATE_MAB;
                dmx_handle.break_detected = 0;
                dmx_handle.last_break_time = HAL_GetTick();
            }
            break;
            
        case DMX_STATE_MAB:
            /* Mark After Break - waiting for start code */
            if (data == DMX_START_CODE)
            {
                dmx_handle.state = DMX_STATE_DATA;
                dmx_handle.channel_count = 0;
                memset(dmx_handle.new_dmx_data, 0, DMX_CHANNELS);
            }
            else
            {
                /* Invalid start code, reset */
                DMX512_Reset();
            }
            break;
            
        case DMX_STATE_DATA:
            /* Receiving channel data */
            if (dmx_handle.channel_count < DMX_CHANNELS)
            {
                dmx_handle.new_dmx_data[dmx_handle.channel_count++] = data;
                
                /* Check if we've received all channels or enough for our needs */
                if (dmx_handle.channel_count >= 2)  // We only need channel 2
                {
                    dmx_handle.data_received = 1;
                }
            }
            else
            {
                /* Frame complete */
                dmx_handle.data_received = 1;
                DMX512_Reset();
            }
            break;
            
        default:
            DMX512_Reset();
            break;
    }
}

/**
  * @brief  UART error callback for break detection
  * @retval None
  */
void DMX512_UART_ErrorCallback(void)
{
    uint32_t error_flags = dmx_uart->ErrorCode;
    
    /* Check for framing error (break signal) */
    if (__HAL_UART_GET_FLAG(dmx_uart, UART_FLAG_FE))
    {
        __HAL_UART_CLEAR_FLAG(dmx_uart, UART_FLAG_FE);
        
        /* Read data register to clear RXNE flag */
        volatile uint8_t dummy = dmx_uart->Instance->DR;
        (void)dummy;
        
        dmx_handle.break_detected = 1;
        dmx_handle.state = DMX_STATE_IDLE;
    }
    
    /* Clear other error flags */
    if (__HAL_UART_GET_FLAG(dmx_uart, UART_FLAG_ORE))
    {
        __HAL_UART_CLEAR_FLAG(dmx_uart, UART_FLAG_ORE);
    }
    
    if (__HAL_UART_GET_FLAG(dmx_uart, UART_FLAG_NE))
    {
        __HAL_UART_CLEAR_FLAG(dmx_uart, UART_FLAG_NE);
    }
    
    if (__HAL_UART_GET_FLAG(dmx_uart, UART_FLAG_PE))
    {
        __HAL_UART_CLEAR_FLAG(dmx_uart, UART_FLAG_PE);
    }
    
    /* Clear error code */
    dmx_uart->ErrorCode = HAL_UART_ERROR_NONE;
}

/**
  * @brief  Check for DMX break signal
  * @retval None
  */
void DMX512_CheckBreak(void)
{
    /* This function can be called from a timer interrupt to check for break */
    if (__HAL_UART_GET_FLAG(dmx_uart, UART_FLAG_FE))
    {
        DMX512_UART_ErrorCallback();
    }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Reset DMX state machine
  * @retval None
  */
static void DMX512_Reset(void)
{
    dmx_handle.state = DMX_STATE_IDLE;
    dmx_handle.channel_count = 0;
    dmx_handle.break_detected = 0;
}