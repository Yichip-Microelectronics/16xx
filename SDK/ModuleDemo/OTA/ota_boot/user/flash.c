#include <stdarg.h>
#include "register.h"
#include "yc_plic.h"
#include "yc_qspi.h"
#include "yc_sysctrl.h"
#include "yc_flash.h"
#include "usbd_hid_core.h"
#include "usb_dcd_init.h"
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
void Flash_Erase(void)
{
    uint32_t start_addr = 0;

    start_addr = OTA_APP_ADDR;

    yc_log("erase addr = %x \r\n", start_addr);

    for(uint8_t i = 0; i< (OTA_FIRMWARE_LEN / (4*1024)); i++)
    {
        FLASH_SectorErase(start_addr + (i * 0x1000));
    }
}
extern USB_OTG_CORE_HANDLE  USB_OTG_dev;
extern USBD_DCD_INT_cb_TypeDef  *USBD_DCD_INT_fops;
void GoToApp(OTA_MODE_Def ota_mode)
{
    if(ota_mode == OTA_USB_MODE)
    {
        USBD_DCD_INT_fops->DevDisconnected(&USB_OTG_dev);
        PLIC_DisableIRQ(USB_IRQn);
    }
    else if(ota_mode == OTA_BLE_MODE)
    {
        PLIC_DisableIRQ(BTTIMER_IRQn);
        PLIC_DisableIRQ(BT_IRQn);
    }
     yc_log("go to APP");
    FLASH_SectorErase(FLASH_OTA_FLAG_ADDR);
    printport_deinit();
    GPIO_Config(BLE_OTA_PIN,GPIO_MODE_HI_Z);
    GPIO_Config(USB_OTA_PIN,GPIO_MODE_HI_Z);
    __disable_irq();
    ((void(*)(void))(((PARAM_MCU_ADDR(OTA_APP_ADDR+24))+0x80)|0x1000000))();
}
