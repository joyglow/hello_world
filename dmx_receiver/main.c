#include <stdint.h>
#include "stm32f103xb.h"

// Compile-time constants
#define DMX_CHANNEL             2u      // We listen to slot 2
#define DMX_BREAK_MIN_US        88u

static volatile uint16_t dmx_slot = 0;
static volatile uint8_t  dmx_value = 0;

static void rcc_init(void)
{
    // Enable GPIOA, TIM2, USART1 clocks (APB2/APB1)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

static void gpio_init(void)
{
    // PA0 -> TIM2_CH1 (Alternate push-pull, 2 MHz)
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |=  (0x2u << GPIO_CRL_MODE0_Pos);    // Output 2 MHz
    GPIOA->CRL |=  (0x2u << GPIO_CRL_CNF0_Pos);     // AF output push-pull

    // PA10 -> USART1_RX (floating input)
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |=  (0x1u << GPIO_CRH_CNF10_Pos);    // Floating input
}

static void tim2_pwm_init(void)
{
    // PSC and ARR chosen for 72 MHz / (PSC+1) / (ARR+1) = ~1 kHz
    TIM2->PSC  = 71u;     // 72 MHz / 72 = 1 MHz
    TIM2->ARR  = 255u;    // 1 MHz / 256 ≈ 3.9 kHz (good enough)

    TIM2->CCR1 = 0u;      // Start with LED off

    TIM2->CCMR1 = (6u << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE; // PWM mode 1 + preload
    TIM2->CCER  = TIM_CCER_CC1E;               // Enable output

    TIM2->CR1   = TIM_CR1_ARPE | TIM_CR1_CEN;  // Auto-reload preload, enable counter
}

static void usart1_dmx_init(void)
{
    // 250 kBd, 8 N 2
    // USART1 clock: 72 MHz -> BRR = 72e6 / 250000 = 288 -> 0x0120 (mantissa 18, fraction 0)
    USART1->BRR = 288u;
    USART1->CR1 = USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

    // 2 stop bits by default when M=0 & STOP=10: already 1 stop bit default; set STOP=10
    USART1->CR2 |= (0x2u << USART_CR2_STOP_Pos);

    // Enable NVIC interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
    // BREAK = framing error (FE) + RXNE set
    if (USART1->SR & USART_SR_FE)
    {
        volatile uint8_t dummy = USART1->DR; // Clear FE by reading SR then DR
        (void)dummy;
        dmx_slot = 0;
        return;
    }

    if (USART1->SR & USART_SR_RXNE)
    {
        uint8_t byte = USART1->DR;
        dmx_slot++;
        if (dmx_slot == 1)
        {
            // Start code (should be 0) – ignore
        }
        else if (dmx_slot == DMX_CHANNEL + 1)
        {
            dmx_value = byte;
            TIM2->CCR1 = dmx_value; // 1:1 mapping (ARR = 255)
        }
        if (dmx_slot >= 513)
            dmx_slot = 0;
    }
}

int main(void)
{
    rcc_init();
    gpio_init();
    tim2_pwm_init();
    usart1_dmx_init();

    while (1)
    {
        // nothing – work done in ISR
    }
}