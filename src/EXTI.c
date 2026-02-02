#include "EXTI.h"

#define NUCLEO 1
#define NIKITA 2

#define BOARD NIKITA


void EXTI_init( void ) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

#if(BOARD == NIKITA)
    GPIOB->CRL &= 0xFFFFFF00;
    GPIOB->CRH &= 0xFFFF00FF;
    GPIOB->CRL |= 0x00000088;
    GPIOB->CRH |= 0x00008800;
    GPIOB->ODR |= GPIO_ODR_ODR0 | GPIO_ODR_ODR1 | GPIO_ODR_ODR10 | GPIO_ODR_ODR11;

    AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0   | AFIO_EXTICR1_EXTI1); // сначала очистить
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB  | AFIO_EXTICR1_EXTI1_PB;
    AFIO->EXTICR[2] &= ~(AFIO_EXTICR3_EXTI10  | AFIO_EXTICR3_EXTI11);
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PB | AFIO_EXTICR3_EXTI11_PB;

    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11); // по спаду    
    EXTI->PR    = (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11); // сбросить флаги
    EXTI->IMR  |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11); // разрешить прерывания
    EXTI->PR    = (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11); // сбросить флаги

    NVIC_SetPriority(EXTI0_IRQn, 2);
    NVIC_SetPriority(EXTI1_IRQn, 2);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
#else
    GPIOB->CRL &= 0xF000FFF0;
    GPIOB->CRL |= 0x08880008;
    GPIOB->ODR |= GPIO_ODR_ODR0 | GPIO_ODR_ODR4 | GPIO_ODR_ODR5 | GPIO_ODR_ODR6;
    // EXTI3 → PB3 → EXTICR1 → [0]
    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0; // сначала очистить
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB;

    // EXTI4,5,6 → PB4,5,6 → EXTICR2 → [1]
    AFIO->EXTICR[1] &= ~(AFIO_EXTICR2_EXTI4 | AFIO_EXTICR2_EXTI5 | AFIO_EXTICR2_EXTI6);
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PB |
                       AFIO_EXTICR2_EXTI5_PB |
                       AFIO_EXTICR2_EXTI6_PB;
    EXTI->FTSR |= (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6); // по спаду    
    EXTI->PR    = (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6); // сбросить флаги
    EXTI->IMR  |= (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6); // разрешить прерывания
    EXTI->PR    = (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6); // сбросить флаги

    NVIC_SetPriority(EXTI0_IRQn, 2);
    NVIC_SetPriority(EXTI4_IRQn, 2);
    NVIC_SetPriority(EXTI9_5_IRQn, 2);
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif
}



void EXTI0_IRQHandler( void ) { 
    static uint32_t last_press = 0;
    uint32_t now = get_ms();

    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        if (now - last_press > 200) {
            last_press = now;
            menu_back();
        }
    }
}


#if(BOARD == NIKITA)
void EXTI1_IRQHandler( void ) { 
    static uint32_t last_press = 0;
    uint32_t now = get_ms();
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR = EXTI_PR_PR1;
        if (now - last_press > 200) {
            last_press = now;
            // Обработчик для PB0 (BACK)
            menu_up();
        }
    }
}
#else
void EXTI4_IRQHandler( void ) {
    static uint32_t last_press = 0;
    uint32_t now = get_ms();
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR = EXTI_PR_PR4;
        if (now - last_press > 300) {
            last_press = now;
            // Обработчик для PB4 (UP)
            menu_up();
        }
    }
}
#endif


#if(BOARD == NIKITA)
void EXTI15_10_IRQHandler( void ) {
    uart_puts("EXIT 10_15\r\n");
    static uint32_t last_press = 0;
    uint32_t now = get_ms();
    // Обработка PB10 — Down
    if (EXTI->PR & EXTI_PR_PR10) {
        EXTI->PR = EXTI_PR_PR10;
        if (now - last_press > 300) {
            last_press = now;
            menu_down(); 
        }
    }
    // Обработка PB11 — SET
    if (EXTI->PR & EXTI_PR_PR11) {
        EXTI->PR = EXTI_PR_PR11;
        if (now - last_press > 300) {
            last_press = now;
            menu_set();
        }
    }
}
#else
void EXTI9_5_IRQHandler( void ) {
    static uint32_t last_press = 0;
    uint32_t now = get_ms();
    // Обработка PB5 — SET
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR = EXTI_PR_PR5;
        if (now - last_press > 300) {
            last_press = now;
            menu_set(); 
        }
    }
    // Обработка PB6 — DOWN
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR = EXTI_PR_PR6;
        if (now - last_press > 300) {
            last_press = now;
            menu_down();
        }
    }
}
#endif

void menu_up(void) {
    uart_puts("UP\r\n");
    if (selected_item > 0) {
        prev_selected = selected_item;
        selected_item--;
        menu_update_selection(); // Только 2 пункта
    }
    else {
        const menu_t* menu = &menus[current_menu];
        prev_selected = selected_item;
        selected_item = menu->count - 1;
        menu_update_selection(); // Только 2 пункта
    }
}

void menu_down(void) {
    uart_puts("DOWN\r\n");
    const menu_t* menu = &menus[current_menu];
    if (selected_item < menu->count - 1) {
        prev_selected = selected_item;
        selected_item++;
        menu_update_selection(); // Только 2 пункта
    }
    else {
        prev_selected = selected_item;
        selected_item = 0;
        menu_update_selection(); // Только 2 пункта
    }
}

void menu_set(void) {
    uart_puts("SET\r\n");
    const menu_t* menu = &menus[current_menu];
    
    if (menu->items[selected_item].action != NULL) {
        menu->items[selected_item].action();
        // После действия — обновляем текущий пункт
        menu_redraw_item(selected_item, 1);
    } else {
        // Переход в подменю → полная перерисовка
        if (current_menu == MENU_MAIN) {
            current_menu = (selected_item == 0) ? MENU_SUB1 :
                          (selected_item == 1) ? MENU_SUB2 : MENU_SUB3;
            selected_item = 0;
            prev_selected = 0;
            menu_redraw_full(); // Полная перерисовка
        }
    }
}

void menu_back(void) {
    uart_puts("BACK\r\n");
    if (current_menu != MENU_MAIN) {
        current_menu = menus[current_menu].parent;
        selected_item = 0;
        prev_selected = 0;
        menu_redraw_full(); // Полная перерисовка
    }
}

/*
void menu_up( void ) {
    uart_puts("UP\r\n");
    if (selected_item > 0) {
        prev_selected = selected_item;
        selected_item--;
        draw_menu_on_display();
        //update_menu_items(); 
    }
}

void menu_down( void ) {
    uart_puts("DOWN\r\n");
    const menu_t* menu = &menus[current_menu];
    if (selected_item < menu->count - 1) {
        prev_selected = selected_item;
        selected_item++;
        draw_menu_on_display();
        //update_menu_items(); 
    }
}

void menu_set( void ) {
    uart_puts("SET\r\n");
    const menu_t* menu = &menus[current_menu];
    if (menu->items[selected_item].action != NULL) {
        menu->items[selected_item].action(); // вызов действия
        // После действия — обновляем текущее меню
        draw_menu_on_display();
    } else {
        // Переход в подменю
        if (current_menu == MENU_MAIN) {
            if (selected_item == 0) current_menu = MENU_SUB1;
            else if (selected_item == 1) current_menu = MENU_SUB2;
            else if (selected_item == 2) current_menu = MENU_SUB3;
            selected_item = 0;
            draw_menu_on_display(); // полная перерисовка
        }
    }
}

void menu_back( void ) {
    uart_puts("BACK\r\n");
    if (current_menu != MENU_MAIN) {
        current_menu = menus[current_menu].parent;
        selected_item = 0;
        draw_menu_on_display();
    }
}

*/