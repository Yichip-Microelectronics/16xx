
/* Includes ------------------------------------------------------------------*/
#include "yc_timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PWM_CLKDIV              (6)
#define PWM_CLK                 TIMER_CLK
#define PWM_HZ(FREQ)            (PWM_CLK/(1 << PWM_CLKDIV)/FREQ)
#define PWM_KHZ(FREQ)           (PWM_CLK/(1 << PWM_CLKDIV)/1000/FREQ)
#define PWM_MHZ(FREQ)           (PWM_CLK/(1 << PWM_CLKDIV)/1000000/FREQ)

#define PWM_SYNC_PCNT           (PWM_HZ(200))//tim low
#define PWM_SYNC_NCNT           (PWM_HZ(400))
#define PWM_SYNC_PCNT_DEAD_TIME (TIMER_1MS(PWM_CLKDIV)*2)//tim high
#define PWM_SYNC_NCNT_DEAD_TIME (TIMER_1MS(PWM_CLKDIV)*4)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  PWM synchronized core para config
 * @param  timx : the timer number,TIM0-TIM7
 * @param  clkDiv : the timer clock div(1-6)
 * @param  lowLevelPeriod :the PWM low level Period(Reload value)
 * @param  highLevelPeriod :the PWM high level Period(Reload value)
 * @retval None
 */
void PWM_SyncCoreParaConfig(TIM_PortEnum TIMx, uint8_t clk_div, uint32_t LeftBorderPeriod , uint32_t high_level_period,uint32_t RightBorderPeriod , uint32_t low_level_period , uint8_t SyncOutputInverse)
{
    _ASSERT(LeftBorderPeriod <= 0xffff);
    _ASSERT(high_level_period <= 0xffff);
    _ASSERT(RightBorderPeriod <= 0xffff);
    _ASSERT(low_level_period <= 0xffff);
    PWM_SyncInitStruct PWM_InitSyncStruct = {0};
    PWM_InitSyncStruct.timx = TIMx;
    PWM_InitSyncStruct.lowLevelPeriod = low_level_period;
    PWM_InitSyncStruct.highLevelPeriod = high_level_period;

    PWM_InitSyncStruct.leftBorderPeriod = LeftBorderPeriod;
    PWM_InitSyncStruct.rightBorderPeriod = RightBorderPeriod;

    PWM_InitSyncStruct.clockDiv = clk_div;
    PWM_InitSyncStruct.startLevel = 0;
    PWM_InitSyncStruct.syncOutputInverse = SyncOutputInverse;

    /*pwm core init*/
    TIM_PwmSyncInit(&PWM_InitSyncStruct);
    TIM_Cmd(TIMx,ENABLE);
    TIM_Cmd(TIMx+1,ENABLE);
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("timer_pwm_synchronized demo");
    yc_log("pwm_clock_freq: %d Hz", PWM_CLK);

    GPIO_Config(GPIO_PWM0_PIN, GPIO_MODE_PWM_OUT);
    GPIO_Config(GPIO_PWM1_PIN, GPIO_MODE_PWM_OUT);

    PWM_SyncCoreParaConfig(TIM0, PWM_CLKDIV, PWM_SYNC_PCNT_DEAD_TIME  ,PWM_SYNC_PCNT ,PWM_SYNC_NCNT_DEAD_TIME ,PWM_SYNC_NCNT ,0);
    while (1)
    {
    }
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
