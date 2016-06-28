#include "manchester.h"

GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

void manchester_protocol_init(uint16_t frequency) {
    // Initialize manchester protocol device with provided clock speed

    // Enable peripheral clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    // Protocol is using DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Setup A10 as output pin
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Use TIM1 / Channel 5 @ DMA1

    DMA_InitStructure.DMA_PeripheralBaseAddr = TIM1_CCR3_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // Enable Transmission Complete interrupt
    // DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Compute the value to be set in ARR register to generate signal frequency at 40 Khz
    double TimerPeriod = (SystemCoreClock / (frequency * 2.0));

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);


    // Channel 3 Configuration in PWM mode
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    zero =  0;
    one = TimerPeriod + 1;

    // DMA1 Channel5 enable
    DMA_Cmd(DMA1_Channel5, ENABLE);


    // TIM1 Update DMA Request enable
    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

    // TIM1 counter enable
    TIM_Cmd(TIM1, ENABLE);


    // Main Output Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

// function for sending data using Manchester Code
void manchester_send(char* string) {

    uint8_t length = strlen(string);

    // each letter from string will be encoded using 8 bits
    // to encode 1 bit we need 2 elements in array
    // additionaly an extra bit is required to "pull down" DMA line

    uint16_t buffer_size = length * 8 * 2 + 1;

    // remove old array and allocate memory for new one
    free(buffer);
    buffer = calloc(buffer_size, sizeof(uint16_t));

    // fill array with data
    char c;
    for (uint8_t i = 0; i < length; ++i) {
        c = string[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if ((c >> j) & 1) {
                // 1 -> 10
                buffer[i * 16 + j * 2] = one;
                buffer[i * 16 + j * 2 + 1] = zero;
            } else {
                // 0 -> 01
                buffer[i * 16 + j * 2] = zero;
                buffer[i * 16 + j * 2 + 1] = one;
            }
        }
    }

    // set last bit to zero
    buffer[buffer_size - 1] = zero;

    DMA_Cmd(DMA1_Channel5, DISABLE);

    // assign new value to DMA buffer, set correct size, restart timer
    DMA1_Channel5->CMAR = buffer;
    DMA1_Channel5->CNDTR = buffer_size;
    TIM1->CNT = 0;

    DMA_Cmd(DMA1_Channel5, ENABLE);
}

//void DMA1_Channel5_IRQHandler(void)
//{
//    // Transfer completed
//    if(DMA_GetITStatus(DMA1_IT_TC5)) {
//        LED_toggle(1);
//        DMA_ClearFlag(DMA1_FLAG_TC5);
//        DMA_ClearITPendingBit(DMA1_IT_TC5);
////        busy = false;
//    }
//}
