/*
File Name    : yc_sadc.h
Author       : Yichip
Version      : V1.0
Date         : 2024/05/30
Description  : sadc driver interface define.
*/

#ifndef __YC_SADC_H__
#define __YC_SADC_H__

#include "register.h"
#include "yc_sysctrl.h"
#ifdef __cplusplus
extern "C"
{
#endif
 #define ADC_REF_OTP_ADDR           0x790
 #define SADC_FILT_NUM              20
 #define SADC_REMOVE_NUM            6
 #define SADC_GPIO_RANGE_3V3        0
 #define SADC_GPIO_RANGE_1V8        1
 #define SADC_GPIO_RANGE            SADC_GPIO_RANGE_1V8

 #define SADC_CLK_1M                (SYSCTRL_GetSystemClk()/1000000-1)
 #define SADC_CLK_24M               (SYSCTRL_GetSystemClk()/24000000-1)

/* SADC Channel enum */
typedef enum
{
    SADC_CHANNEL_VBAT,
	SADC_CHANNEL_VDCDC,
    SADC_CHANNEL_GPIO4=6,
    SADC_CHANNEL_GPIO5,
    SADC_CHANNEL_GPIO9,
    SADC_CHANNEL_GPIO20,
    SADC_CHANNEL_GPIO21,
    SADC_CHANNEL_GPIO22,
    SADC_CHANNEL_GPIO23,
    SADC_CHANNEL_GPIO24,
    SADC_CHANNEL_GPIO25,
    SADC_CHANNEL_GPIO30
}SADC_ChxEnum;

/* SADC Status enum */
typedef enum
{
    SADC_EORC       =BIT0,
    SADC_EOIC       =BIT1,
    SADC_WDT        =BIT2,
    SADC_HALF       =BIT3,
    SADC_IHALF      =BIT4,
    SADC_OVER       =BIT5,
    SADC_IOVER      =BIT6,
    SADC_EOC        =BIT7,
    SADC_ALL_EVENT  =0xff
} SADC_IrqEnum;

#define IS_SADC_IRQ(IRQ)   ((IRQ == SADC_EORC) || \
                            (IRQ == SADC_EOIC) || \
                            (IRQ == SADC_WDT)  || \
                            (IRQ == SADC_HALF) || \
                            (IRQ == SADC_IHALF)|| \
                            (IRQ == SADC_OVER) || \
                            (IRQ == SADC_IOVER)|| \
                            (IRQ == SADC_EOC)  || \
                            (IRQ == SADC_ALL_EVENT))

/* SADC trigger source enum */
typedef enum
{
    SADC_TIMER0 = 0,
    SADC_TIMER1,
    SADC_TIMER2,
    SADC_TIMER3,
    SADC_TIMER4,
    SADC_TIMER5,
    SADC_TIMER6,
    SADC_TIMER7,
    SADC_SOFTWARE = 0xf
} SADC_EtsEnum;
#define IS_SADC_ETS(TIMER) ((TIMER == SADC_SOFTWARE)|| \
                            (TIMER == SADC_TIMER0)  || \
                            (TIMER == SADC_TIMER1)  || \
                            (TIMER == SADC_TIMER2)  || \
                            (TIMER == SADC_TIMER3)  || \
                            (TIMER == SADC_TIMER4)  || \
                            (TIMER == SADC_TIMER5)  || \
                            (TIMER == SADC_TIMER6)  || \
                            (TIMER == SADC_TIMER7))


/* ADC Struct Define*/
typedef struct
{
    FunctionalState continuous_mode;    /*连续采样模式*/
    FunctionalState auto_inject;        /*自动注入使能*/
    SADC_EtsEnum etsrc;                 /*规则组采样触发源*/
    SADC_EtsEnum etsic;                 /*注入组采样触发源*/
} SADC_InitStruct;

typedef struct
{
    FunctionalState wdt_ren;            /*规则组看门狗使能*/
    FunctionalState wdt_ien;            /*注入组看门狗使能*/
    FunctionalState wdt_sc;             /*看门狗通道选择使能,开启此配置wdt_ch配置才有效*/
    SADC_ChxEnum    wdt_ch;             /*看门狗通道选择*/
}
SADC_WdtStruct;

void SADC_DeInit(void);
void SADC_Init(SADC_InitStruct *initStruct);
void SADC_InputDiv2(FunctionalState state);
void SADC_SetRegularChannel(uint16_t rlist);
void SADC_SetInjectedChannel(uint16_t ilist, uint8_t num);

void SADC_RegularTrigsel(SADC_EtsEnum ets);
void SADC_InjectTrigsel( SADC_EtsEnum ets);

void SADC_RegularTrigger(void);
void SADC_InjectTrigger(void);

uint16_t SADC_GetRegularConversionValue(void);
uint16_t SADC_GetInjectedConversionValue(void);

void SADC_SetRegualrSampleMode(FunctionalState state);
void SADC_SetInjectSampleMode(FunctionalState state);

void SADC_DmaCmd(FunctionalState state);
void SADC_DmaConfig(uint16_t *rsaddr, uint16_t *readdr, uint16_t *ieaddr);
void SADC_WdtConfig(SADC_WdtStruct *wdt,uint16_t lowLimit,uint16_t highLimit);
uint8_t SADC_GetStatus(void);
void SADC_SetIrq(SADC_IrqEnum irqn,FunctionalState state);
uint8_t SADC_GetIrqStatus(void);
void SADC_ClearIrq(SADC_IrqEnum irqn);
uint16_t SADC_CalculateVoltage(SADC_ChxEnum ch,uint16_t adcValue);

#ifdef __cplusplus
}
#endif

#endif /*__YC_SADC_H__*/

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
