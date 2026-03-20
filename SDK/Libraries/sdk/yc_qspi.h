/*
File Name    : yc_qspi.h
Author       : Yichip
Version      : V1.0
Date         : 2024/5/7
Description  : qspi driver interface define.
*/


/* Includes ------------------------------------------------------------------*/

#ifndef __yc_QSPI_H__
#define __yc_QSPI_H__

#include "yc_sysctrl.h"
#include "register.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    QSPI_STANDARD_MODE = 0,
    QSPI_DUAL_MODE,
    QSPI_QSPI_MODE
} QSPI_ModeEnum;

void QSPI_SetMode(CORE_QSPI1_Type* qspix, QSPI_ModeEnum cmdMode, QSPI_ModeEnum addrMode, QSPI_ModeEnum dataMode);
void QSPI_ClkSel(CORE_QSPI1_Type* qspix, DPLL_ClkEnum qspiDpllClkSel);
void QSPI_ClkCmd(CORE_QSPI1_Type* qspix, FunctionalState state);
void QSPI_SetIrq(CORE_QSPI1_Type* qspix, FunctionalState state);
void QSPI_ClearIrq(CORE_QSPI1_Type* qspix);
void QSPI_DmaStart(CORE_QSPI1_Type* qspix, FunctionalState state);
void QSPI_SetDummy(CORE_QSPI1_Type* qspix, uint8_t dly);
uint8_t QSPI_WaitDone(CORE_QSPI1_Type* qspix);
void QSPI_9bitModeEn(CORE_QSPI1_Type* qspix, FunctionalState state);
uint8_t Qspi_SendAndReceiveData(CORE_QSPI1_Type* qspix, uint8_t *cmdbuf, uint8_t cmdlen, uint8_t *txbuf, uint32_t txlen, uint8_t *rxbuf, uint32_t rxlen);

#ifdef __cplusplus
}
#endif
#endif

/******************************************************************END OF FILE****/
