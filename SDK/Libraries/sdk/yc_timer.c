/*
File Name    : yc_timer.c
Author       : Yichip
Version      : V1.0
Date         : 2023/12/12
Description  : timer driver interface implement.
*/

/* Includes ------------------------------------------------------------------*/
#include "yc_timer.h"

/**
 * @name    TIM_Init
 * @brief   timer initalize configuration
 * @param   initStruct: TIM initialize structure
 * @retval  None
 */
void TIM_Init(TIM_InitStruct* initStruct)
{
    _ASSERT(IS_TIMER_NUM(initStruct->timx));
    _ASSERT(IS_PERIOD_VALUE(initStruct->period));
    _ASSERT(IS_TIMER_CLKDIV(initStruct->clockdiv));

    TIM_Cmd(initStruct->timx, DISABLE);
    PWM_CTRL(initStruct->timx)->pwm0_ctrl.reg = 0x00;
    PWM_PCNT(initStruct->timx) = initStruct->period;
    PWM_NCNT(initStruct->timx) = 0;
    PWM_CTRL(initStruct->timx)->pwm0_ctrl.bit.load_cfg_enable = initStruct->autoLoadEn;

    PWM_CTRL((TIM_PortEnum)(initStruct->timx - (initStruct->timx % 2)))->pwm0_ctrl.bit.pwm01_clk_src_sel = initStruct->clockdiv;
    PWM_CTRL(initStruct->timx)->pwm0_ctrl.bit.timer_mode_en = ENABLE;
    PWM_CTRL(initStruct->timx)->pwm0_ctrl.bit.pwm0_pwm_en = ENABLE;
}
/**
 * @name    TIM_Getclk
 * @brief   Get TIMER/PWM clock
 * @param   None
 * @retval  TIMER/PWM clock
 */
uint32_t TIM_Getclk(void)
{
    uint32_t timer_clk = 0;
    if(CORE_CHIP_SYS->sys_clksel.bit.sys_clksel == SYS_CLKSEL_RC)
    {
        timer_clk = RC_CLK_SOURCE;
    }
    else if(CORE_CHIP_SYS->sys_clksel.bit.sys_clksel == SYS_CLKSEL_DPLL)
    {
        #if (BOARD_TYPE == BOARD_2020_022_1508FPGA_V_1_2)
        timer_clk = 24000000;
        #else
        timer_clk = 48000000;
        #endif
    }
    else
    {
        timer_clk = EXTERNAL_CRYSTAL;
    }
    return timer_clk;
}
/**
 * @name    TIM_DeInit
 * @brief   timer deinitialization
 * @param   timx : the timer number,TIM0-TIM7
 * @retval  None
 */
void TIM_DeInit(TIM_PortEnum timx)
{
    PWM_CTRL(timx)->pwm0_ctrl.reg=0;
}
/**
 * @name    TIM_Cmd
 * @brief   enable or disable timer
 * @param   timx : the timer number,TIM0-TIM7
 * @param   newState : ENABLE: enable specific timer
 *                     DISABLE: disable specific timer
 * @retval  None
 */
void TIM_Cmd(TIM_PortEnum timx, FunctionalState newState)
{
    _ASSERT(IS_TIMER_NUM(timx));

    if(newState != DISABLE)
    {
        PWM_CLKEN |= (1 << timx);
    }
    else
    {
        PWM_CLKEN &= ~(1 << timx);
    }
}

/**
 * @name    TIM_ITConfig
 * @brief   enable or disable the interrupt of specific timer
 * @param   timx : the timer number,TIM0-TIM7
 * @param   newState : ENABLE:enable the interrupt of specific timer
 *                     DISABLE:disable the interrupt of specific timer
 * @retval  None
 */
void TIM_ITConfig(TIM_PortEnum timx, FunctionalState newState)
{
    _ASSERT(IS_TIMER_NUM(timx));

    if(newState != DISABLE)
    {
        PWM_INT_EN |= (1 << timx);
    }
    else
    {
        PWM_INT_EN &= ~(1 << timx);
    }
}

/**
 * @name    TIM_ClearIrqStatus
 * @brief   Clear timer interrupt status, must call this function at TIMER_IRQHandler last.
 * @param   timx : the timer number,TIM0-TIM7
 * @retval  None
 */
void TIM_ClearIrqStatus(TIM_PortEnum timx)
{
    _ASSERT(IS_TIMER_NUM(timx));

    PWM_INT_EN &= ~(1 << timx);
    PWM_INT_EN |= (1 << timx);
}

/**
 * @name    TIM_GetITStatus
 * @brief   Get the specific TIM interrupt status
 * @param   timx : the timer number,TIM0-TIM7
 * @retval  The new state of specific TIM IT_Flag(SET or RESET)
 */
ITStatus TIM_GetITStatus(TIM_PortEnum timx)
{
    _ASSERT(IS_TIMER_NUM(timx));

    if(PWM_INT_STATUS & (1 << timx))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/**
 * @name   TIM_GetAllITStatus
 * @brief  Get all timer interrupt status
 * @param  None
 * @retval The new state of timer interrupt
 */
uint8_t TIM_GetAllITStatus(void)
{
    return ((uint8_t)PWM_INT_STATUS);
}


/**
 * @name    TIM_PwmInit
 * @brief   PWM initialize configuration
 * @param   initStruct : PWM initialize structure
 * @retval  None
 */
void TIM_PwmInit(PWM_InitStruct* initStruct)
{
    _ASSERT(IS_TIMER_NUM(initStruct->timx));
    _ASSERT(IS_TIMER_CLKDIV(initStruct->clockdiv));

    TIM_Cmd(initStruct->timx, DISABLE);

    PWM_CTRL(initStruct->timx)->pwm0_ctrl.reg = 0x00;

    PWM_PCNT(initStruct->timx) = initStruct->highlevelperiod;
    PWM_NCNT(initStruct->timx) = initStruct->lowlevelperiod;

    PWM_CTRL((TIM_PortEnum)(initStruct->timx - (initStruct->timx % 2)))->pwm0_ctrl.bit.pwm01_clk_src_sel = initStruct->clockdiv;
    if((PWM_CTRL((TIM_PortEnum)(initStruct->timx - (initStruct->timx % 2)))->pwm0_ctrl.bit.pwm0_syn_to_pwm1 != 1) || (initStruct->timx % 2 != 0))
    {
        PWM_CTRL(initStruct->timx)->pwm0_ctrl.bit.load_cfg_enable = ENABLE;
        PWM_CTRL(initStruct->timx)->pwm0_ctrl.bit.pwm0_pwm_en = ENABLE;
    }
}
/**
 * @name   TIM_SetTimerPeriod
 * @brief  Configure timer period
 * @param  timx : the timer number,TIM0-TIM7
 * @param  period :the timer period count(reload value)
 * @retval None
 */
void TIM_SetTimerPeriod(TIM_PortEnum timx, uint16_t period)
{
    _ASSERT(IS_TIMER_NUM(timx));
    uint8_t auto_load=PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable;
     if(auto_load)
     {
        PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
        delay_us(3);
     }
     PWM_PCNT(timx)=period;
     PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=ENABLE;
     if(!auto_load)
     {
        delay_us(3);
        PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
     }
}
/**
 * @name   TIM_SetPwmPeriod
 * @brief  Configure PWM Differential output
 * @param  timx : the timer number of output PWM(TIM0-TIM7)
 * @param  lowLevelPeriod :the PWM low level Period(reload value)
 * @param  highLevelPeriod :the PWM high level Period(reload value)
 * @retval None
 */
void TIM_SetPwmPeriod(TIM_PortEnum timx, uint16_t lowLevelPeriod, uint16_t highLevelPeriod)
{
    _ASSERT(IS_TIMER_NUM(timx));

    PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable = DISABLE;
    PWM_PCNT(timx) = highLevelPeriod;
    PWM_NCNT(timx) = lowLevelPeriod;
    PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable = ENABLE;
}
/**
 * @name    TIM_PwmSyncInit
 * @brief   PWM(sync mode) initalize configuration
 * @param   syncInitStruct : PWM sync initialize structure
 * @retval  None
 */
void TIM_PwmSyncInit(PWM_SyncInitStruct* syncInitStruct)
{
    TIM_Cmd(syncInitStruct->timx,DISABLE);
    TIM_Cmd(syncInitStruct->timx+1,DISABLE);
    PWM_CTRL(syncInitStruct->timx)->pwm0_ctrl.reg = 0x00;
    PWM_CTRL((syncInitStruct->timx+1))->pwm0_ctrl.reg = 0x00;

    PWM_CTRL(syncInitStruct->timx)->pwm0_ctrl.bit.pwm0_syn_to_pwm1=ENABLE;
    PWM_CTRL((syncInitStruct->timx+1))->pwm0_ctrl.bit.pwm0_syn_to_pwm1=syncInitStruct->syncOutputInverse;

    PWM_CTRL((syncInitStruct->timx+1))->pwm0_ctrl.bit.pwm0_init=syncInitStruct->startLevel;

    PWM_CTRL(syncInitStruct->timx)->pwm0_ctrl.bit.pwm01_clk_src_sel=syncInitStruct->clockDiv;

    PWM_CTRL((syncInitStruct->timx+1))->pwm0_ctrl.bit.pwm0_pwm_en=ENABLE;

    PWM_CTRL((syncInitStruct->timx+1))->pwm0_ctrl.bit.load_cfg_enable=ENABLE;

    PWM_PCNT(syncInitStruct->timx)=syncInitStruct->highLevelPeriod;
    PWM_NCNT(syncInitStruct->timx)=syncInitStruct->leftBorderPeriod;
    PWM_PCNT((syncInitStruct->timx+1))=syncInitStruct->lowLevelPeriod;
    PWM_NCNT((syncInitStruct->timx+1))=syncInitStruct->rightBorderPeriod;
}
/**
 * @name    TIM_PwmSyncCmd
 * @brief   enable or disable pwm(sync)
 * @param   timx : the timer number of output PWM(TIM0,TIM2,TIM4,TIM6)
 * @param   newState :ENABLE:enable specific PWM
 *                    DISABLE:disable specific PWM
 * @retval  None
 */
void TIM_PwmSyncCmd(TIM_PortEnum timx, FunctionalState newState)
{
    _ASSERT(IS_TIMER_NUM(timx));
    if(newState == ENABLE)
    {
        PWM_CTRL(timx)->pwm0_ctrl.bit.pwm0_syn_to_pwm1=ENABLE;
    }
    else
    {
        PWM_CTRL(timx)->pwm0_ctrl.bit.pwm0_syn_to_pwm1=DISABLE;
    }
}
/**
 * @name   TIM_SetPwmSyncPeriod
 * @brief  Configure PWM period(sync)
 * @param  timx : the timer number of output PWM(TIM0,TIM2,TIM4,TIM6)
 * @param  lowLevelPeriod :the PWM low level Period(Reload value)
 * @param  highLevelPeriod :the PWM high level Period(Reload value)
 * @param  leftBorderPeriod :the left boundary dead zone protection clock cycles
 * @param  rightBorderPeriod :The right boundary dead zone protection clock cycles
 * @retval None
 */
void TIM_SetPwmSyncPeriod(TIM_PortEnum timx, uint16_t leftBorderPeriod, uint16_t highLevelPeriod, uint16_t rightBorderPeriod, uint16_t lowLevelPeriod)
{
    uint8_t auto_load=PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable;
     if(auto_load)
     {
        PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
        PWM_CTRL(timx+1)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
        delay_us(3);
     }
     PWM_PCNT(timx)=highLevelPeriod;
     PWM_NCNT(timx)=leftBorderPeriod;
     PWM_PCNT(timx+1)=lowLevelPeriod;
     PWM_NCNT(timx+1)=rightBorderPeriod;
     PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=ENABLE;
     PWM_CTRL(timx+1)->pwm0_ctrl.bit.load_cfg_enable=ENABLE;
     if(!auto_load)
     {
        delay_us(3);
        PWM_CTRL(timx)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
        PWM_CTRL(timx+1)->pwm0_ctrl.bit.load_cfg_enable=DISABLE;
     }
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

