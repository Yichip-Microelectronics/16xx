
/*
File Name    : yc_lpm.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : lpm driver interface define.
*/

#ifndef _YC_LPM_H_
#define _YC_LPM_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
  * @brief  lpm gpio latch type enum
  */
typedef enum
{
    LATCH_NONE = 0,
    LATCH_PD,
    LATCH_PU,
    LATCH_OUTPUT_LOW,
    LATCH_OUTPUT_HIGH
}LPM_LatchEnum;

/**
 * @brief lpm mode enum
 *
 */
typedef enum
{
    SLEEP_MODE_NONE = 0,
    SLEEP_MODE_LPM,
    SLEEP_MODE_DEEP
} LPM_ModeEnum;

/**
 * @brief lpm wakeup mode enum
 *
 */
typedef enum
{
    WAKEUP_FROM_MAIN = 0,
    WAKEUP_FROM_SLEEP
}LPM_WakeupModeEnum;

typedef void (*lpmPreActionCb)(LPM_ModeEnum sleepLevel);
typedef void (*lpmPostActionCb)(LPM_ModeEnum sleepLevel);

void LPM_Init(void);
void LPM_SetGpioLatch(GPIO_NumEnum gpiox,LPM_LatchEnum type);
bool LPM_Sleep(uint32_t wakeTimeMs, LPM_ModeEnum sleepLevel,uint8_t retention_cfg,LPM_WakeupModeEnum wakeMode,lpmPreActionCb lpmPreCallback,lpmPostActionCb lpmPostCallback);

#ifdef __cplusplus
}
#endif

#endif /* _YC_LPM_H_ */




