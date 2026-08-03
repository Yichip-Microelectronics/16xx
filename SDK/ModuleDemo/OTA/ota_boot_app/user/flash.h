#include <stdarg.h>
#include "register.h"

typedef enum
{
    BLOCK_A     =   0x01,
    BLOCK_B     =   0x02,
}OTA_BLOCK_Def;

/* 自定义固件地址 */
#define OTA_OFFSET_ADDR         0x0
#define OTA_BLOCK_A_ADDR        0x01000    /* 4K  A Block的大小为100k */
#define OTA_BLOCK_B_ADDR        0x1a000    /* 100K -->end addr :0x33000*/
#define VENDER_BLOCK_ADDR       0x33000    /* 52k reserved */
#define OTA_FIRMWARE_LEN        100*1024   /*100K*/
#define FLASH_PAGE_LEN          256

OTA_BLOCK_Def Get_OffsetAddr(void);
uint32_t UpdateFirmwareAddr(OTA_BLOCK_Def block);
void Flash_Erase(OTA_BLOCK_Def block);
void RunNewApp(bool appflag);
void Show_Current_Block(void);
uint32_t crc32_byte(uint8_t* data, uint32_t length);


