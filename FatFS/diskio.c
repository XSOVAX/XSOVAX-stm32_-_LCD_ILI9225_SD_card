#include "ff.h"
#include "diskio.h"
#include "SD_card.h"
#include <stdio.h>
#include <string.h>

DSTATUS disk_initialize(BYTE pdrv) {
    if(pdrv != 0) return STA_NOINIT;
    return (sd_init() == SD_OK) ? 0 : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != 0) return STA_NOINIT;
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0) return RES_PARERR;
    
    for (UINT i = 0; i < count; i++) {
        if (SD_ReadBlock(sector + i, buff + i * 512) != SD_OK) {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0) return RES_PARERR;
    
    for (UINT i = 0; i < count; i++) {
        if (SD_WriteBlock(sector + i, buff + i * 512) != SD_OK) {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if(pdrv != 0) return RES_PARERR;
    
    switch(cmd)
    {
        case CTRL_SYNC:
            return RES_OK;
            
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 15728640; // 8GB карта (точнее 7.5)
            return RES_OK;
            
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
            
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;
    }
    
    return RES_PARERR;
}

DWORD get_fattime(void) {
    // Возвращаем фиктивное время если нет RTC
    return ((DWORD)(2024 - 1980) << 25) | // 2024 год
           ((DWORD)1 << 21) |             // Январь
           ((DWORD)1 << 16) |             // 1 число  
           ((DWORD)0 << 11) |             // 0 часов
           ((DWORD)0 << 5) |              // 0 минут
           ((DWORD)0 >> 1);               // 0 секунд
}