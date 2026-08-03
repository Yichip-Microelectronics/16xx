/*
File Name    : yc_systick.c
Author       : Yichip
Version      : V1.0
Date         : 2023/12/01
Description  : systick driver interface implement.
*/

#include "yc_vector.h"
#include "yc_systick.h"
#include "yc_rtc.h"
uint32_t LPM_RETENTION SystickCount=0;
uint32_t LPM_RETENTION SystickBackup=0;
uint32_t LPM_RETENTION RtcCntBackup=0;
uint32_t LPM_RETENTION SysTimerBackup=0;
uint32_t LPM_RETENTION SystickReloadValue=0;

/**
 * @name    SYSTICK_BtTimerCmd
 * @brief   enable ordisable BTTimer
 * @param   state:ENABLE: enable BTTimer
 *                DISABLE:disable BTTimer
 * @retval  None
 */
_ramcode
void SYSTICK_BtTimerCmd(FunctionalState state)
{
    if (state == ENABLE)
    {
        CORE_CHIP_SYS->sys_timer_en.bit.timer_enable = 1;
    }
    else
    {
        CORE_CHIP_SYS->sys_timer_en.bit.timer_enable = 0;
    }
}

/**
 * @name   SYSTICK_Disable
 * @brief  Disable systick timer
 * @param  None
 * @return None
 */
void SYSTICK_Disable(void)
{
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en = DISABLE;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_autoadd = DISABLE;
    VECTOR_DisableIRQ(VECTOR_SYSTICK_IRQn);
}

/**
 * @name   SYSTICK_Config
 * @brief  Initialize systick timer and start systick timer
 * @param  reload_value: systick timer period
 * @retval None
 */
void SYSTICK_Config(uint32_t reloadValue)
{
   _ASSERT(reloadValue <= 0xffffff);
    SYSTICK_Disable();
    SystickCount = 0;
    SystickReloadValue=reloadValue;
    CORE_CHIP_SYS->sys_timer_period.bit.sys_timer_period = reloadValue;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_autoadd = ENABLE;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en = ENABLE;
    VECTOR_EnableIRQ(VECTOR_SYSTICK_IRQn);
}

/**
 * @name   SYSTICK_Backup
 * @brief  backup systick and rtc count before lpm
 * @param  None:
 * @retval None
 */
void SYSTICK_Backup(void)
{
    if(CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en==DISABLE || LPM_RTC->rtc_ov_en.bit.rtc_enable==DISABLE)
    {
        return;
    }
    CORE_CHIP_SYS->rtc_systick_sync_cmd.bit.rtc_systick_sync_cmd=1;
    while(CORE_CHIP_SYS->rtc_systick_sync_cmd.bit.rtc_systick_sync_done==0);
    VECTOR_DisableIRQ(VECTOR_SYSTICK_IRQn);
    SystickBackup=CORE_CHIP_SYS->systick_sync.bit.systick_sync;
    RtcCntBackup=CORE_CHIP_SYS->rtc_cnt_sync.bit.rtc_cnt_sync;
    SysTimerBackup=CORE_CHIP_SYS->sys_timer.bit.sys_timer;
}

/**
 * @name   SYSTICK_Recover
 * @brief  recover systick timer and start systick timer
 * @param  None
 * @retval None
 */
void SYSTICK_Recover(void)
{
   uint32_t RtcRecoverCnt;
   uint32_t SystickRecoverCnt;
   uint32_t SystickTimerRecoverCnt;
   uint32_t RtcCntDiff;
   uint32_t TempCnt;

   if(LPM_RTC->rtc_ov_en.bit.rtc_enable==DISABLE)
    {
        return;
    }

    SYSTICK_Disable();

    RtcRecoverCnt=RTC_GetCount(OV_INTR)+10;/*同步时间点设在当前时间往后推延一段时间(计算和配置需要消耗时间)*/
    if(RtcRecoverCnt>=RtcCntBackup)
    {
        RtcCntDiff=RtcRecoverCnt-RtcCntBackup;
    }
    else
    {
        RtcCntDiff=((0xffffffff-RtcCntBackup)+1+RtcRecoverCnt);
    }

    uint64_t SystickCntDiff=(((uint64_t)SYSTICK_MHZ)*((uint64_t)RtcCntDiff))/(uint64_t)RTC_GetClk();
    if(SysTimerBackup>=SystickBackup)
    {
        TempCnt=SysTimerBackup-SystickBackup;
    }
    else
    {
        TempCnt=((0xffffffff-SystickBackup)+1+SysTimerBackup);
    }
    if(SystickCntDiff<TempCnt)
    {
        SystickRecoverCnt=SystickBackup+SystickCntDiff;
        SystickTimerRecoverCnt=SysTimerBackup;
    }
    else
    {
        SystickRecoverCnt=SystickBackup+TempCnt;
        SystickCntDiff -=TempCnt;
        SystickTimerRecoverCnt=SysTimerBackup+SystickReloadValue;
        while(SystickCntDiff>SystickReloadValue)
        {
            SystickRecoverCnt += SystickReloadValue;
            SystickCntDiff -=SystickReloadValue;
            SystickTimerRecoverCnt += SystickReloadValue;
        }
        SystickRecoverCnt += SystickCntDiff;
        SystickTimerRecoverCnt += SystickReloadValue;

    }

    CORE_CHIP_SYS->sys_timer_period.bit.sys_timer_period = SystickReloadValue;
    CORE_CHIP_SYS->systick_ovwr_dat.bit.systick_ovwr_dat=SystickRecoverCnt;
    CORE_CHIP_SYS->systick_ovwr_rtc_val.bit.systick_ovwr_rtc_val=RtcRecoverCnt;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_autoadd = ENABLE;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en = ENABLE;
    CORE_CHIP_SYS->rtc_systick_sync_cmd.bit.systick_ovwr_cmd=1;
    while(CORE_CHIP_SYS->rtc_systick_sync_cmd.bit.systick_ovwr_done==0);
    CORE_CHIP_SYS->sys_timer.bit.sys_timer = SystickTimerRecoverCnt;
    VECTOR_EnableIRQ(VECTOR_SYSTICK_IRQn);
}

/**
 * @name   SYSTICK_SetClk1M
 * @brief  set systick clk to 1M
 * @param  None
 * @retval None
 */
_ramcode void SYSTICK_SetClk1M(void)
{
    CORE_CHIP_SYS->systick_div1m.reg = (SYSCTRL_GetSystemClk()/1000000) - 1;
}
/**
 * @name   SYSTICK_GetReloadValue
 * @brief  Get systick timer period
 * @param  None
 * @retval systick timer period
 */
uint32_t SYSTICK_GetReloadValue(void)
{
    return CORE_CHIP_SYS->sys_timer_period.bit.sys_timer_period;
}
/**
 * @name   SYSTICK_GetTick
 * @brief  Get the current value of SystickCount
 * @param  None
 * @retval Current value of SystickCount
 */
uint32_t SYSTICK_GetTick(void)
{
    return SystickCount;
}
/**
 * @name   SYSTICK_GetRelativeTime
 * @brief  Get relative time
 * @param  startTick:start tick(get from SysTick_GetTick())
 * @retval The relative time(millisecond)
 */
uint32_t SYSTICK_GetRelativeTime(uint32_t startTick)
{
    uint32_t current_tick = SYSTICK_GetTick();
    uint32_t relative_tick = 0;
    if(current_tick >= startTick)
    {
        relative_tick = current_tick - startTick;
    }
    else
    {
        relative_tick = (0xFFFFFFFF - startTick) + 1 + current_tick;
    }
    return ((relative_tick*SystickReloadValue) / SYSTICK_ONE_MS);
}
/**
 * @name   SYSTICK_ClearIrq
 * @brief  Clear systick interrupt status, must call this function at VECTOR_SYSTICK_IRQ last.
 * @param  None
 * @retval None
 */
void SYSTICK_ClearIrq(void)
{
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en = DISABLE;
    CORE_CHIP_SYS->sys_timer_en.bit.sys_timer_en = ENABLE;
}
/**
 * @name   SYSTICK_IsTimeOut
 * @brief  Determine whether the timeout that millisecond
 * @param  start_tick:start tick(get from SysTick_GetTick())
 * @param  interval:time interval(ms)
 * @retval true is timeout ,false is not timeout
 */
bool SYSTICK_IsTimeOut(uint32_t startTick, uint32_t interVal)
{
    if (SYSTICK_GetRelativeTime(startTick) >= interVal)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
