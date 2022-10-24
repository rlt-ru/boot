#include "diskio.h" /* FatFs lower layer API */

#include <main.h>
#include <port.h>
#include <sdio.h>
#include <string.h>

#include "atomic.h"

#define SD_DISK_TIMEOUT 1000

#define SD_DMA_BUFFER_SIZE_IN_BLOCKS 0x08
BYTE sd_buffer[BLOCKSIZE * SD_DMA_BUFFER_SIZE_IN_BLOCKS]
    __attribute__((aligned(4)));

typedef struct {
  DSTATUS status;
} DiskIoContext;

static DiskIoContext diskIoContext = {
    .status = STA_NOINIT,
};

a_mux sd_mux = 0;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
/* Physical drive number to identify the drive */
DSTATUS disk_status(BYTE pdrv) {
  if (pdrv) {
    return STA_NOINIT;
  } else {
    return diskIoContext.status;
  }
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
/* Physical drive number to identify the drive */
DSTATUS disk_initialize(BYTE pdrv) {
  if (pdrv) {
    return STA_NOINIT;
  } else {
    MX_SDIO_SD_Init();

    a_mux_lock(&sd_mux);

    diskIoContext.status &= ~STA_NOINIT;

    return diskIoContext.status;
  }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,  /* Physical drive number to identify the drive */
                  BYTE *buff, /* Data buffer to store read data */
                  DWORD sector, /* Sector address in LBA */
                  UINT count    /* Number of sectors to read */
) {
  if (pdrv || !count) return RES_PARERR;
  if (diskIoContext.status & STA_NOINIT) return RES_NOTRDY;

  UINT cur_num_blocks = 0;
  UINT total_num_blocks = 0;
  BYTE *data = 0;
  UINT sd_addr = 0;

  __HAL_DMA_DISABLE(hsd.hdmarx);
  hsd.hdmarx->Init.Direction = DMA_PERIPH_TO_MEMORY;
  HAL_DMA_Init(hsd.hdmarx);

  total_num_blocks = count;
  data = (BYTE *)buff;
  sd_addr = sector;

  while (total_num_blocks) {
    if (total_num_blocks <= SD_DMA_BUFFER_SIZE_IN_BLOCKS)
      cur_num_blocks = total_num_blocks;
    else
      cur_num_blocks = SD_DMA_BUFFER_SIZE_IN_BLOCKS;

    a_mux_lock_try(&sd_mux);  // try lock
    if (HAL_SD_ReadBlocks_DMA(&hsd, sd_buffer, sd_addr, cur_num_blocks) !=
        HAL_OK) {
      HAL_DMA_Abort(hsd.hdmarx);
      return RES_ERROR;
    }

    UINT to = HAL_GetTick();
    while (!a_mux_lock_try(&sd_mux)) {
      if (HAL_GetTick() - to > SD_DISK_TIMEOUT) {
        HAL_DMA_Abort(hsd.hdmarx);
        return RES_ERROR;
      }
      HAL_Delay(1);
    }

    memcpy(data, sd_buffer, BLOCKSIZE * cur_num_blocks);

    total_num_blocks -= cur_num_blocks;
    data += BLOCKSIZE * cur_num_blocks;
    sd_addr += cur_num_blocks;
  }
  return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#define _USE_WRITE 1
#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, /* Physical drive number to identify the drive */
                   const BYTE *buff, /* Data to be written */
                   DWORD sector,     /* Sector address in LBA */
                   UINT count        /* Number of sectors to write */
) {
  if (pdrv || !count) return RES_PARERR;
  if (diskIoContext.status & (STA_NOINIT | STA_NODISK)) return RES_NOTRDY;
  if (diskIoContext.status & STA_PROTECT) return RES_WRPRT;

  UINT cur_num_blocks = 0;
  UINT total_num_blocks = 0;
  BYTE *data = 0;
  UINT sd_addr = 0;

  __HAL_DMA_DISABLE(hsd.hdmarx);
  hsd.hdmarx->Init.Direction = DMA_MEMORY_TO_PERIPH;
  HAL_DMA_Init(hsd.hdmarx);

  total_num_blocks = count;
  data = (BYTE *)buff;
  sd_addr = sector;

  while (total_num_blocks) {
    if (total_num_blocks <= SD_DMA_BUFFER_SIZE_IN_BLOCKS)
      cur_num_blocks = total_num_blocks;
    else
      cur_num_blocks = SD_DMA_BUFFER_SIZE_IN_BLOCKS;

    memcpy(sd_buffer, data, BLOCKSIZE * cur_num_blocks);

    a_mux_lock_try(&sd_mux);  // try lock
    if (HAL_SD_WriteBlocks_DMA(&hsd, sd_buffer, sd_addr, cur_num_blocks) !=
        HAL_OK) {
      HAL_DMA_Abort(hsd.hdmarx);
      return RES_ERROR;
    }

    UINT to = HAL_GetTick();
    while (!a_mux_lock_try(&sd_mux)) {
      if (HAL_GetTick() - to > SD_DISK_TIMEOUT) {
        HAL_DMA_Abort(hsd.hdmarx);
        return RES_ERROR;
      }
      HAL_Delay(1);
    }

    total_num_blocks -= cur_num_blocks;
    data += BLOCKSIZE * cur_num_blocks;
    sd_addr += cur_num_blocks;
  }

  return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#define _USE_IOCTL 1
#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, /* Physical drive number (0..) */
                   BYTE cmd,  /* Control code */
                   void *buff /* Buffer to send/receive control data */
) {
  HAL_SD_CardInfoTypeDef cardInfo;

  DRESULT res = RES_ERROR;

  if (pdrv) return RES_PARERR;
  if (diskIoContext.status & (STA_NOINIT | STA_NODISK)) return RES_NOTRDY;

  switch (cmd) {
    case CTRL_SYNC: /* Make sure that no pending write process */
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
      if (HAL_SD_GetCardInfo(&hsd, &cardInfo) == HAL_OK) {
        *(UINT *)buff = cardInfo.BlockNbr / cardInfo.BlockSize;
        res = RES_OK;
      }
      break;

    case GET_SECTOR_SIZE: /* Get R/W sector size (DWORD) */
    case GET_BLOCK_SIZE:  /* Get erase block size in unit of sector (DWORD) */
      if (HAL_SD_GetCardInfo(&hsd, &cardInfo) == HAL_OK) {
        *(UINT *)buff = cardInfo.BlockSize;
        res = RES_OK;
      }
      break;

    default:
      res = RES_PARERR;
      break;
  }

  return res;
}
#endif

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) { a_mux_unlock(&sd_mux); }

/**
 * @brief Tx Transfer completed callbacks
 * @param hsd: SD handle
 */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) { a_mux_unlock(&sd_mux); }

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) { a_mux_unlock(&sd_mux); }

DWORD get_fattime(void) { return 0; }
