#include "interrupts.h"

// Note:
// This file provides weak references to ARM interrupts and allows to easily
// catch undefined interrupts

// USART
void __attribute__((weak)) USART1_IRQHandler(void) {while(1){}}

// RTC
void __attribute__((weak)) RTC_IRQHandler(void) {while(1){}}

// EXTI
void __attribute__((weak)) EXTI1_IRQHandler(void) {while(1){}}
void __attribute__((weak)) EXTI15_10_IRQHandler(void) {while(1){}}

// TIM
void __attribute__((weak)) TIM2_IRQHandler(void) {while(1){}}

// SysTick
void __attribute__((weak)) SysTick_Handler(void) {while(1){}}

