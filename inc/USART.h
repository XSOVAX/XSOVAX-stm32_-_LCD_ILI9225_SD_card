#ifndef USART_H

    #define USART_H

    #include <stdint.h>

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
    void uart_init(void);

    /**
     * @brief Передача одного символа
     * @param c Символ для передачи
     */
    void uart_putc(char c);

    /**
     * @brief Передача строки
     * @param s Указатель на строку (null-terminated)
     */
    void uart_puts(const char* s);

    /**
     * @brief Вывод байта в шестнадцатеричном формате
     * @param value Байт для вывода
     */
    void print_hex(uint8_t value);

    /**
     * @brief Вывод строки (дубль uart_puts, оставлено для совместимости)
     * @param str Указатель на строку
     */
    void print_string(const char* str);

#endif