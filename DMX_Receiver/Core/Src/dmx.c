#include "dmx.h"

static UART_HandleTypeDef *huart_dmx;
static uint8_t rx_byte;
static volatile uint16_t dmx_slot = 0;      // 0 = waiting for BREAK, 1 = start code
static volatile uint8_t dmx_buffer[DMX_UNIVERSE_SIZE];

static void start_rx(void)
{
    HAL_UART_Receive_IT(huart_dmx, &rx_byte, 1);
}

void DMX_Init(UART_HandleTypeDef *huart)
{
    huart_dmx = huart;
    start_rx();
}

uint8_t DMX_GetChannelLevel(uint16_t ch)
{
    if (ch < DMX_UNIVERSE_SIZE)
        return dmx_buffer[ch];
    return 0;
}

/* HAL weak callbacks override ------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != huart_dmx) return;

    dmx_slot++;

    if (dmx_slot == 1)
    {
        // Start code, ignore content
    }
    else if (dmx_slot < DMX_UNIVERSE_SIZE)
    {
        dmx_buffer[dmx_slot] = rx_byte;
    }

    if (dmx_slot >= DMX_UNIVERSE_SIZE)
        dmx_slot = 0; // wait for next BREAK

    start_rx();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart != huart_dmx) return;

    if (huart->ErrorCode & HAL_UART_ERROR_FE)
    {
        // Framing error -> BREAK received
        dmx_slot = 0;
    }
    // restart reception
    start_rx();
}