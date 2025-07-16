#include "main.h"
#include "dmx.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  DMX_Init(&huart1);

  while (1)
  {
    /* Map channel 2 level to PWM duty (ARR = 255) */
    __disable_irq();
    uint8_t level = DMX_GetChannelLevel(DMX_CHANNEL);
    __enable_irq();

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, level);

    HAL_Delay(5); // small delay – duty updated every ~5 ms
  }
}