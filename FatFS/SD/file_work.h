#ifndef FILE_WORK_H
#define FILE_WORK_H

#include <stdint.h>
#include "ff.h"
#include "diskio.h"
#include "SD_card.h"
#include "SPI.h"
#include "ILI9225.h"


// -----------------------------------------------------------------------------
// Конфигурация
// -----------------------------------------------------------------------------

#define MAX_FILES     10
#define FILENAME_LEN  23  // 8.3 формат: "FILENAME.TXT" = 12 + 1

// -----------------------------------------------------------------------------
// Глобальные переменные
// -----------------------------------------------------------------------------

extern char file_list[MAX_FILES][FILENAME_LEN];
extern uint8_t file_count;
extern FATFS fs;

// -----------------------------------------------------------------------------
// Публичные функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация файловой системы
 * 
 * Монтирует FatFS, форматирует карту при необходимости
 * @return FR_OK при успехе, код ошибки при неудаче
 */
FRESULT filesystem_init(void);

/**
 * @brief Тест записи файла
 */
void test_file_write(void);

/**
 * @brief Тест чтения файла
 */
void test_file_read(void);

/**
 * @brief Выводит список файлов в корневом каталоге
 * @param suffix Фильтр по расширению (например, ".TXT") или ".*" для всех
 */
void list_files(const char *suffix);

/**
 * @brief Сканирует файлы с заданным расширением
 * @param suffix Фильтр по расширению (например, ".TXT")
 */
void scan_files(const char *suffix);

/**
 * @brief Выводит список найденных файлов с индексами
 */
void print_file_list(void);

/**
 * @brief Чтение из файла картинки и вывод ее на экран LCD
 * @param suffix название файла для открытия
 * @param buffer буфер для хранения части открытого файла 
 * для последующей передаче его на экран
 */
void file_read(const char *suffix, uint8_t *buffer, uint16_t len_b);

#endif