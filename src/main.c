/**
 * @file main.c
 * @brief Основной файл проекта STM32F103 + microSDHC + FatFS
 * 
 * Функции:
 * - Инициализация системы (RCC, UART, SPI)
 * - Работа с microSD-картой через FatFS
 * - Запись и чтение файлов
 * - Сканирование файлов по расширению
 * - Вывод информации через UART
 */

#include "stm32f1xx.h"
#include <stdio.h>
#include "USART.h"
#include "TIMER.h"
#include "SPI.h"
#include "file_work.h"
#include "SD_card.h"
#include "ILI9225.h"

#define BUF_READ_PICTURE (15 /* строчек */ * LCD_HEIGHT * 3 /* байта на цвет */)
static uint8_t picture[BUF_READ_PICTURE];

/**
 * @brief Инициализация тактирования системы
 * 
 * Настройки:
 * - Тактирование от HSE (8 МГц)
 * - PLL: 8 МГц * 9 = 72 МГц
 * - APB1 = 36 МГц (для периферии)
 * - APB2 = 72 МГц (для периферии)
 * - Flash latency = 2 (для 72 МГц)
 */
static void system_rcc_init(void) {
    // Включаем HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}

    // Настройка Flash latency (2 WS для 72 МГц)
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_2) {}

    // Настройка предделителя APB1 (деление на 2 → 36 МГц)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // Настройка PLL: 8 МГц * 9 = 72 МГц
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

    // Включаем PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    // Переключаемся на PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}


/**
 * @brief Основная функция программы
 */
int main(void) {
    system_rcc_init();

    // Инициализурем USART1 для логов
    uart_init();
    uart_puts("Проект инициализирован!\r\n");
    Delay_ms(500);
    uart_puts("Прождали 500 мсек!\r\n");
    Delay_us(500);
    uart_puts("Прождали 500 нсек!\r\n");
    spi_init();
    uart_puts("Теперь есть SPI1!\r\n");
    uart_puts("Он нужен для общения с SD картой и LCD дисплеем\r\n");
    uart_puts("Пока у нас мульти слейв режим потом разнесем для использования DMA \r\n");

    sd_init();

    uart_puts("\r\n=== FatFS Test ===\r\n");
    // Инициализация файловой системы
    FRESULT res = filesystem_init();
    while (res != FR_OK) {
        uart_puts("Filesystem init failed: ");
        print_hex(res);
        uart_puts("\r\n");
        SPI_devices[0].deactivate();
        Delay_ms(1000);
        res = filesystem_init();
    }
    

    uart_puts("Filesystem mounted!\r\n");

    // Вывод всех файлов 
    list_files(".*");

    // Инициализация дисплея
    ILI9225_init();
	// Очистка дисплея (заливка черным цветом)
    ILI9225_clear();
    // Установка ориентации (011)
    ILI9225_write(ENTRY_MODE, (0x1000) | (0b011 << 3));

    file_read("xp_.bmp", picture, BUF_READ_PICTURE);

    drawString8x8(10, 10, "HELLO WORLD", COLOR_TOMATO, 0);
    drawString8x8(10, 10 + 9, "HELLO WORLD", COLOR_TOMATO, 0);
    drawString8x8(10, 10 + 9 * 2, "HELLO WORLD", COLOR_TOMATO, 0);
    drawString8x8(10, 40, "HELLO WORLD", COLOR_TOMATO, 1);
    drawString8x8(10, 40 + 9, "HELLO WORLD", COLOR_TOMATO, 1);
    drawString8x8(10, 40 + 9 * 2, "HELLO WORLD", COLOR_TOMATO, 1);
    drawString8x8(10, 150, "HELLO WORLD", COLOR_TOMATO, 0);
    drawString8x8(10, 150 + 9, "HELLO WORLD", COLOR_TOMATO, 0);
    drawString8x8(10, 150 + 9 * 2, "HELLO WORLD", COLOR_TOMATO, 1);
    drawString8x8(10, 150 + 9 * 3, "HELLO WORLD", COLOR_TOMATO, 0);
    while(1){}
}