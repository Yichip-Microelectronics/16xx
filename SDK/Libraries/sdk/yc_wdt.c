/*
File Name    : yc_wdt.c
Author       : Yichip
Version      : V1.0
Date         : 2024/4/28
Description  : wdt driver interface implement.
*/


/* Includes ------------------------------------------------------------------*/

#include "yc_wdt.h"
#include "yc_sysctrl.h"

/**
 * @brief  init WDT.
 * @param  initStruct:configuration information struct of WDT init
 * @return None
 */
void WDT_Init(WDT_InitStruct* initStruct)
{

    CORE_CHIP_SYS->wdt_counter.bit.wdt_counter = 0;//must reset here
    CORE_CHIP_SYS->wdt_counter.bit.wdt_counter = initStruct->reload;
    if(initStruct->mode == RESET_MODE)
    {
        CORE_CHIP_SYS->wdt_counter.bit.wdt_int_en = 0;
    }
    else
    {
        CORE_CHIP_SYS->wdt_counter.bit.wdt_int_en = 1;
    }
    WDT_Enable();
}


/**
 * @brief  feeding watchdog.
 * @param  count:WDT reload value，WDT interrupt mod is overflow interrupt,overtime = (0x80000 - (count<<12))/(32768),32768 is wdt clock
 * @return None
 */
void WDT_SetReload(uint8_t count)
{
    CORE_CHIP_SYS->wdt_counter.bit.wdt_counter = count;
    SYSCTRL_WaitLpoEdge(1);
}

/**
 * @brief  enable WDT.
 * @param  None
 * @return None
 */
void WDT_Enable(void)
{
    LPM_CONTROL->ice_mode.bit.wd_en = 0;
    LPM_CONTROL->ice_mode.bit.wd_en = 1;
}
/**
 * @brief  disable WDT.
 * @param  None
 * @return None
 */
void WDT_Disable(void)
{

    LPM_CONTROL->ice_mode.bit.wd_en = 0;
}

/**
  * @brief  clear WDT interrupt flag.
  * @param  None
  * @return None
  */
void WDT_ClearIrqSatus(void)
{
    CORE_CHIP_SYS->wdt_counter.bit.wdt_int_en = 1;
    SYSCTRL_WaitLpoEdge(3);
}

/**
  * @brief  hold WDT when cpu wfi
  * @param  newState:ENABLE or DISABLE
  * @return None
  */
void WDT_WfiHoldEn(FunctionalState newState)
{
    CORE_CHIP_SYS->wdt_counter.bit.wdt_hold_en = newState;
}
