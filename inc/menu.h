#ifndef MENU_H

#define MENU_H

    #include "stm32f1xx.h"
    #include <string.h>
    #include "USART.h"
    #include <stdio.h>
    #include "ILI9225.h"
/*
    Физические координаты дисплея (Y инвертирована):
    ┌──────────────────────────────┐ Y = 319 (верх экрана)
    │ === ЗАГОЛОВОК МЕНЮ ===       │ ← MENU_HEADER_Y = 300
    ├──────────────────────────────┤
    │ > Пункт 1                    │ ← index=0, Y = MENU_ITEMS_Y_BOTTOM - 0*16 = 260
    │   Пункт 2                    │ ← index=1, Y = 260 - 16 = 244
    │   Пункт 3                    │ ← index=2, Y = 244 - 16 = 228
    │   ...                        │
    │   Пункт 8                    │ ← index=7, Y = 260 - 7*16 = 148
    ├──────────────────────────────┤ Y = 148 (низ области пунктов)
    │                              │
    │                              │
    ├──────────────────────────────┤
    │ BACK  - Назад                │ ← MENU_HINTS_Y = 10
    │ UP    - Вверх                │
    │ DOWN  - Вниз                 │
    │ ENTER - Выбрать              │
    └──────────────────────────────┘ Y = 0 (низ экрана)
*/

    // === ЧЁТКИЕ ГРАНИЦЫ ОБЛАСТЕЙ МЕНЮ ===
    // Y=0 — низ экрана, Y=ILI9225_maxY-1 — верх экрана

    // === ГРАНИЦЫ ОБЛАСТЕЙ МЕНЮ (с инверсией Y: Y=0 — низ экрана) ===

#define MENU_ITEM_HEIGHT_16 16
#define MENU_ITEM_WIDTH     8
#define MENU_WIDTH          160
#define MAX_MENU_ITEMS      4
#define MENU_HEIGHT_16      (MAX_MENU_ITEMS * MENU_ITEM_HEIGHT_16)  // = 64

// Минимум дополнений для работы:
#define MENU_HEADER_Y       (ILI9225_maxY - 20)  // Заголовок сверху экрана
#define MENU_HINTS_Y        10                   // Подсказки снизу экрана
#define MENU_START_X        10
#define MENU_START_Y        (MENU_HEADER_Y - 20) // НИЖНЯЯ граница области пунктов (рисуем снизу вверх)

typedef enum {
    MENU_MAIN,
    MENU_SUB1,
    MENU_SUB2,
    MENU_SUB3
} menu_id_t;

typedef struct {
    const char* text;
    void (*action)(void); // если не NULL — это конечный пункт
} menu_item_t;

typedef struct {
    const char* title;
    const menu_item_t* items;
    uint8_t count;
    menu_id_t parent; // куда возвращаться по "назад"
} menu_t;

extern menu_id_t current_menu;
extern uint8_t selected_item;
extern uint8_t prev_selected;
extern menu_t menus[];


void action_sub1_item1(void);
void action_sub1_item2(void);
void action_sub1_item3(void);
void action_sub1_item4(void);


void action_sub2_item1(void);
void action_sub2_item2(void);
void action_sub2_item3(void);
void action_sub2_item4(void);

void action_sub3_item1(void);
void action_sub3_item2(void);
void action_sub3_item3(void);
void action_sub3_item4(void);


void print_menu(void);

void draw_menu_on_display(void);


void menu_update_selection(void);
void menu_redraw_item(uint8_t index, uint8_t is_selected);
void menu_redraw_full(void);
void menu_debug_draw_borders(void);
#endif /* MENU_H */