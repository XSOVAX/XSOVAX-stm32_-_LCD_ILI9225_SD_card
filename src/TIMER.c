#include "TIMER.h"

volatile uint32_t systick_ms = 0;

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




// Обработчик SysTick (уже должен быть, но убедимся)
void SysTick_Handler(void) {
    systick_ms++;
}

// Настройка SysTick на 1 мс (при 72 МГц)
void SysTick_init(void ) {
    SysTick->LOAD = (72000000 / 8 / 1000) - 1; // 72 MHz / 8 = 9 MHz → 9000 тиков = 1 мс
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk  |
                    SysTick_CTRL_ENABLE_Msk;
}

uint32_t get_ms(void) {
    return systick_ms;
}

//--------------------------------------/
// TIM1 configuration:					//
// PA8 	- PWM output for channel 1		//
//--------------------------------------/

void TIM1_init( void ) {
	//configure pins ALT funct out PP 50MHz gpioTIM4_CH4
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRH &= ~GPIO_CRH_CNF8;
	GPIOA->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1; // 1 chanel
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_0;

	RCC->APB2ENR|= RCC_APB2ENR_TIM1EN;
	TIM1->CCMR1 |=  0x0060; //CC1 - PWM1
	TIM1->CCER  &=	0x0;
	TIM1->CCER  |=  0x09;//Enable output
	TIM1->CNT    =  0; //start with 0
	TIM1->PSC    =  12 - 1; //prescaler 1
	TIM1->ARR    =  1600 - 1; //ARR is set to 9600 Hz
	TIM1->CCR1   =  800 - 1; //starting duty cycle 0.5

	TIM1->EGR   |=  TIM_EGR_UG; //update shadow registers
	TIM1->SR    &= ~TIM_SR_UIF; //reset upd flag
	TIM1->BDTR  |=  TIM_BDTR_MOE;
	TIM1->CR1   |=  TIM_CR1_CEN; //start timer
}