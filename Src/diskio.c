/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdio.h"
#include "stm32f4xx_hal_sd.h"
#include "main.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	HAL_StatusTypeDef sd_state;

    sd_state = HAL_SD_GetCardInfo(&hsd, &card_info);
    if(sd_state != HAL_OK) {
        return RES_ERROR;
    }

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    uint16_t i = 0;
    for(i = 0; i < count; i++)
    {
        HAL_StatusTypeDef res;
        res = HAL_SD_ReadBlocks(&hsd, buff, sector, 1, 10000);
        if(res == HAL_OK)
        {
            while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
            {
            }
            buff += card_info.LogBlockSize;
            sector++;
        }
        else
        {
            return RES_ERROR;
        }
    }
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{

    uint16_t i = 0;
    for(i = 0; i < count; i++)
    {
        HAL_StatusTypeDef res;
        res = HAL_SD_WriteBlocks(&hsd, (uint8_t *)buff, sector, 1, 10000);
        if(res == HAL_OK)
        {
            while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
            {
            }
            buff += card_info.LogBlockSize;
            sector++;
        }
        else
        {
            return RES_ERROR;
        }
    }

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	UINT *result = (UINT *)buff;
	switch(cmd) {
		case GET_SECTOR_COUNT:
			*result = card_info.LogBlockNbr;
			break;
		case GET_SECTOR_SIZE:
			*result = card_info.LogBlockSize;
			break;
		case GET_BLOCK_SIZE:
			*result = 1;
			break;
		default:
			break;
	}
	return RES_OK;
}

