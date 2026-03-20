/*
File Name    : yc_plic.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/24
Description  : plic irq driver interface define.
*/

#ifndef __YC_PLIC_H__
#define __YC_PLIC_H__

#include "board_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
/* ---------------------  RV PLIC Interrupt ID  --------------------                                                           */
    RSVD_IRQn                   =0,/*Reserved*/
    BT_IRQn                     =1,
    SOFT_IRQn                   =2,
    BTTIMER_IRQn                =3,
    UARTA_IRQn                  =4,
    USB_IRQn                    =5,
    AES_IRQn                    =6,
    IIC0_IRQn                   =7,
    IIC1_IRQn                   =8,
    HSPI_IRQn                   =9,
    MIC_IRQn                    =10,
    VIO_IRQn                    =11,
    GPIO_IRQn                   =12,
    SDMA_IRQn                   =13,
    HWAKE_IRQn                  =14,
    WDT_IRQn                    =15,
    TIMERGR0_IRQn               =16,/*TIM0-TIM3*/
    TIMERGR1_IRQn               =17,/*TIM4-TIM7*/
    CHGR_IRQn                   =18,
    GDAC_IRQn                   =19,
    SARADC_IRQn                 =20,
    UARTC_IRQn                  =22,
    UARTD_IRQn                  =23,
    UARTB_IRQn                  =24,
    NEC_IRQn                    =25,
    QSPI_IRQn                   =26,
    QSPI1_IRQn                  =27,
    QSPI2_IRQn                  =28,
    RTC_IRQn                    =29,
    CAN_IRQn                    =30,
} IRQn_Enum;

#define IS_IRQN(IRQN)         ((((int)IRQN) >= 0) && (((int)IRQN) < 32))
#define IS_PRIORITY(priority) (((priority) >= 0) && ((priority) <= 3))

void PLIC_EnableIRQ(IRQn_Enum IRQn);
void PLIC_DisableIRQ(IRQn_Enum IRQn);
void PLIC_SetPriority(IRQn_Enum IRQn, uint32_t priority);

#ifdef __cplusplus
}
#endif

#endif
