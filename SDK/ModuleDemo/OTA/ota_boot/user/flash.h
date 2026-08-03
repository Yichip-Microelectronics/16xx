#include <stdarg.h>
#include "register.h"
#include "ota_config.h"

typedef enum
{
    BLOCK_A     =   0x01,
    BLOCK_B     =   0x02,
}OTA_BLOCK_Def;

/* 自定义固件地址 */
#define OTA_OFFSET_ADDR         0x0
#define OTA_APP_ADDR            0x23000    /* APP固件起始地址*/
#define OTA_FIRMWARE_LEN        100*1024   /*100K*/
#define FLASH_PAGE_LEN          256
#define FLASH_OTA_FLAG_ADDR     0x3a000
uint32_t UpdateFirmwareAddr(OTA_BLOCK_Def block);
void Flash_Erase(void);
void GoToApp(OTA_MODE_Def ota_mode);
uint32_t crc32_byte(uint8_t* data, uint32_t length);


