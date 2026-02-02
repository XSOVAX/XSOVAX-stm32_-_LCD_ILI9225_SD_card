#include "menu.h"


menu_id_t current_menu = MENU_MAIN;
uint8_t selected_item = 0;
uint8_t prev_selected = 0;

// Подменю 1
menu_item_t sub1_items[] = {
    {"Пункт 1 ", action_sub1_item1},
    {"Пункт 2 ", action_sub1_item2},
    {"Пункт 3 ", action_sub1_item3},
    {"Пункт 4 ", action_sub1_item4}
};

// Подменю 2
menu_item_t sub2_items[] = {
    {"Пункт 1 ", action_sub2_item1},
    {"Пункт 2 ", action_sub2_item2},
    {"Пункт 3 ", action_sub2_item3},
    {"Пункт 4 ", action_sub2_item4}
};

// Подменю 3
menu_item_t sub3_items[] = {
    {"Пункт 1", action_sub3_item1},
    {"Пункт 2", action_sub3_item2},
    {"Пункт 3", action_sub3_item3},
    {"Пункт 4", action_sub3_item4}
};

// Главное меню
menu_item_t main_items[] = {
    {"Подменю 1 ", NULL},
    {"Подменю 2 ", NULL},
    {"Подменю 3 ", NULL}
};

// Все меню
menu_t menus[] = {
    [MENU_MAIN] = {"=== ГЛАВНОЕ МЕНЮ ===", main_items, sizeof(main_items)/sizeof(main_items[0]), MENU_MAIN},
    [MENU_SUB1] = {"--- Подменю 1 ---", sub1_items, sizeof(sub1_items)/sizeof(sub1_items[0]), MENU_MAIN},
    [MENU_SUB2] = {"--- Подменю 2 ---", sub2_items, sizeof(sub2_items)/sizeof(sub2_items[0]), MENU_MAIN},
    [MENU_SUB3] = {"--- Подменю 3 ---", sub3_items, sizeof(sub3_items)/sizeof(sub3_items[0]), MENU_MAIN}
};

void action_sub1_item1(void) { uart_puts("\r\nPodmenu 11>>> Вы в подменю 1, пункт 1\r\n"); }
void action_sub1_item2(void) { uart_puts("\r\nPodmenu 12>>> Вы в подменю 1, пункт 2\r\n"); }
void action_sub1_item3(void) { uart_puts("\r\nPodmenu 13>>> Вы в подменю 1, пункт 3\r\n"); }
void action_sub1_item4(void) { uart_puts("\r\nPodmenu 14>>> Вы в подменю 1, пункт 4\r\n"); }


void action_sub2_item1(void) { uart_puts("\r\n>>> Вы в подменю 2, пункт 1\r\n"); }
void action_sub2_item2(void) { uart_puts("\r\n>>> Вы в подменю 2, пункт 2\r\n"); }
void action_sub2_item3(void) { uart_puts("\r\n>>> Вы в подменю 2, пункт 3\r\n"); }
void action_sub2_item4(void) { uart_puts("\r\n>>> Вы в подменю 2, пункт 4\r\n"); }

void action_sub3_item1(void) { uart_puts("\r\n>>> Вы в подменю 3, пункт 1\r\n"); }
void action_sub3_item2(void) { uart_puts("\r\n>>> Вы в подменю 3, пункт 2\r\n"); }
void action_sub3_item3(void) { uart_puts("\r\n>>> Вы в подменю 3, пункт 3\r\n"); }
void action_sub3_item4(void) { uart_puts("\r\n>>> Вы в подменю 3, пункт 4\r\n"); }


volatile int i = 0;
void print_menu(void) {
    char buffer[100];
    const menu_t* menu = &menus[current_menu];

    uart_puts("\x1b[H\x1b[J");

    snprintf(buffer, sizeof(buffer), "печатаю меню %d раз\r\n\r\n", i++);
    uart_puts(buffer);

    snprintf(buffer, sizeof(buffer), "%s\r\n\r\n", menu->title);
    uart_puts(buffer);

    for (int i = 0; i < menu->count; i++) {
        if (i == selected_item) {
            snprintf(buffer, sizeof(buffer), "\x1b[7m> %s <\x1b[0m\r\n", menu->items[i].text);
        } else {
            snprintf(buffer, sizeof(buffer), "  %s  \r\n", menu->items[i].text);
        }
        uart_puts(buffer);
    }

    uart_puts("\r\n| ?  Назад \r\n| ?  Вверх \r\n| >< Выбрать \r\n| ?  Вниз  \r\n");
}


// Заливка прямоугольника цветом через низкоуровневый доступ к дисплею
// Заливка прямоугольника (корректно для инверсии Y)
static void menu_fill_rect(uint16_t x, uint16_t y_top, uint16_t width, uint16_t height, uint16_t color) {
    if (width == 0 || height == 0) return;
    
    // y_top — меньшая координата Y (физически "ниже" на экране при инверсии)
    // y_bottom — большая координата Y (физически "выше")
    uint16_t y_bottom = y_top + height - 1;
    
    ILI9225_setWindow(x, y_top, x + width - 1, y_bottom);
    ILI9225_writeIndex(GRAM_DATA_REG);
    
    uint32_t pixels = (uint32_t)width * height;
    for (uint32_t i = 0; i < pixels; i++) {
        SPI_send_16bit(SPI2, color);
    }
}

// Полная перерисовка меню
void menu_redraw_full(void) {
    const menu_t* menu = &menus[current_menu];
    
    // 1. Очистка области пунктов (от верха до низа)
    // Верхняя граница = MENU_START_Y - MENU_HEIGHT_16 + 1
    uint16_t area_top = MENU_START_Y - MENU_HEIGHT_16 + 1;
    menu_fill_rect(MENU_START_X, area_top, MENU_WIDTH, MENU_HEIGHT_16, COLOR_BLACK);
    
    // 2. Заголовок (сверху экрана)
    drawString8x16(MENU_START_X, MENU_HEADER_Y, menu->title, COLOR_WHITE, COLOR_BLACK);
    
    // 3. Пункты меню
    uint8_t visible_count = (menu->count < MAX_MENU_ITEMS) ? menu->count : MAX_MENU_ITEMS;
    for (uint8_t i = 0; i < visible_count; i++) {
        menu_redraw_item(i, (i == selected_item));
    }
    
    // 4. Подсказки (внизу экрана)
    drawString8x16(MENU_START_X, MENU_HINTS_Y,      "BACK  - Назад",    COLOR_GREEN, COLOR_BLACK);
    drawString8x16(MENU_START_X, MENU_HINTS_Y + 16, "UP    - Вверх",    COLOR_GREEN, COLOR_BLACK);
    drawString8x16(MENU_START_X, MENU_HINTS_Y + 32, "DOWN  - Вниз",     COLOR_GREEN, COLOR_BLACK);
    drawString8x16(MENU_START_X, MENU_HINTS_Y + 48, "ENTER - Выбрать",  COLOR_GREEN, COLOR_BLACK);
}

// Перерисовка ОДНОГО пункта
void menu_redraw_item(uint8_t index, uint8_t is_selected) {
    if (index >= menus[current_menu].count || index >= MAX_MENU_ITEMS) return;
    
    // Пункты рисуются СНИЗУ ВВЕРХ:
    // index=0 ? Y = MENU_START_Y (самый нижний видимый пункт)
    // index=1 ? Y = MENU_START_Y - 16 (выше)
    uint16_t y = MENU_START_Y - index * MENU_ITEM_HEIGHT_16;
    
    const char* text = menus[current_menu].items[index].text;
    char line[32];
    
    if (is_selected) {
        snprintf(line, sizeof(line), "> %s", text);
    } else {
        snprintf(line, sizeof(line), "  %s", text);
    }
    
    // Заливаем фон пункта
    menu_fill_rect(MENU_START_X, y, MENU_WIDTH, MENU_ITEM_HEIGHT_16,
                   is_selected ? COLOR_BLUE : COLOR_BLACK);
    
    // Рисуем текст
    drawString8x16(MENU_START_X, y, line, COLOR_WHITE,
                   is_selected ? COLOR_BLUE : COLOR_BLACK);
}

// Частичное обновление (только 2 пункта)
void menu_update_selection(void) {
    if (prev_selected != selected_item) {
       menu_redraw_item(prev_selected, 0);  // Снять выделение
       menu_redraw_item(selected_item, 1);  // Выделить новый
    }
}
