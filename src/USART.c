/**
 * @file USART.c
 * @brief Драйвер UART для STM32F103
 * 
 * Используется для отладочного вывода через USART2.
 * 
 * Пины:
 * - PA2 (TX) — передача данных
 * - PA3 (RX) — приём данных (не используется в этом проекте)
 * 
 * Настройки:
 * - Скорость: 115200 бод
 * - Формат: 8N1 (8 бит, без проверки чётности, 1 стоп-бит)
 * - Тактирование: APB1 = 36 МГц
 */

#include "USART.h"
#include "stm32f1xx.h"
#include <stdint.h>

// -----------------------------------------------------------------------------
// Внутренние функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация USART2
 * 
 * Настройка пинов:
 * - PA2: TX (Alternate Function Push-Pull, 50MHz)
 * - PA3: RX (Input Floating)
 */
static void usart2_init(void) {
    // Тактирование порта A и альтернативных функций
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    // Тактирование USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // PA2 = USART2_TX (Alternate Function Push-Pull, 50MHz)
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0 | GPIO_CRL_CNF2_1;

    // PA3 = USART2_RX (Input Floating)
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_CNF3_1;
    // Установка пина в 1 (для входа)
    GPIOA->BSRR = GPIO_BSRR_BS3;

    // Настройка скорости: 36 МГц / 115200 = 312.5 → BRR = 0x138 (0x1380 для OVER8=0)
    USART2->BRR = 0x138; // 36000000 / 16 / 115200 = 19.53 ≈ 0x138 (19.5 * 16 = 312)
    // Включение USART, передатчика и приёмника
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

// -----------------------------------------------------------------------------
// Публичные функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация UART
 * 
 * Инициализирует USART2 с настройками:
 * - Скорость: 115200 бод
 * - Формат: 8N1
 * - Пины: PA2 (TX), PA3 (RX)
 */
void uart_init(void) {
    usart2_init();
}

/**
 * @brief Передача одного символа
 * @param c Символ для передачи
 */
void uart_putc(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

/**
 * @brief Передача строки
 * @param s Указатель на строку (null-terminated)
 */
void uart_puts(const char* s) {
    while (*s) {
        uart_putc(*s++);
    }
    // Ждём завершения передачи
    while (!(USART2->SR & USART_SR_TC));
}

/**
 * @brief Вывод байта в шестнадцатеричном формате
 * @param value Байт для вывода
 */
void print_hex(uint8_t value) {
    const char hex_chars[] = "0123456789ABCDEF";

    // Старший ниббл
    uart_putc(hex_chars[(value >> 4) & 0x0F]);
    // Младший ниббл
    uart_putc(hex_chars[value & 0x0F]);
}

/**
 * @brief Вывод строки (дубль uart_puts, оставлено для совместимости)
 * @param str Указатель на строку
 */
void print_string(const char* str) {
    while (*str) {
        uart_putc(*str++);
    }
}