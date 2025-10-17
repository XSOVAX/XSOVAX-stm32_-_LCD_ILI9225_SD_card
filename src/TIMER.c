#include "TIMER.h"


void TIM3_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 
	TIM3->CR1 |= TIM_CR1_OPM; // One Pulse Mode
	TIM3->CNT = 0;
	TIM3->PSC = 32000 - 1;
  	TIM3->ARR = 0;
	TIM3->EGR |= TIM_EGR_UG; // Update Generation 
}


int Delay_ms(int time_ms) {
	TIM3->ARR = (uint16_t)(time_ms) * 2;
	TIM3->CR1 |= TIM_CR1_CEN;
  	while (TIM3->CR1 & TIM_CR1_CEN) {};
	return time_ms; 
}


int Delay_us(int time_us) {
	SysTick->VAL   = 0;
	SysTick->LOAD  = (uint32_t)(time_us*1.75f);
	SysTick->CTRL |= 0x05;
	while(!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL &= ~((unsigned int)0x01);
	return time_us;
}