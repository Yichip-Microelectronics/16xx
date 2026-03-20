/*
File Name    : yc_iic.c
Author       : Yichip
Version      : V1.0
Date         : 2024/5/6
Description  : iic driver interface implement.
*/


/* Includes ------------------------------------------------------------------*/

#include "yc_iic.h"
#include "yc_gpio.h"


/**
 * @brief    Initializes the IIC_0/IIC_1 according to the specified parameters in the iicInitStruct.
 * @param    iicx: Select the IIC peripheral.
 *                 CORE_IIC
 *                 CORE_IIC1
 * @param    iicInitStruct : pointer to a iicInitStruct structure that contains
 *                             the configuration information for the IIC.
 * @return   None
 */
void IIC_Init(CORE_IIC_Type* iicx,IIC_InitStruct* iicInitStruct)
{
    iicx->iic_ctrl.reg = 0;

    iicx->iic_ctrl.bit.iic_restart = 1;//
    iicx->iic_ctrl.bit.iic_ram_select = 1;//

    /*set iic clock speed*/
    iicx->iic_delay_low_clk_cnt.reg         = iicInitStruct->scll;
    iicx->iic_delay_high_clk_cnt.reg        = iicInitStruct->sclh;
    iicx->iic_delay_setup_clk_cnt_start.reg = iicInitStruct->stsu;
    iicx->iic_delay_hold_clk_cnt_start.reg  = iicInitStruct->sthd;
    iicx->iic_delay_setup_clk_cnt_stop.reg  = iicInitStruct->sosu;
    iicx->iic_delay_setup_clk_cnt_data.reg  = iicInitStruct->dtsu;
    iicx->iic_delay_hold_clk_cnt_data.reg   = iicInitStruct->dthd;
}

/**
 * @brief    Deinitializes the IIC registers to their default reset values.
 * @param    iicx: Select the IIC peripheral.
 *                 CORE_IIC
 *                 CORE_IIC1
 * @return   None
 */
void IIC_DeInit(CORE_IIC_Type* iicx)
{
    iicx->iic_ctrl.reg = 0;
}

/**
 * @brief     IIC send buffer data
 * @param     iicx: Select the IIC peripheral.
 *                  CORE_IIC
 *                  CORE_IIC1
 * @param     src :send buffer addr
 * @param     len : send data length
 * @return    None
 */
void IIC_SendData(CORE_IIC_Type* iicx,uint8_t *src, uint16_t len)
{
    iicx->iic_txaddr.reg = (uint32_t)src;
    iicx->iic_txlen.reg = len;

    iicx->iic_rxlen.reg= 0;
    iicx->iic_rxaddr.reg = 0;

    iicx->iic_start.reg = 1;
    while(!(iicx->iic_status.bit.iic_status_dma_done));
}

/**
 * @brief       IIC receive data
 * @param       iicx: Select the IIC peripheral.
 *                    CORE_IIC
 *                    CORE_IIC1
 * @param       src : send data buffer
 * @param       srcLen : send data length
 * @param       dest : read buffer addr
 * @param       destLen :read data length
 * @return      None
 */
void IIC_ReceiveData(CORE_IIC_Type* iicx,uint8_t *src, uint16_t srcLen, uint8_t *dest, uint16_t destLen)
{
    iicx->iic_txaddr.reg = (uint32_t)src;
    iicx->iic_txlen.reg = srcLen;

    iicx->iic_rxlen.reg= destLen;
    iicx->iic_rxaddr.reg = (uint32_t)dest;

    iicx->iic_start.reg = 1;
    while(!(iicx->iic_status.bit.iic_status_dma_done));
}

/**
 * @brief  Disable or enable IIC interrupt
 * @param  iicx: Select the IIC peripheral.
 *               CORE_IIC
 *               CORE_IIC1
 * @return  None
 */
void IIC_SetIrq(CORE_IIC_Type* iicx, FunctionalState newState)
{
    iicx->iic_ctrl.bit.iic_intr_en = newState;
}

/**
 * @brief  Clear Interrupt trigger status
 * @param  iicx: Select the IIC peripheral.
 *               CORE_IIC
 *               CORE_IIC1
 * @return  None
 */
void IIC_ClearIrq(CORE_IIC_Type* iicx)
{
    iicx->iic_ctrl.bit.iic_intr_clr = 1;//111change to bit
}