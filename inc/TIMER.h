#ifndef _TIMER

	#define _TIMER
	#include "stm32f1xx.h"
	
	void TIM3_init(void);
	int Delay_ms(int time_ms);
	int Delay_us(int time_us);

#endif

