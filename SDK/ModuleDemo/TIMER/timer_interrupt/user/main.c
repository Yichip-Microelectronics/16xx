#include "yc_plic.h"
#include "yc_timer.h"
#include "yc_vector.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIMER_CLKDIV    (7)
#define VECTOR_IRQ      (0)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/**
 * @brief  Timer core para config
 * @param  timx : the timer number,TIM0-TIM7
 * @param  clkDiv : the timer clock div(0-7)
 * @param  period : the timer period,MAX ms:(0xffff/(TIMER_CLK/pow(2,6)))*1000 ms,MIN ms:(0xffff/(TIMER_CLK/pow(2,1)))*1000 ms
 * @retval None
 */
static void TIM_CoreParaConfig(TIM_PortEnum timx, uint8_t clkDiv, uint32_t period)
{
    _ASSERT(period <= 0xffff);
    TIM_InitStruct TIM_InitStruct;
    TIM_InitStruct.timx = timx;
    TIM_InitStruct.period = period;
    TIM_InitStruct.clockdiv = clkDiv;
    TIM_InitStruct.autoLoadEn=ENABLE;
    /*timer core init*/
    TIM_Init(&TIM_InitStruct);

    /*timer it config*/
    TIM_ClearIrqStatus(timx);
    TIM_ITConfig(timx, ENABLE);

    /*timer enable*/
    TIM_Cmd(timx, ENABLE);
}

/**
 * @brief  Timer config.
 * @param  None
 * @retval None
 */
static void TIM_Configuration(void)
{

    TIM_CoreParaConfig((TIM_PortEnum)TIM0, TIMER_CLKDIV, TIMER_1MS(TIMER_CLKDIV) * 20);
    TIM_CoreParaConfig((TIM_PortEnum)TIM4, TIMER_CLKDIV, TIMER_1MS(TIMER_CLKDIV) * 50);
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
    yc_log("timer_interrupt demo");
    #if VECTOR_IRQ
    VECTOR_EnableIRQ(VECTOR_TIMERGR0_IRQn);
    #else
    PLIC_SetPriority(TIMERGR0_IRQn, 1);
    PLIC_EnableIRQ(TIMERGR0_IRQn);
    #endif
    PLIC_SetPriority(TIMERGR1_IRQn, 1);
    PLIC_EnableIRQ(TIMERGR1_IRQn);

    TIM_Configuration();
    yc_log("timer_clock_freq: %d Hz", TIMER_CLK);
    while (1)
    {
    }
}

#if VECTOR_IRQ
_interrupt void VECTOR_TIMERGR0_IRQ(void)
{
    uint8_t timerIrqStatus = TIM_GetAllITStatus();
    for (int i = 0; i <= TIM3; i++)
    {
        if (timerIrqStatus & (1 << i))
        {
            yc_log("TIM%d INTERRUPT !!!", i); /* TIM_I service function */
            GPIO_Config(GPIO_PWM2_PIN, GPIO_MODE_OUT_HIGH);
            GPIO_Config(GPIO_PWM2_PIN, GPIO_MODE_OUT_LOW);
            TIM_ClearIrqStatus(i);
        }
    }
}
#else
/**
 * @brief  Timer interrupt service function.
 * @param  None
 * @retval None
 */
void TIMERGR0_IRQHandler(void)
{
    uint8_t timerIrqStatus = TIM_GetAllITStatus();
    for (int i = 0; i <= TIM3; i++)
    {
        if (timerIrqStatus & (1 << i))
        {
            yc_log("TIM%d INTERRUPT !!!", i); /* TIM_I service function */
            GPIO_Config(GPIO_PWM1_PIN,GPIO_MODE_OUT_HIGH);
            GPIO_Config(GPIO_PWM1_PIN,GPIO_MODE_OUT_LOW);
            TIM_ClearIrqStatus(i);
        }
    }
}
#endif
void TIMERGR1_IRQHandler(void)
{
    uint8_t timerIrqStatus = TIM_GetAllITStatus();
    for (int i = TIM4; i <= TIM7; i++)
    {
        if (timerIrqStatus & (1 << i))
        {
            yc_log("TIM%d INTERRUPT !!!", i); /* TIM_I service function */
            GPIO_Config(GPIO_PWM0_PIN,GPIO_MODE_OUT_HIGH);
            GPIO_Config(GPIO_PWM0_PIN,GPIO_MODE_OUT_LOW);
            TIM_ClearIrqStatus(i);
        }
    }
}

// /************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
