/*
File Name    : yc_rtc.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/26
Description  : rtc driver interface define.
*/

#ifndef __YC_RTC_H_
#define __YC_RTC_H_

#include "board_config.h"
#include "time.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ALARM0 = 0,
    ALARM1,
    OV_INTR
} RTC_CompEnum;

typedef enum
{
    RC_32K = 0,
    XO_32K
} RTC_ClkSelEnum;

void RTC_Cmd(FunctionalState state);
void RTC_DeInit(void);
void RTC_Calibration(void);
uint32_t RTC_GetClk(void);
void RTC_ClkSel(RTC_ClkSelEnum clkSel);
void RTC_SetIrq(RTC_CompEnum type, FunctionalState state);
void RTC_WKConfig(RTC_CompEnum type, FunctionalState state);
void RTC_SetAlarm0(uint32_t alarmValue);
void RTC_SetAlarm1(uint32_t alarmValue);
uint32_t RTC_GetCount(RTC_CompEnum type);
uint8_t RTC_GetIrqStauts(RTC_CompEnum type);
void RTC_ClearIrq(RTC_CompEnum type);
void RTC_SetTime(struct tm dateStruct);
struct tm* RTC_GetTime(void);
time_t RTC_GetTimeStamp(void);

#ifdef __cplusplus
}
#endif
#endif  /* __YC_RTC_H_ */

