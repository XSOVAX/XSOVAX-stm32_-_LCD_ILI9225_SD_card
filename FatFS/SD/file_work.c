/**
 * @file file_work.c
 * @brief Работа с файлами на SD-карте через FatFS
 * 
 * Функции для:
 * - Сканирования файлов по расширению
 * - Вывода списка файлов
 * - Поиска файлов с заданным расширением
 */

#include "file_work.h"
#include "ff.h"
#include "USART.h"
#include <string.h>

// -----------------------------------------------------------------------------
// Глобальные переменные
// -----------------------------------------------------------------------------

char file_list[MAX_FILES][FILENAME_LEN];
uint8_t file_count = 0;
FATFS fs;

// -----------------------------------------------------------------------------
// Внутренние функции
// -----------------------------------------------------------------------------

/**
 * @brief Проверяет, заканчивается ли строка на заданный суффикс
 * @param str Строка для проверки
 * @param suffix Искомое окончание
 * @return 1 если совпадает, 0 если нет
 */
static int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);
    if (len_suffix > len_str) return 0;
    return strncmp(str + len_str - len_suffix, suffix, len_suffix) == 0;
}

// -----------------------------------------------------------------------------
// Публичные функции
// -----------------------------------------------------------------------------


/**
 * @brief Инициализация файловой системы
 * 
 * Монтирует FatFS, форматирует карту при необходимости
 * @return FR_OK при успехе, код ошибки при неудаче
 */
FRESULT filesystem_init(void) {
    FRESULT res = f_mount(&fs, "", 1);

    if (res == FR_NOT_READY || res == FR_NO_FILESYSTEM || res == FR_INVALID_PARAMETER) {
        uart_puts("Formatting card...\r\n");

        static MKFS_PARM mkfs_opt = {
            .fmt = FM_FAT32,
            .n_fat = 1,
            .align = 0,
            .n_root = 512,
            .au_size = 0
        };
        static uint8_t work[4096];

        res = f_mkfs("", &mkfs_opt, work, sizeof(work));
        if (res != FR_OK) {
            return res;
            uart_puts("Formatting failed...\r\n");
        }
        else {
            uart_puts("Formatting good...\r\n");
        }

        res = f_mount(&fs, "", 1);
    }

    return res;
}


/**
 * @brief Тест записи файла
 */
void test_file_write(void) {
    FIL file;
    FRESULT res = f_open(&file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        uart_puts("f_open write failed: ");
        print_hex(res);
        uart_puts("\r\n");
        return;
    }

    const char *msg = "Hello from STM32F103! MicroSDHC + FatFS works!\r\n";
    UINT bytes_written;
    res = f_write(&file, msg, strlen(msg), &bytes_written);
    if (res != FR_OK) {
        uart_puts("f_write failed!\r\n");
    } else {
        uart_puts("Wrote ");
        if (bytes_written < 100) {
            uart_putc('0' + (bytes_written / 10));
            uart_putc('0' + (bytes_written % 10));
        }
        uart_puts(" bytes to test.txt\r\n");
    }

    f_close(&file);
}

/**
 * @brief Тест чтения файла
 */
void test_file_read(void) {
    FIL file;
    FRESULT res = f_open(&file, "test.txt", FA_READ);
    if (res != FR_OK) {
        uart_puts("f_open read failed: ");
        print_hex(res);
        uart_puts("\r\n");
        return;
    }

    uart_puts("File content:\r\n");

    uint8_t buffer[64];
    UINT bytes_read;

    while (1) {
        res = f_read(&file, buffer, sizeof(buffer), &bytes_read);
        if (res != FR_OK || bytes_read == 0) break;

        for (UINT i = 0; i < bytes_read; i++) {
            uart_putc(buffer[i]);
        }
    }

    f_close(&file);
    uart_puts("\r\n--- End of file ---\r\n");
}


/**
 * @brief Выводит список файлов в корневом каталоге
 * @param suffix Фильтр по расширению (например, ".TXT")
 *               Если передать ".*", выводит все файлы
 */
void list_files(const char *suffix) {
    DIR dir;
    FILINFO fno;
    FRESULT res;
    int show_all = 0;

    // Открываем корневой каталог
    res = f_opendir(&dir, "/");
    if (res != FR_OK) {
        uart_puts("f_opendir failed!\r\n");
        return;
    }

    // Выводим заголовок
    uart_puts("Files on SD card ");
    if (strncmp(suffix, ".*", 2) == 0) {
        uart_puts("of all types");
        show_all = 1;
    } else {
        uart_puts("of type ");
        uart_puts(suffix);
    }
    uart_puts(":\r\n");

    // Читаем записи по одной
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) {break; uart_puts("not files\r\n");}

        // Пропускаем папки, показываем файлы с нужным расширением или все
        if (!(fno.fattrib & AM_DIR) && (show_all || ends_with(fno.fname, suffix))) {
            uart_puts("  ");
            uart_puts(fno.fname);
            uart_puts("\r\n");
        }
    }
    f_closedir(&dir);
}

/**
 * @brief Сканирует файлы с заданным расширением и сохраняет в глобальный массив
 * @param suffix Фильтр по расширению (например, ".TXT")
 */
void scan_files(const char *suffix) {
    DIR dir;
    FILINFO fno;
    FRESULT res;

    file_count = 0;
    res = f_opendir(&dir, "/");
    if (res != FR_OK) {
        return;
    }

    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;

        // Если файл и подходит по расширению
        if (!(fno.fattrib & AM_DIR) && ends_with(fno.fname, suffix)) {
            if (file_count < MAX_FILES) {
                strncpy(file_list[file_count], fno.fname, FILENAME_LEN - 1);
                file_list[file_count][FILENAME_LEN - 1] = '\0';
                file_count++;
            }
        }
    }
    f_closedir(&dir);
}

/**
 * @brief Выводит список найденных файлов с индексами
 */
void print_file_list(void) {
    uart_puts("Available files:\r\n");
    for (uint8_t i = 0; i < file_count; i++) {
        uart_putc('[');
        uart_putc('0' + i);
        uart_puts("] ");
        uart_puts(file_list[i]);
        uart_puts("\r\n");
    }
}

/**
 * @brief Чтение из файла картинки и вывод ее на экран LCD
 * @param suffix название файла для открытия
 * @param buffer буфер для хранения части открытого файла 
 * для последующей передаче его на экран
 */
void file_read(const char *suffix, uint8_t *buffer, uint16_t len_b) {
    FIL file;
    FRESULT res = f_open(&file, suffix, FA_READ);
    if (res != FR_OK) {
        uart_puts("f_open read failed: ");
        print_hex(res);
        uart_puts("\r\n");
        return;
    }
    UINT bytes_read;

  
    //uart_puts("Читаем заголовок файла длиною 54 байта\r\n");
    uint8_t header[54];
    res = f_read(&file, header, 54, &bytes_read);
    uint32_t width =  header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
    uint32_t height = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
    uart_puts("BMP:");
    uart_puts("\r\nширина = ");
    print_hex(width);
    uart_puts("\r\nВысота = ");
    print_hex(height);
    uart_puts("\r\n");
    ILI9225_setWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    ILI9225_writeIndex(GRAM_DATA_REG);
	LCD_CS.activate();
    
    //uart_puts("Читаем файл кусками по 3300 байтов\r\n");
    //uart_puts("3300 байтов это 5 строчек по 220 пикселей (3 байта на пиксель)\r\n");
    while (1) {
        res = f_read(&file, buffer, len_b, &bytes_read);
        if (res != FR_OK || bytes_read == 0) break;
        ILI9225_Draw_File(buffer, len_b);
    }

    f_close(&file);

    uart_puts("\r\n--- End of file ---\r\n");
}