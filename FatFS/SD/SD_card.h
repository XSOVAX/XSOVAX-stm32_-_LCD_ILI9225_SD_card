#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdint.h>
#include <ff.h>
#include "SPI.h"

// -----------------------------------------------------------------------------
// Внутренние константы
// -----------------------------------------------------------------------------
#define SD_CMD0_GO_IDLE_STATE       (0)
#define SD_CMD8_SEND_IF_COND        (8)
#define SD_CMD17_READ_SINGLE_BLOCK  (17)
#define SD_CMD24_WRITE_SINGLE_BLOCK (24)
#define SD_CMD55_APP_CMD            (55)
#define SD_CMD58_READ_OCR           (58)
#define SD_CMD41_SD_SEND_OP_COND    (41)

#define SD_TOKEN_SINGLE_READ        (0xFE)
#define SD_TOKEN_SINGLE_WRITE       (0xFE)
#define SD_TOKEN_DATA_ACCEPTED      (0x05)

#define SD_R1_IDLE_STATE            (0x01)
#define SD_R1_READY_STATE           (0x00)


typedef enum {
    SD_OK = 0,
    SD_ERROR,
    SD_TIMEOUT_ERROR
} SD_Status;

/**
 * @brief Ждёт R1-ответ от карты (не 0xFF)
 */
uint8_t sd_wait_for_r1(uint32_t timeout_ms) ;
// === Обязательные для FatFS функции ===
SD_Status SD_ReadBlock(uint32_t sector, uint8_t *buffer);
SD_Status SD_WriteBlock(uint32_t sector, const uint8_t *buffer);

// === Вспомогательные функции ===
SD_Status sd_init(void);
void sd_spi_set_high_speed(void);
void log_message(const char* msg);

#endif