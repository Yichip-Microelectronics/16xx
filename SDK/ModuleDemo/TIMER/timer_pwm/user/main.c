
/* Includes ------------------------------------------------------------------*/
#include "yc_timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PWM_CLKDIV          (1)
#define PWM_CLK             TIMER_CLK
#define PWM_HZ(FREQ)        (PWM_CLK / (1 << PWM_CLKDIV) / FREQ)
#define PWM_KHZ(FREQ)       (PWM_CLK / (1 << PWM_CLKDIV) / 1000 / FREQ)
#define PWM_MHZ(FREQ)       (PWM_CLK / (1 << PWM_CLKDIV) / 1000000 / FREQ)
#define PWM_DUTY_RATIO      (0.5F)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  PWM core para config
 * @param  timx : the timer number,TIM0-TIM7
 * @param  clkDiv : the timer clock div(0-7)
 * @param  lowLevelPeriod :the PWM low level Period(Reload value)
 * @param  highLevelPeriod :the PWM high level Period(Reload value)
 * @retval None
 */
static void PWM_CoreParaConfig(TIM_PortEnum timx, uint8_t clkDiv, uint32_t lowLevelPeriod, uint32_t highLevelPeriod)
{
    _ASSERT(lowLevelPeriod < 0xFFFF);
    _ASSERT(highLevelPeriod < 0xFFFF);
    PWM_InitStruct PWM_InitStruct = {0};
    PWM_InitStruct.timx = timx;
    PWM_InitStruct.lowlevelperiod = lowLevelPeriod;
    PWM_InitStruct.highlevelperiod = highLevelPeriod;
    PWM_InitStruct.clockdiv = clkDiv;

    /*pwm core init*/
    TIM_PwmInit(&PWM_InitStruct);

    /*pwm enable*/
    TIM_Cmd(timx, ENABLE);
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
    yc_log("timer_pwm demo");
    yc_log("pwm_clock_freq: %d Hz", PWM_CLK);

    GPIO_Config(GPIO_PWM0_PIN, GPIO_MODE_PWM0_1_OUT);
    PWM_CoreParaConfig(TIM0,PWM_CLKDIV, (PWM_HZ(1000)*PWM_DUTY_RATIO), (PWM_HZ(1000)*PWM_DUTY_RATIO));

    GPIO_Config(GPIO_PWM1_PIN, GPIO_MODE_PWM0_3_OUT);
    PWM_CoreParaConfig(TIM1, PWM_CLKDIV, (PWM_HZ(1000)*PWM_DUTY_RATIO), (PWM_HZ(1000)*PWM_DUTY_RATIO));

    GPIO_Config(GPIO_PWM2_PIN, GPIO_MODE_PWM_OUT);
    PWM_CoreParaConfig(TIM2, PWM_CLKDIV, (PWM_HZ(1000)*PWM_DUTY_RATIO),(PWM_HZ(1000)*PWM_DUTY_RATIO));
    GPIO_Config(GPIO_PWM3_PIN, GPIO_MODE_PWM_OUT);
    PWM_CoreParaConfig(TIM3, PWM_CLKDIV, (PWM_HZ(1000)*PWM_DUTY_RATIO), (PWM_HZ(1000)*PWM_DUTY_RATIO));

    while (1)
    {

    }
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
