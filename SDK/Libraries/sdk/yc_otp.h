/*
File Name    : yc_otp.h
Author       : Yichip
Version      : V1.0
Date         : 2022/12/13
Description  : otp interface define.
*/

#ifndef __YC_OTP_H__
#define __YC_OTP_H__

#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif


void OTP_Init(void);
void OTP_Deinit(void);
uint8_t OTP_Read(uint32_t addr,uint8_t *data,uint32_t len);
uint8_t OTP_Write(uint32_t addr, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
