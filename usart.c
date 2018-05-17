#include "usart.h"

void usart1_init(unsigned int speed)
{
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // Use PA9 and PA10
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure; // Configure the NVIC (nested vector interrupt controller)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);

    // USART1
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_Init(USART1, &USART_InitStructure);

    // Enable interrupts
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Enable usart
    USART_Cmd(USART1, ENABLE);

}

void usart2_init(unsigned int speed)
{
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // Use PA9 and PA10
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure; // Configure the NVIC (nested vector interrupt controller)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);

    // USART2
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_Init(USART2, &USART_InitStructure);

    // Enable interrupts
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // Enable usart
    USART_Cmd(USART2, ENABLE);

}

void usart_print(USART_TypeDef* usart, char* string)
{
    u8 a = 0;
    while (string[a]) {
        USART_SendData(usart, string[a++]);
        while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
    }
}

void usart_printf(USART_TypeDef *usart, const char *format, ...)
{
    va_list args;
    static char buffer[120];
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    usart_print(usart, buffer);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* msg, uint8_t* file, uint32_t line) {
      usart_printf(USART1, "Assert: %s failed: %s:%d\r\n", msg, file, (int)line);
      hacf();
}
#endif
