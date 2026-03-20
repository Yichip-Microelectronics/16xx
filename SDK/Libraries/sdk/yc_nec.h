/*
File Name    : yc_nec.h
Author       : Yichip
Version      : V1.0
Date         : 2022/12/13
Description  : nec driver interface define.
*/

#ifndef __YC_NEC_H__
#define __YC_NEC_H__

#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief nec irq(status) flag define
 *
 */
#define NEC_IRQ_DONE        (1<<0)
#define NEC_IRQ_HALF_DONE   (1<<1)


/**
 * @brief nec Receive signal source enum
 *
 */
typedef enum{
    NEC_FROM_GPIO,
    NEC_FROM_IR
} NEC_RxSrcEnum;

/**
 * @brief nec gain level enum
 *
 */
typedef enum
{
    GAIN_LOWEST,
    GAIN_LOW,
    GAIN_HIGH,
    GAIN_HIGHEST
} NEC_GainLevelEnum;

/**
 * @brief nec mode select enum
 *
 */
typedef enum
{
    NEC_TX_CARRIER_NONE,                /*!< nec tx,no carrier wave. */
    NEC_TX_CARRIER_HIGH,                /*!< nec tx,carrier wave on high level. */
    NEC_TX_CARRIER_LOW,                 /*!< nec tx,carrier wave on low level. */
    NEC_RX                              /*!< nec rx. */
} NEC_ModeEnum;


/**
 * @brief nec init struct
 *
 */
typedef struct
{
    uint32_t            bufSaddr;       /*!< nec receive dma start address. */
    uint32_t            bufSize;        /*!< nec receive dma buf size. */
    uint32_t            clk;         /*!< nec module clk. */
    NEC_ModeEnum        mode;
} NEC_InitStruct;

void NEC_Init(NEC_InitStruct *initStruct);
void NEC_DeInit(void);
void NEC_SetClk(uint32_t clk);
void NEC_SetMode(NEC_ModeEnum mode);
void NEC_RxSrcSel(NEC_RxSrcEnum necSource);
void NEC_SetRxGainLevel(NEC_GainLevelEnum gainLevel);
void NEC_PtrInit(void);
void NEC_Cmd(FunctionalState state);
void NEC_SetIrq(uint8_t irqType,FunctionalState state);
void NEC_ClearIrq(uint8_t irqType);
uint8_t NEC_GeIrqStatus(void);
int NEC_WriteData(uint16_t *buf,int len);
uint16_t NEC_ReadData(void);
uint32_t NEC_GetDataCount(void);

#ifdef __cplusplus
}
#endif

#endif /* __YC_NEC_H__ */
