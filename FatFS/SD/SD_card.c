/**
 * @file SD_card.c
 * @brief Драйвер microSD карты по SPI для STM32F103
 * 
 * Поддерживает:
 * - SDHC / SDXC (≥4 ГБ)
 * - Только SPI-режим
 * - Блочная адресация (sector = 512 байт)
 * 
 * Совместим с FatFS через функции:
 * - SD_ReadBlock()
 * - SD_WriteBlock()
 */

#include "SD_card.h"
#include "USART.h"  // Для отладки 
#include "stm32f1xx.h"
#include <stdint.h>
#include "TIMER.h"


// -----------------------------------------------------------------------------
// Внутренние функции
// -----------------------------------------------------------------------------

/**
 * @brief Ждёт R1-ответ от карты (не 0xFF)
 */
uint8_t sd_wait_for_r1(uint32_t timeout_ms) {
    volatile uint32_t timeout = timeout_ms * 1000; // ~1 мкс на итерацию при 72 МГц
    uint8_t response;
    SPI_devices[0].activate();
    do {
        response = SPI_transfer(SPI1, 0xFF);
        if (response != 0xFF) return response;
    } while (timeout--);
    SPI_devices[0].deactivate();
    return 0xFF;
}

/**
 * @brief Отправляет команду SD-карте
 */
static uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
    SPI_devices[0].deactivate();
    SPI_transfer(SPI1, 0xFF); // Пауза

    SPI_devices[0].activate();

    SPI_transfer(SPI1, 0x40 | cmd);
    SPI_transfer(SPI1, (uint8_t)(arg >> 24));
    SPI_transfer(SPI1, (uint8_t)(arg >> 16));
    SPI_transfer(SPI1, (uint8_t)(arg >> 8));
    SPI_transfer(SPI1, (uint8_t)arg);
    SPI_transfer(SPI1, crc);

    return sd_wait_for_r1(100); // 100 мс таймаут
}

/**
 * @brief Читает N байт из SPI
 */
static void sd_read_data(uint8_t *buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        buf[i] = SPI_transfer(SPI1, 0xFF);
    }
}

/**
 * @brief Чтение одного сектора (внутренняя реализация)
 */
static SD_Status sd_read_sector(uint32_t sector, uint8_t *buffer) {
    uint8_t r1 = sd_send_command(SD_CMD17_READ_SINGLE_BLOCK, sector, 0xFF);
    if (r1 != SD_R1_READY_STATE) {
        SPI_devices[0].deactivate();
        return SD_ERROR;
    }

    // Ждём токен данных 0xFE
    uint32_t timeout = 0xFFFF;
    uint8_t token;
    do {
        token = SPI_transfer(SPI1, 0xFF);
        if (token != 0xFF) break;
    } while (timeout--);

    if (token != SD_TOKEN_SINGLE_READ) {
        SPI_devices[0].deactivate();
        return SD_ERROR;
    }

    // Читаем 512 байт
    for (int i = 0; i < 512; i++) {
        buffer[i] = SPI_transfer(SPI1, 0xFF);
    }

    // Пропускаем CRC (2 байта)
    SPI_transfer(SPI1, 0xFF);
    SPI_transfer(SPI1, 0xFF);

    SPI_devices[0].deactivate();
    return SD_OK;
}

/**
 * @brief Запись одного сектора (внутренняя реализация)
 */
static SD_Status sd_write_sector(uint32_t sector, const uint8_t *buffer) {
    uint8_t r1 = sd_send_command(SD_CMD24_WRITE_SINGLE_BLOCK, sector, 0xFF);
    if (r1 != SD_R1_READY_STATE) {
        SPI_devices[0].deactivate();
        return SD_ERROR;
    }

    // Токен записи
    SPI_transfer(SPI1, SD_TOKEN_SINGLE_WRITE);

    // Данные
    for (int i = 0; i < 512; i++) {
        SPI_transfer(SPI1, buffer[i]);
    }

    // Фиктивный CRC
    SPI_transfer(SPI1, 0xFF);
    SPI_transfer(SPI1, 0xFF);

    // Проверка ответа
    uint8_t response = SPI_transfer(SPI1, 0xFF);
    if ((response & 0x1F) != SD_TOKEN_DATA_ACCEPTED) {
        SPI_devices[0].deactivate();
        return SD_ERROR;
    }

    // Ждём завершения записи
    volatile uint32_t timeout = 0xFFFF;
    while (SPI_transfer(SPI1, 0xFF) == 0x00) {
        if (timeout-- == 0) {
            SPI_devices[0].deactivate();
            return SD_TIMEOUT_ERROR;
        }
    }

    SPI_devices[0].deactivate();
    return SD_OK;
}

// -----------------------------------------------------------------------------
// Публичные функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация SD-карты
 */
SD_Status sd_init(void) {
    SPI_devices[0].deactivate();
    // Задержка после подачи питания
    for (volatile int i = 0; i < 100000; i++);
    // ≥80 тактов при CS=HIGH
    for (int i = 0; i < 10; i++) SPI_transfer(SPI1, 0xFF);
    Delay_ms(50);
    // CMD0
    uint8_t r1 = sd_send_command(SD_CMD0_GO_IDLE_STATE, 0, 0x95);
    if (r1 != SD_R1_IDLE_STATE) {uart_puts("\r\nSD init SD_CMD0_GO_IDLE_STATE error\r\n");return SD_ERROR;}

    // CMD8
    r1 = sd_send_command(SD_CMD8_SEND_IF_COND, 0x000001AA, 0x87);
    if (r1 != SD_R1_IDLE_STATE) {uart_puts("\r\nSD init SD_R1_IDLE_STATE error\r\n");return SD_ERROR;}

    uint8_t cmd8_resp[4];
    sd_read_data(cmd8_resp, 4);
    if (cmd8_resp[2] != 0x01 || cmd8_resp[3] != 0xAA) {uart_puts("\r\nSD init cmd8_resp error\r\n");return SD_ERROR;}

    // ACMD41 (выход из IDLE)
    uint32_t timeout = 1000;
    do {
        r1 = sd_send_command(SD_CMD55_APP_CMD, 0, 0xFF);
        if (r1 != SD_R1_IDLE_STATE)  {uart_puts("\r\nSD init SD_R1_IDLE_STATE error\r\n");return SD_ERROR;}

        r1 = sd_send_command(SD_CMD41_SD_SEND_OP_COND, 0x40000000, 0xFF);
        if (r1 == SD_R1_READY_STATE) break;

        for (volatile int i = 0; i < 20000; i++);
    } while (timeout--);

    if (r1 != SD_R1_READY_STATE) return SD_TIMEOUT_ERROR;

    SPI_devices[0].deactivate();
    return SD_OK;
}

/**
 * @brief Чтение блока — для FatFS (diskio.c)
 */
SD_Status SD_ReadBlock(uint32_t sector, uint8_t *buffer) {
    return sd_read_sector(sector, buffer);
}

/**
 * @brief Запись блока — для FatFS (diskio.c)
 */
SD_Status SD_WriteBlock(uint32_t sector, const uint8_t *buffer) {
    return sd_write_sector(sector, buffer);
}



void log_message(const char* msg) {
    FIL file;
    FRESULT res = f_open(&file, "sensor.log", FA_WRITE | FA_OPEN_APPEND | FA_OPEN_ALWAYS);
    if (res != FR_OK) return;

    f_write(&file, msg, strlen(msg), NULL);
    f_write(&file, "\r\n", 2, NULL);

    f_close(&file);
}