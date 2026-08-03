#include "board_config.h"
#include "ota_ble.h"
#include "yc_wdt.h"
#include "yc_flash.h"
#include "app_ble.h"
#include "usb_ota.h"
#include "ota_config.h"
extern bool bBacktoAppFlag;
OTA_CONFIG ota_config={0};
OTA_MODE_Def ota_mode_flag=0;
void OTA_Process(OTA_MODE_Def ota_mode)
{
    if (ota_mode==USB_MODE)
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
    uint8_t RecvData = 0;
    yc_log("YC OTA BOOT AND APP Demo.\n");
    yc_log("/****************************/\n");
    yc_log("please input 'B' start Ble OTA Test.\n");
    yc_log("please input 'U' start USB OTA Test.\n");
    yc_log("/****************************/\n");
    Show_Current_Block();

    while(1)
    {
        if((UART_GetRxCount(PRINT_PORT) > 0)&&(ota_mode_flag==0))
        {
            RecvData = UART_ReceiveData(PRINT_PORT);
            yc_log("RecvData = %c.\n", RecvData);

            if(RecvData == 'B' || RecvData == 'b')
            {
                yc_log("Ble Init.\n");
                Ble_ParamInit();
                ota_mode_flag=BLE_MODE;
            }
            else if(RecvData == 'U' || RecvData == 'u')
            {
                yc_log("USB Init.\n");
                USB_Init();
                ota_mode_flag=USB_MODE;
            }
        }
        RunNewApp(ota_config.SetAppFlag);
        OTA_Process(ota_mode_flag);
    }
}
