/*
 * Copyright (c) 2006-2020, YICHIP Development Team
 * @file     yc_rand.c
 * @brief    This file contains all the functions prototypes for the efuse firmware library.
 *
 * Change Logs:
 * Date           Author             Version        Notes
 * 2023-06-08     jiangheng          V1.0.0         the first version
 */

/* Includes ------------------------------------------------------------------*/

#include "yc_rand.h"

/**
 * @brief    Init rand.
 * @param    seed: configure rand seed
 * @return   None
 */
void YC_RandInit(uint32_t seed)
{
    CORE_CHIP_SYS->clken.bit.bt_dma = 1;
    delay(seed);
}

/**
 * @brief    get rand number.
 * @param    none
 * @return   rand number
 */
uint16_t YC_RandGet(void)
{
    return (uint16_t)CORE_BTDMA->rand_num.reg;
}


/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
