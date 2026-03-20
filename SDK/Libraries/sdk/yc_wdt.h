/*
File Name    : yc_wdt.h
Author       : Yichip
Version      : V1.0
Date         : 2024/4/28
Description  : wdt driver interface define.
*/


/* Includes ------------------------------------------------------------------*/

#ifndef __YC_WDT_H_
#define __YC_WDT_H_

#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
  * @brief  watch dog mode definition
  */
typedef enum
{
    RESET_MODE = 0,
    INTR_MODE
}WDT_ModeEnum;

/**
  * @brief  watch dog Init Structure definition
  */
typedef struct
{
    WDT_ModeEnum mode;
    uint8_t      reload;
} WDT_InitStruct;


#define WDT_TIMER_125MS             0x7F
#define WDT_TIMER_250MS             0x7e
#define WDT_TIMER_500MS             0x7c
#define WDT_TIMER_1_5S              0x74
#define WDT_TIMER_10S               0x30
#define WDT_TIMER_MAX               0x00 //16.384s


void WDT_Init(WDT_InitStruct* initStruct);
void WDT_SetReload(uint8_t count);
void WDT_Enable(void);
void WDT_Disable(void);
void WDT_ClearIrqSatus(void);
void WDT_WfiHoldEn(FunctionalState newState);

#ifdef __cplusplus
}
#endif

#endif /* __YC_WDT_H_ */

