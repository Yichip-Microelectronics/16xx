/*
File Name    : yc_systick.h
Author       : Yichip
Version      : V1.0
Date         : 2023/12/01
Description  : systick driver interface define.
*/

#ifndef _yc_SYSTICK_H_
#define _yc_SYSTICK_H_

/* Includes ------------------------------------------------------------------*/
#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYSTEM_MHZ      (SYSCTRL_GetSystemClk())
#define SYSTICK_MHZ     (SYSTEM_MHZ / (CORE_CHIP_SYS->systick_div1m.reg + 1))
#define SYSTICK_ONE_MS  (SYSTICK_MHZ / 1000)
#define SYSTICK_ONE_US  (SYSTICK_MHZ / 1000000)

extern uint32_t SystickCount;
void SYSTICK_BtTimerCmd(FunctionalState state);
void SYSTICK_Disable(void);
void SYSTICK_Config(uint32_t reloadValue);
void SYSTICK_Backup(void);
void SYSTICK_Recover(void);
void SYSTICK_SetClk1M(void);
void SYSTICK_ClearIrq(void);
uint32_t SYSTICK_GetReloadValue(void);
uint32_t SYSTICK_GetRelativeTime(uint32_t startTick);
uint32_t SYSTICK_GetTick(void);
bool SYSTICK_IsTimeOut(uint32_t startTick, uint32_t interVal);
#ifdef __cplusplus
}
#endif

#endif /* _yc_SYSTICK_H_ */

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
