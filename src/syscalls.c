/**
 * @file syscalls.c
 * @brief Заглушки для системных вызовов newlib
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#undef errno
extern int errno;

// ============================================================================
// СИСТЕМНЫЕ ВЫЗОВЫ ДЛЯ NEWLIB
// ============================================================================

/**
 * @brief Close a file
 */
int _close(int file) {
    return -1;
}

/**
 * @brief Get status of a file
 */
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief Check if terminal
 */
int _isatty(int file) {
    return 1;
}

/**
 * @brief Seek in file
 */
int _lseek(int file, int ptr, int dir) {
    return 0;
}

/**
 * @brief Read from file
 */
int _read(int file, char *ptr, int len) {
    return 0;
}

/**
 * @brief Write to file (можно использовать для UART)
 */
int _write(int file, char *ptr, int len) {
    // Если нужно выводить через UART, можно раскомментировать:
    /*
    for(int i = 0; i < len; i++) {
        // uart_putc(ptr[i]);  // Твоя функция для отправки символа
    }
    */
    return len;
}

/**
 * @brief Program break (heap management)
 */
void *_sbrk(int incr) {
    extern char _end;   // Defined by linker
    extern char _estack; // Defined by linker
    static char *heap_end = 0;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &_estack) {
        // Heap overflow
        return (void *)-1;
    }

    heap_end += incr;
    return (void *)prev_heap_end;
}

/**
 * @brief Exit program
 */
void _exit(int status) {
    while(1) {
        // Бесконечный цикл при выходе
    }
}

/**
 * @brief Get process ID
 */
int _getpid(void) {
    return 1;
}

/**
 * @brief Kill process
 */
int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

/**
 * @brief Register environment
 */
char *__env[1] = { 0 };
char **environ = __env;