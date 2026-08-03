#include <stdarg.h>
#include "register.h"
#include "yc_qspi.h"
#include "yc_sysctrl.h"
#include "yc_flash.h"
#include "flash.h"
// CRC32多项式
#define CRC32_POLYNOMIAL 0x04C11DB7
uint32_t crc32_write_res=0;
uint32_t crc32_read_res=0;
uint32_t crc32_byte(uint8_t* data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;  // 初始值
    
    for (uint32_t i = 0; i < length; i++) {
        crc ^= ((uint32_t)data[i] << 24);  // 将当前字节移到最高位
        
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
        crc = (crc << 1) ^ ((crc & 0x80000000) ? CRC32_POLYNOMIAL : 0);
    }
    
    return crc;  // 返回32位校验值
}
OTA_BLOCK_Def Get_OffsetAddr(void)
{
    uint8_t result = 0;
    uint32_t ota_offset_addr = 0x0;
    uint8_t rbuf[256] = {0};
    result = FLASH_Read(OTA_OFFSET_ADDR, 256,rbuf);
    if(result != SUCCESS)
    {
        yc_log("qspi page read error. \r\n");
    }

    ota_offset_addr = ((rbuf[2] << 16)|(rbuf[1] << 8)|rbuf[0]);
    if(ota_offset_addr == 0x001003)
    {
        return BLOCK_A;
    }
    else
    {
        return BLOCK_B;
    }
}

uint32_t UpdateFirmwareAddr(OTA_BLOCK_Def block)
{
    uint32_t start_addr = 0;

    /* 偏移地址 如果是A区,则擦除B区,是B区,则擦除A区 */
    if(block == BLOCK_A)
    {
        start_addr = OTA_BLOCK_B_ADDR;
    }
    else if(block == BLOCK_B)
    {
        start_addr = OTA_BLOCK_A_ADDR;
    }
    return start_addr;
}

void Flash_Erase(OTA_BLOCK_Def block)
{
    uint32_t start_addr = 0;

    start_addr = UpdateFirmwareAddr(block);

    yc_log("erase addr = %x \r\n", start_addr);

    for(uint8_t i = 0; i< (OTA_FIRMWARE_LEN / (4*1024)); i++)
    {
        FLASH_SectorErase(start_addr + (i * 0x1000));
    }
}

void RunNewApp(bool appflag)
{
    uint8_t result = 0;
    uint32_t start_addr = 0;
    uint8_t  buf[256] = {0};

    if(!appflag)
    {
        return;
    }

    /* 该芯片上电后,首先跑Boot,在Boot中去读flash的首三个字节来确定固件地址,所以跳app的操作就是将
        我们更新后固件的偏移地址写到flash的首三个字节,在进行软复位就可以运行升级后的固件了 */
    start_addr = UpdateFirmwareAddr(Get_OffsetAddr());

    result = FLASH_Read(OTA_OFFSET_ADDR,FLASH_PAGE_LEN, buf);
    if(result != SUCCESS)
    {
        yc_log("qspi page read error. \r\n");
    }

	result = FLASH_SectorErase(OTA_OFFSET_ADDR);
    if(result != SUCCESS)
    {
        yc_log("qspi page erase error. \r\n");
    }
    buf[2] = (start_addr >> 16) & 0xff;
    buf[1] = (start_addr >> 8) & 0xff;
    buf[0] = ((start_addr & 0xff) + 0x03);
    result = FLASH_Write(OTA_OFFSET_ADDR,FLASH_PAGE_LEN, buf);

    if(result == SUCCESS)
    {
        yc_log("ota success!\r\n");
        SYSCTRL_SoftReset();
    }
    else
    {
        yc_log("ota fail!\r\n");
    }
}


void Show_Current_Block(void)
{
    /* print App is run Block A or Block B */
    if(Get_OffsetAddr() == BLOCK_A)
    {
        yc_bt_log("app run block A \n");
    }
    else
    {
        yc_bt_log("app run block B \n");
    }
}

