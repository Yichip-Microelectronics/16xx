#include "board_config.h"
#include "ota_ble.h"
#include "yc_wdt.h"
#include "yc_flash.h"
#include "app_ble.h"
#include "usb_ota.h"
#include "ota_config.h"

#define FLASH_OTA_FLAG_USB  0xaa
#define FLASH_OTA_FLAG_BLE  0x55

OTA_CONFIG ota_config={0};
uint8_t flash_ota_flag=0;
OTA_MODE_Def ota_mode_flag=OTA_NONE;

void OTA_Process(OTA_MODE_Def ota_mode)
{
    if (ota_mode==OTA_USB_MODE)
    {
        Cmd_check();
    }
    else
    {
        IPC_HandleRxPacket();
    }
}
/* 本Demo演示了通过蓝牙进行固件升级 */
int main(void)
{
    SYSCTRL_ChipInit();
    SYSTICK_SetClk1M();
    printport_init();
    yc_log("YC OTA BOOT AND APP Demo.\n");
    yc_log("/****************************/\n");
    yc_log("Hold key1 start Ble OTA Test.\n");
    yc_log("Hold key2 start USB OTA Test.\n");
    yc_log("/****************************/\n");

    GPIO_Config(BLE_OTA_PIN,GPIO_MODE_PULL_UP);
    GPIO_Config(USB_OTA_PIN,GPIO_MODE_PULL_UP);
    FLASH_Read(FLASH_OTA_FLAG_ADDR,1,&flash_ota_flag);
    if (flash_ota_flag==FLASH_OTA_FLAG_USB)
    {
        ota_mode_flag=OTA_USB_MODE;
    }
    else if (flash_ota_flag==FLASH_OTA_FLAG_BLE)
    {
        ota_mode_flag=OTA_BLE_MODE;
    }
    else if (GPIO_ReadDataBit(BLE_OTA_PIN)==0)
    {
        delay_ms(50);
        if (GPIO_ReadDataBit(BLE_OTA_PIN)==0)
        {
            ota_mode_flag=OTA_BLE_MODE;
        }
    }
    else if (GPIO_ReadDataBit(USB_OTA_PIN)==0)
    {
        delay_ms(50);
        if (GPIO_ReadDataBit(USB_OTA_PIN)==0)
        {
            ota_mode_flag=OTA_USB_MODE;
        }
    }
    if(ota_mode_flag == OTA_USB_MODE)
    {
        yc_log("USB Init.\n");
        USB_Init();
    }
    else if(ota_mode_flag == OTA_BLE_MODE)
    {
        yc_log("Ble Init.\n");
        Ble_ParamInit();
    }
    while((ota_mode_flag == OTA_USB_MODE || ota_mode_flag == OTA_BLE_MODE) && ota_config.SetAppFlag==0)
    {
        OTA_Process(ota_mode_flag);
    }
    GoToApp(ota_mode_flag);
}




