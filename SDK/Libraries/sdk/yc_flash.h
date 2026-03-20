/*
File Name    : yc_flash.h
Author       : Yichip
Version      : V1.0
Date         : 2024/5/11
Description  : flash driver interface define.
*/

#ifndef _yc_FLASH_H_
#define _yc_FLASH_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_PAGE_SIZE                 (256)

#define QSPI_BUSYTIMEOUT                (0x1000000)
#define DMA_WAIT_TIMEOUT                (0x10000)
#define P25X_PAGE_SIZE                  (0x100)
#define QSPI_DONE_BIT                   (BIT3)

#define QSPI_BUSYTIMEOUT                (0x1000000)
#define DMA_WAIT_TIMEOUT                (0x10000)

#define W25X_WRITE_ENABLE               0x06
#define W25X_WRITE_DISABLE              0x04
#define W25X_READ_STATUS1               0x05
#define W25X_READ_STATUS2               0x35
#define W25X_WRITE_STATUS               0x01
#define W25X_READ_DATA                  0x03
#define W25X_FASTREAD_DATA              0x0B
#define W25X_FASTREAD_DUAL1             0x3B
#define W25X_FASTREAD_DUAL2             0xBB

#define W25X_FASTREAD_QUAD1             0x6B
#define W25X_FASTREAD_QUAD2             0xEB
#define W25X_FASTREAD_QUAD3             0xE7

#define W25X_PAGE_PROGRAM               0x02
#define W25X_SECTOR_ERASE               0x20
#define W25X_BLOCK_ERASE32K             0x52
#define W25X_BLOCK_ERASE64K             0xD8
#define FlashCMD_PageErase              0x81
#define W25X_CHIP_ERASE                 0xC7
#define W25X_POWER_DOWN                 0xB9
#define W25X_RELEASE_POWERDOWN          0xAB
#define W25X_DEVICEID                   0xAB
#define W25X_MANUFACT_DEVICEID          0x90
#define W25X_JEDEC_DEVICEID             0x9F


#define QSPICFG_DUAL_MODE               (1 << 0)
#define QSPICFG_QUAD_MODE               (2 << 0)
#define QSPICFG_MBYTE                   (1 << 4)
#define QSPICFG_MBYTE_CONT              (1 << 5)
#define QCSFT_DUMMY                     (8)
#define QSPICFG_MODE_3B                 (QSPICFG_DUAL_MODE | 8 << QCSFT_DUMMY)
#define QSPICFG_MODE_6B                 (QSPICFG_QUAD_MODE | 8 << QCSFT_DUMMY)
#define QSPICFG_MODE_BB                 (QSPICFG_DUAL_MODE | QSPICFG_MBYTE | 0x6000)
#define QSPICFG_MODE_EB                 (QSPICFG_QUAD_MODE | QSPICFG_MBYTE | 0x6000 | 4 << QCSFT_DUMMY)
#define QSPICFG_MODE_E7                 (QSPICFG_QUAD_MODE | QSPICFG_MBYTE | 0x6000 | 2 << QCSFT_DUMMY)

#define PARAM_MCU_ADDR(flashAddr)       (((((flashAddr)-4096-8-16)/34)*32)|0x1000000)

uint8_t FLASH_Write(uint32_t flashAddr, uint32_t tlen, uint8_t *tbuf);
uint8_t FLASH_Read(uint32_t flashAddr, uint32_t rlen, uint8_t *rbuf);
uint8_t FLASH_Lock(void);
uint8_t FLASH_Unlock(void);
uint8_t FLASH_PageErase(uint32_t flashAddr);
uint8_t FLASH_SectorErase(uint32_t flashAddr);
uint8_t FLASH_BlockErase_32k(uint32_t flashAddr);
uint8_t FLASH_BlockErase_64k(uint32_t flashAddr);
uint8_t FLASH_Powerdown(void);
uint8_t FLASH_SetReadMode(uint8_t readModeCmd);
uint32_t FLASH_GetJedecId(void);
void FLASH_SetParamAddr(uint32_t paramOffset);
#ifdef __cplusplus
}
#endif

#endif /* _yc_FLASH_H_ */

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

