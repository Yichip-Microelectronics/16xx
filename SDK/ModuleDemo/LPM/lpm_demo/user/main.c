#include "yc_uart.h"
#include "yc_lpm.h"
#include "yc_systick.h"
#include "yc_rtc.h"
#include "yc_vector.h"
#include "yc_plic.h"


#define RTC_WAKEUP_TIME         (5000)/*ms*/

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
            yc_log("retention ram no keep");
        }
    }
}

void LPM_prepareActionBeforeSleep(LPM_ModeEnum sleepLevel)
{
    //gpio latch config
    GPIO_Config(GPIO_PWM0_PIN,GPIO_MODE_PULL_DOWN);
    LPM_SetGpioLatch(GPIO_PWM0_PIN,LATCH_PD);

    GPIO_Config(GPIO_PWM1_PIN,GPIO_MODE_PULL_UP);
    LPM_SetGpioLatch(GPIO_PWM1_PIN,LATCH_PU);

    GPIO_Config(GPIO_PWM2_PIN,GPIO_MODE_OUT_LOW);
    LPM_SetGpioLatch(GPIO_PWM2_PIN,LATCH_OUTPUT_LOW);

    GPIO_Config(GPIO_PWM3_PIN,GPIO_MODE_OUT_HIGH);
    LPM_SetGpioLatch(GPIO_PWM3_PIN,LATCH_OUTPUT_HIGH);
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
    SYSCTRL_ChipInit();
    printport_init();
}

uint8_t led_flag=0;
int main(void)
{
    SYSCTRL_ChipInit();

    printport_init();

    yc_log("lpm demo");

    wakeup_FlagAnalysis();

    while(1)
    {
        GPIO_Config(GPIO_LED_PIN,(GPIO_ModeEnum)led_flag);

        if(UART_GetRxCount(PRINT_PORT)>0)
        {
            switch (UART_ReceiveData(PRINT_PORT))
            {
                case 'l':
                    yc_log("lpm sleep");
                    LPM_Sleep(RTC_WAKEUP_TIME,SLEEP_MODE_DEEP,0x11,WAKEUP_FROM_SLEEP,LPM_prepareActionBeforeSleep,LPM_postActionWakeup);
                    yc_log("wakeup");
                    wakeup_FlagAnalysis();
                    break;

                case 's':
                    yc_log("deep sleep(no retention)");
                    LPM_Sleep(RTC_WAKEUP_TIME,SLEEP_MODE_DEEP,0x00,WAKEUP_FROM_MAIN,LPM_prepareActionBeforeSleep,LPM_postActionWakeup);
                    yc_log("deep sleep wakeup");
                    wakeup_FlagAnalysis();
                    break;

                default:
                    break;
            }
        }
    }
}

/**
  * @brief  Systick interrupt service function.
  * @param  None
  * @retval None
  */
_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    if(led_flag==GPIO_MODE_OUT_HIGH)
    {
        led_flag=GPIO_MODE_OUT_LOW;
    }
    else
    {
        led_flag=GPIO_MODE_OUT_HIGH;
    }
    SYSTICK_ClearIrq(); //must call this function at last!
}
