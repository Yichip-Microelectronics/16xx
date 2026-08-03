#include "le.h"
#include "yc_bt.h"
#include "yc_user.h"
#include "yc_ipc.h"
#include "yc_log.h"
#include "yc_systick.h"
#include "yc_lpm.h"
#include "yc_rtc.h"
#include "BLE_L2capATT_List.h"

#define BLE_LOCAL_MTU 240

#define BLE_ADV_INTERVAL        (30000) //30ms
#define BLE_SCAN_INTERVAL       (20000)
#define BLE_SCAN_WINDOW         (3000)
/**************************蓝牙Public地址***************************/
//ble 从机地址，前四位有效，后两位为flash ID
static uint8_t ble_addr_public[6] = {0x65,0x22,0x72,0x30,0x00,0x00};
//ble 做主的连接地址
static uint8_t ble_addr_con[6] = {0x11,0x22,0x33,0x33,0x22,0x11};
/**************************蓝牙LL 参数****************************/
static const uint8_t ll_param[13] = {0x21,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x0b, 0x0e, 0x05, 0x5a, 0x02};
/**************************蓝牙设备名***************************/
//最大不能超过16个字节
static char ble_name[15] = "yc1161-";
/**************************蓝牙连接参数***************************/
const uint8_t ble_conn_param[8]  =
{
    0x18, 0x00,         //minimum connection interval. unit: 1.25ms
    0x24, 0x00,         //maximum connection interval, in normal case can be set same as min_intv. unit: 1.25ms
    0x00, 0x00,         //slave latency, number of connection events that slave would like to skip.
    0x2c, 0x01          //timeout before connection is dropped. unit: 10ms
};

extern LPM_RETENTION le_att_entry *ble_user_list_ptr;

// Attribute List init
struct ble_gatt_service_static *gatt_svc[] =
{
    (struct ble_gatt_service_static *) &Generic_Access,
    (struct ble_gatt_service_static *) &Generic_Attribute,
    (struct ble_gatt_service_static *) &User_Service,
    NULL,
};

LPM_RETENTION le_att_entry ble_user_list[]=
{
    {ATTOP_READ_RESPONSE                ,NULL},
    {ATTOP_HANDLE_VALUE_NOTIFICATION    ,U_BleMasterParseNotifyData},
    {ATTOP_HANDLE_VALUE_INDICATION      ,U_BleMasterParseNotifyData},
};

void LPM_prepareActionBeforeSleep(LPM_ModeEnum sleepLevel);
void LPM_postActionWakeup(LPM_ModeEnum sleepLevel);
void wakeup_FlagAnalysis(void);


extern void __libc_init_array(void);


void Ble_MasterParamInit(void)
{
    uint16_t version = BT_GetLibVersion();
    yc_log("bt_lib_version:%04x",version);

    init_le();
    ble_user_list_ptr = ble_user_list;
    //设置BLE地址，public or random.
    BT_BleSetMac((uint8_t*)ble_addr_public);
    //设置BLE名称
    U_BleSetDeviceName((char *)ble_name,(uint8_t*)ble_addr_public);
    yc_bt_log("ble_device_name:%s", ble_name);
    //设置本地MTU
    BT_BleSetMtu(BLE_LOCAL_MTU);
    //从Flash读NVRAM配置
    BT_NvramLoad();
    //设置LL参数
    BT_SetLLParam((uint8_t*)ll_param);
    //设置ble master连接地址
    BT_SetBleConnAddr((uint8_t*)ble_addr_con);
    //设置BLE扫描使能
    BT_SetScanEnable(ENABLE,BLE_SCAN_INTERVAL,BLE_SCAN_WINDOW);
    //查询蓝牙状态
    //yc_bt_log("Bt state = 0x%x", BT_GetStatus());

}


void app_init(void)
{
    if((LpmRecoverStruct->wakeup_flag&(WAKEUP_FLAG_WAKE|WAKEUP_FLAG_RAM_CRC_ERR))==WAKEUP_FLAG_WAKE && \
        LpmRecoverStruct->rg_lpm_ldo_dig_lpm_vosel.bit.sleep_mode==SLEEP_MODE_LPM)
    {
        //带蓝牙lpm
        #ifndef YC_BT_BB_LOG_SWITCH
        printport_init();
        #endif
        yc_log("lpm wake from %s",LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.wake_mode == WAKEUP_FROM_SLEEP ? "sleep" : "main");
        wakeup_FlagAnalysis();
    }
    else
    {
        SYSCTRL_ChipInit();
        SYSTICK_SetClk1M();
        printport_init();
        if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_WAKE)
        {
            yc_log("deep wake from %s",LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.wake_mode == WAKEUP_FROM_SLEEP ? "sleep" : "main");
            wakeup_FlagAnalysis();
        }
        else
        {
            yc_log("YC16xx ble demo!");
        }
        Ble_MasterParamInit();
        rtc_cali_start(RTC_CALI_CNT);
    }

}


void wakeup_FlagAnalysis(void)
{
    yc_log("wakeup_flag=%x",LpmRecoverStruct->wakeup_flag);
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_KEY_SCAN)
    {
        yc_log("key scan wakeup");
    }
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_RTCOV)
    {
        yc_log("rtc over wakeup");
        RTC_WKConfig(OV_INTR,DISABLE);
    }
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_RTC0)
    {
        yc_log("ble event wakeup");
        RTC_WKConfig(ALARM0,DISABLE);
    }
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_RTC1)
    {
        yc_log("user timeout wakeup");
        RTC_WKConfig(ALARM1,DISABLE);
    }
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_GPIO)
    {
        yc_log("gpio wakeup");
        yc_log_buf("gpio_in",LpmRecoverStruct->gpio_in,sizeof(LpmRecoverStruct->gpio_in));
        for(int i=0;i<GPIO_NUM;i++)
        {
            uint8_t wake_cfg=((LPM_PUWAKEUP->gpio_wake_cfg[i/2]>>((i%2)*4))&0x0f);
            if(LpmRecoverStruct->gpio_in[i/8]&(1<<(i%8)))/*gpio i input high*/
            {
                if(wake_cfg==SLEEP_HIGHWAKE || wake_cfg==SLEEP_PULLDOWN_HIGHWAKE)
                {
                    yc_log("gpio%d high wakeup",i);
                }
            }
            else/*gpio i input low*/
            {
                if(wake_cfg==SLEEP_LOWWAKE || wake_cfg==SLEEP_PULLUP_LOWWAKE)
                {
                    yc_log("gpio%d low wakeup",i);
                }
            }
        }
    }
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_RAM_CRC_ERR)
    {
        yc_log("retention ram no keep");
    }
}

void LPM_prepareActionBeforeSleep(LPM_ModeEnum sleepLevel)
{
    //gpio latch config
    GPIO_Config(GPIO_PWM0_PIN,GPIO_MODE_PULL_DOWN);
    LPM_SetGpioLatch(GPIO_PWM0_PIN,LATCH_PD);
    //gpio wakeup config
    GPIO_Config(KEY1_PIN,GPIO_MODE_PULL_UP);
    LPM_SetGpioLatch(KEY1_PIN,LATCH_PU);
    GPIO_SetWakeupType(KEY1_PIN,SLEEP_LOWWAKE);

    /*
    为了降低睡眠功耗，建议将睡眠后不使用的外设关闭
    并根据外围电路将睡眠期间不使用的GPIO配置成GPIO_MODE_HI_Z
    */
    __disable_irq();
    /*保电电压配置*/
    LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.sleep=5;
    LpmRecoverStruct->rg_lpm_ldo_dig_lpm_vosel.bit.sleep=2;
    printport_deinit();
}

void LPM_postActionWakeup(LPM_ModeEnum sleepLevel)
{
    app_init();
}


uint8_t lpm_mode = SLEEP_MODE_NONE;
void UART_DebugTest()
{
        if(UART_GetRxCount(PRINT_PORT))
        {
            switch(UART_ReceiveData(PRINT_PORT))
            {
                case 'l':
                    yc_log("lpm sleep");
                    lpm_mode = SLEEP_MODE_LPM;
                    break;
                case 's':
                    yc_log("deep sleep");
                    lpm_mode = SLEEP_MODE_DEEP;
                    break;
                default:
                    break;
            }
        }

}

void LPM_Dispatch(void)
{
    if(lpm_mode != SLEEP_MODE_NONE)
    {
        if(lpm_mode == SLEEP_MODE_LPM)
        {
            if(LPM_Sleep(0,SLEEP_MODE_LPM,0x11,WAKEUP_FROM_SLEEP,LPM_prepareActionBeforeSleep,LPM_postActionWakeup))
            {
                lpm_mode = SLEEP_MODE_NONE;
                yc_log("lpm wakeup");
            }
            else
            {
                yc_log("try lpm sleep fail");
            }
        }
        else
        {
            if(LPM_Sleep(10000,SLEEP_MODE_DEEP,0,WAKEUP_FROM_MAIN,LPM_prepareActionBeforeSleep,LPM_postActionWakeup))
            {
                lpm_mode = SLEEP_MODE_NONE;
                yc_log("deep wakeup");
            }
            else
            {
                yc_log("try deep sleep fail");
            }
        }
    }
}

int main(void)
{
    app_init();
    while(1)
    {
        IPC_HandleRxPacket();
        UART_DebugTest();
        LPM_Dispatch();
    }

}

