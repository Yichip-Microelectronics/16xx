/*
File Name    : yc_rtc.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/26
Description  : RTC driver interface implement.
*/

#include "yc_rtc.h"
#include "yc_sysctrl.h"

#define LPO_CAIL_COUNT          (0xb) /*2^n lpo clock*/

time_t LPM_RETENTION RtcUnixTimestamp=0;
uint32_t LPM_RETENTION RtcLastCnt=0;
uint32_t LPM_RETENTION RtcClk=32768;
/**
 * @name     RTC_Cmd
 * @brief    enable or disable RTC.
 * @param    state:ENABLE:enable RTC
 *                 DISABLE:disable RTC
 * @return   None
 */
void RTC_Cmd(FunctionalState state)
{
    LPM_RTC->rtc_ov_en.bit.rtc_enable = state;
    SYSCTRL_WaitLpoEdge(3);
}

/**
 * @name     RTC_DeInit
 * @brief    Deinitializes the RTC registers to their default reset values.
 * @param    None
 * @return   None
 */
void RTC_DeInit(void)
{
    LPM_RTC->rtc_ov_en.reg = 0;
}

/**
 * @name     RTC_Calibration
 * @brief    Calibrate RTC clock
 * @param    None
 * @return   None
 */
void RTC_Calibration(void)
{
    CORE_LPO->lpo_counter_cali.bit.lpo_counter_cali = LPO_CAIL_COUNT;
    CORE_LPO->lpo_counter_cali.bit.lpo_cali_start = 1;
    while(!(CORE_LPO->lpo_cali_done.bit.lpo_cali_done));
    RtcClk = (uint32_t)((((uint64_t)(1<< LPO_CAIL_COUNT)) * ((uint64_t)SYSCTRL_GetSystemClk()))/((uint64_t)CORE_LPO->lpo_cali_done.bit.lpo_cali_cnt));
}

/**
 * @name     RTC_GetClk
 * @brief    Get RTC clock
 * @param    none
 * @return   RTC clock
 */
uint32_t RTC_GetClk(void)
{
    return RtcClk;
}

/**
 * @name     RTC_ClkSel
 * @brief    Select RTC clock source
 * @param    clkSel:RC_32K or XO_32K
 * @return   None
 */
void RTC_ClkSel(RTC_ClkSelEnum clkSel)
{
    GPIO_Config(GPIO_8,GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_10,GPIO_MODE_HI_Z);
    if(clkSel == XO_32K)
    {
        GPIO_AnalogEn(GPIO_8,ENABLE);
        GPIO_AnalogEn(GPIO_10,ENABLE);
        if(LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.da_lpm_xo32k_en == 0)
        {
            LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.da_lpm_xo32k_en = 1;
        }
        LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.rg_lpm_xo32k_disconnect = 0;

        if(LPM_CONTROL->lpm_xo32k_sel.reg == 0)
        {
            LPM_CONTROL->lpm_xo32k_sel.reg = 1;
        }
        delay_ms(100);
    }
    else
    {
        if(LPM_CONTROL->lpm_xo32k_sel.reg == 1)
        {
            LPM_CONTROL->lpm_xo32k_sel.reg = 0;
        }
        if(LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.da_lpm_xo32k_en == 1)
        {
            LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.da_lpm_xo32k_en = 0;
        }
        LPM_HW_CONTROL_NOEXP->rg_lpm_xo32k_cgm_isel.bit.rg_lpm_xo32k_disconnect = 1;
        GPIO_AnalogEn(GPIO_8,DISABLE);
        GPIO_AnalogEn(GPIO_10,DISABLE);
    }
}
/**
 * @name     RTC_SetIrq
 * @brief    Configure RTC interrupts.
 * @param    type: 0:ALARM0
 *                 1:ALARM1
 *                 2:OV_INTR.
 * @param    state: ENABLE/DISABLE.
 * @return   None
 */
void RTC_SetIrq(RTC_CompEnum type, FunctionalState state)
{
    if(type==ALARM0)
    {
        LPM_RTC->rtc_ov_en.bit.rtc_comp0_en = state;
        SYSCTRL_WaitLpoEdge(3);
    }
    else if (type==ALARM1)
    {
        LPM_RTC->rtc_ov_en.bit.rtc_comp1_en = state;
        SYSCTRL_WaitLpoEdge(3);
    }
    else
    {
        LPM_RTC->rtc_ov_en.bit.rtc_ov_en = state;
        SYSCTRL_WaitLpoEdge(3);
    }
}

/**
 * @name     RTC_WKConfig
 * @brief    Configure RTC wakeup.
 * @param    type: 0:ALARM0
 *                 1:ALARM1
 *                 2:OV_INTR.
 * @param    state: ENABLE/DISABLE.
 * @return   None
 */
void RTC_WKConfig(RTC_CompEnum type, FunctionalState state)
{
    if(state == ENABLE)
    {
        LPM_RTC->rtc_ov_en.bit.rtc_wk_en |= (1<<type);
        SYSCTRL_WaitLpoEdge(3);
    }
    else
    {
        LPM_RTC->rtc_ov_en.bit.rtc_wk_en &= ~(1<<type);
        SYSCTRL_WaitLpoEdge(3);
    }
}

/**
 * @name      RTC_SetAlarm0
 * @brief     RTC ALARM0 reload comp0 counter value.
 * @param     alarmValue : comp0 count value.
 * @return    None
 */
void RTC_SetAlarm0(uint32_t alarmValue)
{
    LPM_RTC->rtc_ov_en.bit.rtc_comp0_en = 0;
    SYSCTRL_WaitLpoEdge(3);
    LPM_RTC->rtc_comp0.reg = alarmValue;
    LPM_RTC->rtc_ov_en.bit.rtc_comp0_en = 1;
}

/**
 * @name      RTC_SetAlarm1
 * @brief     RTC ALARM1 reload comp1 counter value.
 * @param     alarmValue : comp1 count value.
 * @return    None
 */
void RTC_SetAlarm1(uint32_t alarmValue)
{
    LPM_RTC->rtc_ov_en.bit.rtc_comp1_en = 0;
    SYSCTRL_WaitLpoEdge(3);
    LPM_RTC->rtc_comp1.reg = alarmValue;
    LPM_RTC->rtc_ov_en.bit.rtc_comp1_en = 1;
}


/**
 * @name      RTC_GetCount
 * @brief     get RTC ALARM0/ALARM1/RTC counter value.
 * @param     type : 0:ALARM0
 *                   1:ALARM1
 *                   2:OV_INTR.
 * @return    ALARM0/ALARM1/RTC counter value
 */
uint32_t RTC_GetCount(RTC_CompEnum type)
{
    if(type==ALARM0)
    {
        return LPM_RTC->rtc_comp0.reg;
    }
    else if (type==ALARM1)
    {
        return LPM_RTC->rtc_comp1.reg;
    }
    else
    {
        return LPM_RTC->rtc_cnt.reg;
    }
}

/**
 * @name      RTC_GetIrqStauts
 * @brief     get RTC interrupt status bits.
 * @param     type : 0:ALARM0
 *                   1:ALARM1
 *                   2:OV_INTR.
 * @return    RTC interrupt status
 */
uint8_t RTC_GetIrqStauts(RTC_CompEnum type)
{
    if(type==ALARM0)
    {
        return LPM_RTC->rtc_ov_en.bit.intr_rtc0;
    }
    else if (type==ALARM1)
    {
        return LPM_RTC->rtc_ov_en.bit.intr_rtc1;
    }
    else
    {
        return LPM_RTC->rtc_ov_en.bit.intr_rtc_ov;
    }
}
/**
 * @name      RTC_SetTime
 * @brief     set RTC initial time.
 * @param     dateStruct : initial time structure.
 * @return    None
 */
void RTC_SetTime(struct tm dateStruct)
{
    dateStruct.tm_year -=1900;
    dateStruct.tm_mon -=1;
    RtcUnixTimestamp = mktime(&dateStruct);
    RtcLastCnt=RTC_GetCount(OV_INTR);
}

/**
 * @name      RTC_GetTime
 * @brief     Get RTC current time.
 * @param     None
 * @return    (struct RTC_TmStruct*)ptime: current time struct pointer.
 */
struct tm* RTC_GetTime(void)
{
    struct tm *ptime;
    RTC_GetTimeStamp();
    ptime = localtime(&RtcUnixTimestamp);
    ptime->tm_year += 1900;
    ptime->tm_mon += 1;
    return ptime;
}

/**
 * @name      RTC_ClearIrq
 * @brief     clear RTC interrupt status bits.
 * @param     type: 0:ALARM0
 *                  1:ALARM1
 *                  2:OV_INTR.
 * @return    None
 */
void RTC_ClearIrq(RTC_CompEnum type)
{
    if(type==ALARM0)
    {
        LPM_RTC->rtc_ov_en.bit.rtc_comp0_en = 0;
        SYSCTRL_WaitLpoEdge(3);
        LPM_RTC->rtc_ov_en.bit.rtc_comp0_en = 1;
        SYSCTRL_WaitLpoEdge(3);
    }
    else if (type==ALARM1)
    {
        LPM_RTC->rtc_ov_en.bit.rtc_comp1_en = 0;
        SYSCTRL_WaitLpoEdge(3);
        LPM_RTC->rtc_ov_en.bit.rtc_comp1_en = 1;
        SYSCTRL_WaitLpoEdge(3);
    }
    else
    {
        LPM_RTC->rtc_ov_en.bit.rtc_ov_en = 0;
        SYSCTRL_WaitLpoEdge(3);
        LPM_RTC->rtc_ov_en.bit.rtc_ov_en = 1;
        SYSCTRL_WaitLpoEdge(3);
    }
}
/**
 * @name    RTC_GetTimeStamp
 * @brief   get timestamp
 * @param   none
 * @retval  timestamp
 */
time_t RTC_GetTimeStamp(void)
{
    uint32_t RtcCurrentCnt=RTC_GetCount(OV_INTR);
    uint32_t TempRtcUnixTimestamp;
    if(RtcCurrentCnt<RtcLastCnt)
    {
        TempRtcUnixTimestamp=((0xffffffff-RtcLastCnt)+1+RtcCurrentCnt)/RtcClk;
    }
    else
    {
        TempRtcUnixTimestamp=(RtcCurrentCnt-RtcLastCnt)/RtcClk;
    }
    if(TempRtcUnixTimestamp)
    {
        RtcUnixTimestamp += TempRtcUnixTimestamp;
        RtcLastCnt = ((TempRtcUnixTimestamp*RtcClk)+RtcLastCnt);
    }
    return RtcUnixTimestamp;
}


/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

