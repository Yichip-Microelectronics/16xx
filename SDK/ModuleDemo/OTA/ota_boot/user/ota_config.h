#ifndef _OTA_CONFIG_
#define _OTA_CONFIG_

#include <stdarg.h>
#include "register.h"

#define OTA_DEBUG           0
#define OTA_BUFFER_LEN      4096
typedef enum
{
    OTA_NONE        = 0x00,
    OTA_USB_MODE    = 0x01,
    OTA_BLE_MODE    = 0x02
}OTA_MODE_Def;

typedef struct
{
    bool    SetAppFlag;             //跳APP标志
    bool    isBufferFull;           //buf满
    uint32_t pos;                   //当前帧数偏移
    uint32_t offset;                //4K的偏移地址
    uint32_t updateaddr;            //app起始地址
    uint32_t writedLen;
    uint32_t Sumlength;
    uint32_t CheckSum;
    uint8_t buffer[OTA_BUFFER_LEN]; //升级buf 4K
}OTA_CONFIG;


extern OTA_CONFIG ota_config;

#endif

