/*
File Name    : yc_vector.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/24
Description  : vector irq driver interface define.
*/

#ifndef __YC_VECTOR_H__
#define __YC_VECTOR_H__

#include "board_config.h"
#include "riscv_encoding.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
/* ---------------------  RV VECTOR Interrupt ID  --------------------                                                           */
    VECTOR_SOFT_IRQn              =3,
    VECTOR_SYSTICK_IRQn           =7,
    VECTOR_RTC_IRQn               =16,
    VECTOR_QSPI2_IRQn             =17,
    VECTOR_QSPI1_IRQn             =18,
    VECTOR_QSPI_IRQn              =19,
    VECTOR_UARTA_IRQn             =20,
    VECTOR_SADC_IRQn              =21,
    VECTOR_GDAC_IRQn              =22,
    VECTOR_TIMERGR0_IRQn          =23,/*TIM0-TIM3*/
    VECTOR_WDT_IRQn               =24,
    VECTOR_SDMA_IRQn              =25,
    VECTOR_GPIO_IRQn              =26,
    VECTOR_MIC_IRQn               =27,
    VECTOR_AES_IRQn               =28,
    VECTOR_USB_IRQn               =29,
    VECTOR_BTTIMER_IRQn           =30,
    VECTOR_BT_IRQn                =31,
} VECTOR_IRQn_Enum;

#define IS_VECTOR_IRQN(IRQN)         ((((int)IRQN) >= 16) && (((int)IRQN) < 32)) || (IRQN == 7)|| (IRQN == 3)

void VECTOR_EnableIRQ(VECTOR_IRQn_Enum IRQn);
void VECTOR_DisableIRQ(VECTOR_IRQn_Enum IRQn);

#ifdef __cplusplus
}
#endif

#endif
