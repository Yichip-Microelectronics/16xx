/*
 * Copyright (c) 2006-2020, YICHIP Development Team
 * @file     yc_rand.h
 * @brief    This file contains all the functions prototypes for the efuse firmware library.
 *
 * Change Logs:
 * Date           Author             Version        Notes
 * 2023-06-08     jiangheng          V1.0.0         the first version
 */

#ifndef __yc_RAND_H__
#define __yc_RAND_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "board_config.h"

void YC_RandInit(uint32_t seed);
uint16_t YC_RandGet(void);
#ifdef __cplusplus
}
#endif
#endif // !__yc_RAND_H__

/******************************************************************END OF FILE****/
