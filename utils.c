#include "utils.h"

static __IO uint32_t _delay;

void setup_delay_timer(TIM_TypeDef *timer) {
    TIM_DeInit(timer);
    // Enable Timer clock
    if (timer == TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    } else if (timer == TIM3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    } else if (timer == TIM4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    } else {
        // TODO: not implemented
        while(1){}
    }

    // Configure timer
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us (10 ms)
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(timer, &TIM_InitStructure);

    TIM_Cmd(timer, ENABLE);
}

void delay(__IO uint32_t nTime)
{
  _delay = nTime;

  while(_delay != 0);
}

void delay_decrement(void)
{
  if (_delay--) {}
}

void delay_us(TIM_TypeDef *timer, unsigned int time) {
    timer->CNT = 0;
    time -= 3;
    while (timer->CNT <= time) {}
}

void delay_ms(TIM_TypeDef *timer, unsigned int time) {
    while (time--) {
        delay_us(timer, 1000);
    }
}

void LED_Init1(void) {
    //Smaller board

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Use PC8 and PC9 // Discovery LEDs
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LED_Init2(void) {
    //Bigger board

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Use PC8 and PC9 // Discovery LEDs
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
}

void LED_Init3(void) {
    //Discovery board

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Use PC8 and PC9 // Discovery LEDs
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// Initialize Discovery User Button
void BTN_Init(void) {
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Use PC8 and PC9 // Discovery LEDs
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

