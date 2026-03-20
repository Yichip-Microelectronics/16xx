#include "yc_rtc.h"
#include "yc_plic.h"
#include "yc_vector.h"
#include "yc_lpm.h"

#define VECTOR_IRQ_ENBALE           (0)
#define RTC_LPM_ENABLE              (0)
#define RTC_WAKEUP_TIME             (1000)  /*ms*/


struct tm timer_struct={        /* 初始时间: 2023-11-21 13:48:00 */
    .tm_sec = 0,
    .tm_min = 45,
    .tm_hour = 13,
    .tm_mday = 22,
    .tm_mon = 7,
    .tm_year = 2025,
};

time_t LPM_RETENTION last_calibration_time = 0;

void RTC_Configuration(void)
{
    RTC_DeInit();
    #if (RTC_LPM_ENABLE==0)
    RTC_SetAlarm1(RTC_GetCount(OV_INTR)+RTC_GetClk()); /* alarm1 timer 1 seconds */
    RTC_SetIrq(ALARM1,ENABLE);
    #endif
    RTC_Cmd(ENABLE);
}

#if (RTC_LPM_ENABLE)
void wakeup_FlagAnalysis(void)
{
    yc_log("wakeup_flag=%x",LpmRecoverStruct->wakeup_flag);
    if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_WAKE)
    {
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
            yc_log("rtc0 wakeup");
            RTC_WKConfig(ALARM0,DISABLE);
            if(difftime(RTC_GetTimeStamp(),last_calibration_time) >= 3600)    /* 校准频率为一小时一次，可以根据时间精度调整；当前校准精度为2048，理论耗时62.5ms */
            {
                RTC_Calibration();
                last_calibration_time = RTC_GetTimeStamp();
                yc_log("rtc_clk = %d",RTC_GetClk());
            }
            struct tm *ptime = RTC_GetTime();
            yc_log("%04d-%02d-%02d %02d:%02d:%02d", ptime->tm_year, ptime->tm_mon, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
        }
        if(LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_RTC1)
        {
            yc_log("rtc1 wakeup");
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
            yc_log("ram verify error");
        }
    }
}

void printport_deinit(void)
{
    UART_DeInit(PRINT_PORT);
    GPIO_Config(PRINT_TX_IO_PIN,GPIO_MODE_HI_Z);
    GPIO_Config(PRINT_RX_IO_PIN,GPIO_MODE_HI_Z);
}

void LPM_prepareActionBeforeSleep(LPM_ModeEnum sleepLevel)
{
    /*
    为了降低睡眠功耗，建议将睡眠后不使用的外设关闭
    并根据外围电路将睡眠期间不使用的GPIO配置成GPIO_MODE_HI_Z
    */

    LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.sleep=5;
    LpmRecoverStruct->rg_lpm_ldo_dig_lpm_vosel.bit.sleep=0;

    /*唤醒响应时间调整*/
    LPM_CONTROL->digldo_cnt.bit.digldo_cnt=0x0;
    LPM_CONTROL->digldo_cnt.bit.rco_cnt=0x0;
    LPM_CONTROL->xtal_cnt.reg=0x0;
    LPM_CONTROL->exen_cnt.reg=0x0;
    while(UART_GetTxCount(PRINT_PORT)>0);
    printport_deinit();
}

extern void __libc_init_array(void);
void LPM_postActionWakeup(LPM_ModeEnum sleepLevel)
{
    __libc_init_array();
    SYSCTRL_SystemInit();
    SYSCTRL_ChipInit();
    printport_init();
}
#endif

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("rtc timer demo");

    if((LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_WAKE) == 0)
    {
        delay_ms(500);/*此处延时为方便下载sync,应用里不需要加*/

        RTC_Calibration();
        yc_log("rtc_clk = %d",RTC_GetClk());
        RTC_Configuration();
        RTC_SetTime(timer_struct);
        last_calibration_time = RTC_GetTimeStamp();
        /*long long型的time_t数据类型打印需要去掉编译器全局附加选项的--specs=nano.specs,并使用%lld格式符,但会大大增加固件体积*/
        // yc_log("last_calibration_time = %lld",last_calibration_time);
    }
    #if (RTC_LPM_ENABLE)
    else
    {
        wakeup_FlagAnalysis();
    }
    #else
    #if VECTOR_IRQ_ENBALE
    VECTOR_EnableIRQ(VECTOR_RTC_IRQn);
    #else
    PLIC_SetPriority(RTC_IRQn,1);
    PLIC_EnableIRQ(RTC_IRQn);
    #endif
    #endif

    while(1)
    {
        #if RTC_LPM_ENABLE
        yc_log("lpm sleep");
        LPM_Sleep(RTC_WAKEUP_TIME,SLEEP_MODE_DEEP,0x1f,WAKEUP_FROM_SLEEP,LPM_prepareActionBeforeSleep,LPM_postActionWakeup);
        yc_log("wakeup");
        wakeup_FlagAnalysis();
        #endif
    }
}

#if (RTC_LPM_ENABLE==0)
#if VECTOR_IRQ_ENBALE
_interrupt void VECTOR_RTC_IRQ(void)
#else
void RTC_IRQHandler(void)
#endif
{
    if(RTC_GetIrqStauts(ALARM1))    /*将alarm1作为秒中断使用，实现日历功能*/
    {
        if(difftime(RTC_GetTimeStamp(),last_calibration_time) >= 3600)    /* 校准频率为一小时一次，可以根据时间精度调整；当前校准精度为2048，理论耗时62.5ms */
        {
            RTC_Calibration();
            last_calibration_time = RTC_GetTimeStamp();
            yc_log("rtc_clk = %d",RTC_GetClk());
        }
        struct tm *ptime = RTC_GetTime();
        yc_log("%04d-%02d-%02d %02d:%02d:%02d", ptime->tm_year, ptime->tm_mon, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
        RTC_SetAlarm1(RTC_GetCount(ALARM1)+RTC_GetClk());
        RTC_ClearIrq(ALARM1);
    }
    if(RTC_GetIrqStauts(OV_INTR))
    {
        yc_log("rtc reg over");
        RTC_ClearIrq(OV_INTR);
    }
}
#endif
