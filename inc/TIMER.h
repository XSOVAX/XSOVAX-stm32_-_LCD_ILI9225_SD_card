#ifndef _TIMER

	#define _TIMER
	#include "stm32f1xx.h"
	
	void TIM3_init( void );
	int Delay_ms( int time_ms );
	void SysTick_init( void );
	uint32_t get_ms(void);
	void TIM1_init( void );

#endif

