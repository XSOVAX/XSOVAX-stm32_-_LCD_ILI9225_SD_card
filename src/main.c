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
    while(1){}
}