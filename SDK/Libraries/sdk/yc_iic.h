/*
File Name    : yc_iic.h
Author       : Yichip
Version      : V1.0
Date         : 2024/5/6
Description  : iic driver interface define.
*/


/* Includes ------------------------------------------------------------------*/

#ifndef _yc_IIC_H_
#define _yc_IIC_H_
#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
  * @brief  IIC channel define
  */

typedef struct{
    uint8_t scll;    /*!< Specifies the Clock Pulse Width Low. */
    uint8_t sclh;    /*!< Specifies the Clock Pulse Width High. */
    uint8_t stsu;    /*!< Specifies the Start Setup Time. */
    uint8_t sthd;    /*!< Specifies the Start Hold Time. */
    uint8_t sosu;    /*!< Specifies the Stop Setup Time. */
    uint8_t dtsu;    /*!< Specifies the Data Setup Time. */
    uint8_t dthd;    /*!< Specifies the Data Hold Time. */
} IIC_InitStruct;

void IIC_DeInit(CORE_IIC_Type* iicx);
void IIC_Init(CORE_IIC_Type* iicx, IIC_InitStruct *iicInitStruct);
void IIC_SendData(CORE_IIC_Type* iicx, uint8_t *src, uint16_t len);
void IIC_ReceiveData(CORE_IIC_Type* iicx, uint8_t *src, uint16_t srcLen, uint8_t *dest, uint16_t destLen);
void IIC_SetIrq(CORE_IIC_Type* iicx, FunctionalState newState);
void IIC_ClearIrq(CORE_IIC_Type* iicx);
#ifdef __cplusplus
}
#endif
#endif






