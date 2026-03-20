/*
File Name    : yc_timer.h
Author       : Yichip
Version      : V1.0
Date         : 2023/12/12
Description  : timer driver interface define.
*/

#ifndef _YC_TIMER_H_
#define _YC_TIMER_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C"
{
#endif



#define TIMER_CLK                (TIM_Getclk())
#define TIMER_1MS(CLK_DIV)       (TIMER_CLK / (1 << CLK_DIV) / 1000)
#define TIMER_1US(CLK_DIV)       (TIMER_CLK / (1 << CLK_DIV) / 1000000)

/**
 * @brief  timer Numble enable Structure definition
 */
typedef enum
{
    TIM0 = 0,
    TIM1,
    TIM2,
    TIM3,
    TIM4,
    TIM5,
    TIM6,
    TIM7
} TIM_PortEnum;
#define IS_TIMER_NUM(TIMx)   (TIMx <= TIM7)
#define IS_TIMER_CLKDIV(DIV) (DIV >= 0 && DIV <= 7)
#define IS_PERIOD_VALUE(X)   (X <= 0xffff)

/**
 * @brief  timer Init Structure definition
 */
typedef struct
{
    TIM_PortEnum timx;
    uint16_t     period;
    uint8_t      clockdiv;
    uint8_t      autoLoadEn;
} TIM_InitStruct;


/**
 * @brief  pwm Init Structure definition
 */
typedef struct
{
    TIM_PortEnum   timx;
    uint16_t       lowlevelperiod;
    uint16_t       highlevelperiod;
    uint8_t        clockdiv;
} PWM_InitStruct;
typedef struct
{
    TIM_PortEnum timx;              /*first TIMx of sync pwm group(TIM0、TIM2、TIM4、TIM6)*/
    uint16_t lowLevelPeriod;
    uint16_t highLevelPeriod;
    uint16_t leftBorderPeriod;
    uint16_t rightBorderPeriod;
    uint8_t startLevel;             /*0:even pwm number output hight first;1:odd pwm number output hight first*/
    uint8_t clockDiv;
    uint8_t syncOutputInverse;
} PWM_SyncInitStruct;

uint32_t TIM_Getclk(void);
void TIM_DeInit(TIM_PortEnum timx);
void TIM_Init(TIM_InitStruct *initStruct);
void TIM_Cmd(TIM_PortEnum timx, FunctionalState newState);
void TIM_SetTimerPeriod(TIM_PortEnum timx, uint16_t period);
void TIM_ITConfig(TIM_PortEnum timx, FunctionalState newState);
void TIM_PwmInit(PWM_InitStruct *initStruct);
void TIM_SetPwmPeriod(TIM_PortEnum timx, uint16_t lowLevelPeriod, uint16_t highLevelPeriod);
void TIM_PwmSyncInit(PWM_SyncInitStruct* syncInitStruct);
void TIM_PwmSyncCmd(TIM_PortEnum timx, FunctionalState newState);
void TIM_SetPwmSyncPeriod(TIM_PortEnum timx, uint16_t leftBorderPeriod, uint16_t highLevelPeriod, uint16_t rightBorderPeriod, uint16_t lowLevelPeriod);
void TIM_ClearIrqStatus(TIM_PortEnum timx);
ITStatus TIM_GetITStatus(TIM_PortEnum timx);
uint8_t TIM_GetAllITStatus(void);


#ifdef __cplusplus
}
#endif

#endif /* _YC_TIMER_H_ */

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
