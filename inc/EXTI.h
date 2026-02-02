#ifndef EXTI_H

#define EXTI_H

    #include "stm32f1xx.h"
    #include "menu.h"
    #include "TIMER.h"


void EXTI_init( void );
void EXTI0_IRQHandler( void );
void EXTI4_IRQHandler( void );
void EXTI9_5_IRQHandler( void );

void menu_up( void );
void menu_down( void );
void menu_set( void );
void menu_back( void );
#endif