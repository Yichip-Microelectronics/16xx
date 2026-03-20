/*
File Name    : yc_qspi.c
Author       : Yichip
Version      : V1.0
Date         : 2024/5/7
Description  : qspi driver interface implement.
*/


/* Includes ------------------------------------------------------------------*/

#include "yc_qspi.h"
#include "yc_log.h"

#define QSPI_WAITTIMEOUT        (0x100000)

/**
 * @brief  qspi set mode function
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  cmdMode: qspi cmd mode from QSPI_ModeEnum
 * @param  addrMode: qspi addr mode from QSPI_ModeEnum
 * @param  dataMode: qspi data mode from QSPI_ModeEnum
 * @return None
 */
void QSPI_SetMode(CORE_QSPI1_Type* qspix, QSPI_ModeEnum cmdMode, QSPI_ModeEnum addrMode, QSPI_ModeEnum dataMode)
{
        qspix->qspi1_ctrl.bit.mode_cmd  = cmdMode;
        qspix->qspi1_ctrl.bit.mode_addr = addrMode;
        qspix->qspi1_ctrl.bit.mode_data = dataMode;

}

/**
 * @brief  Qspi dpll clock select(only active when sys_clk_source==SYS_CLKSEL_DPLL)
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  qspiDpllClkSel : qspi clock frequency from DPLL_ClkEnum(DPLL_CLKSEL_24MHZ、DPLL_CLKSEL_48MHZ、DPLL_CLKSEL_64MHZ or DPLL_CLKSEL_96MHZ)
 * @return None
 */
void QSPI_ClkSel(CORE_QSPI1_Type* qspix, DPLL_ClkEnum qspiDpllClkSel)
{
    if(qspix==CORE_QSPI1)
    {
        CORE_CHIP_SYS->qspi_clk_sel.bit.qspi1_clk_sel = qspiDpllClkSel;
    }
    else
    {
        CORE_CHIP_SYS->qspi_clk_sel.bit.qspi2_clk_sel = qspiDpllClkSel;
    }
}

/**
 * @brief  Qspi Enable/Disable
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  state: ENABLE or DISABLE
 * @return None
 */
void QSPI_ClkCmd(CORE_QSPI1_Type* qspix, FunctionalState state)
{
    qspix->qspi1_ctrl.bit.qspi_clk_en = state;
}


/**
 * @brief  Qspi start transmit data
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  state: DISABLE or ENABLE
 * @return None
 */
void QSPI_DmaStart(CORE_QSPI1_Type* qspix, FunctionalState state)
{

    qspix->qspi1_start.reg = state;

}

/**
 * @brief  set dummy(the interval between the address and the data)
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  dly: dummy number
 * @return None
 */
void QSPI_SetDummy(CORE_QSPI1_Type* qspix,uint8_t dly)
{

    qspix->qspi1_ctrl.bit.dummy_bits_cnt = dly;

}

/**
 * @brief  Wait for read and write to complete
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @return 1:send and receive success 0:send or receive timeout
 */
uint8_t QSPI_WaitDone(CORE_QSPI1_Type* qspix)
{
    int timeout = 0;

    while (1)
    {
        timeout++;

        if(qspix->qspi1_status.reg & 0x01)
        {
            return 1;
        }
        else if(timeout > QSPI_WAITTIMEOUT)
        {
            return 0;
        }
    }

}
/**
 * @brief  qspi 9bit mode Enable/Disable
 * @param  state: ENABLE or DISABLE
 * @return None
 */
void QSPI_9bitModeEn(CORE_QSPI1_Type* qspix, FunctionalState state)
{
    qspix->qspi1_ctrl.bit.dc_mode = state;
}
/**
 * @brief qspi send and receive data function
 * @param qspix: Select the QSPI peripheral.
 *               CORE_QSPI1
 *               CORE_QSPI2
 * @param cmdbuf instruction code
 * @param cmdlen instruction code length
 * @param txbuf send data buff
 * @param txlen send data length
 * @param rxbuf receive data buff
 * @param rxlen receive data length
 * @return 1:send and receive success 0:send or receive timeout
 */
uint8_t Qspi_SendAndReceiveData(CORE_QSPI1_Type* qspix, uint8_t *cmdbuf, uint8_t cmdlen, uint8_t *txbuf, uint32_t txlen, uint8_t *rxbuf, uint32_t rxlen)
{
    qspix->qspi1_cmdlen.bit.qspi1_cmdaddr = (uint32_t)cmdbuf;
    qspix->qspi1_cmdlen.bit.qspi1_cmdlen = cmdlen;
    qspix->qspi1_txaddr.reg = (uint32_t)txbuf;
    qspix->qspi1_txlen.reg  = txlen;
    qspix->qspi1_rxaddr.reg = (uint32_t)rxbuf;
    qspix->qspi1_rxlen.reg  = rxlen;

    QSPI_DmaStart(qspix, ENABLE);
    return QSPI_WaitDone(qspix);
}

/**
 * @brief  Qspi config interrupt function
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @param  state DISABLE or ENABLE
 * @return None
 */
void QSPI_SetIrq(CORE_QSPI1_Type* qspix, FunctionalState state)
{
    qspix->qspi1_ctrl.bit.intr_en = state;
}

/**
 * @brief  Qspi clear interrupt flag function
 * @param  qspix: Select the QSPI peripheral.
 *                CORE_QSPI1
 *                CORE_QSPI2
 * @return None
 */
void QSPI_ClearIrq(CORE_QSPI1_Type* qspix)
{
    qspix->qspi1_ctrl.bit.intr_clr = ENABLE;
    qspix->qspi1_ctrl.bit.intr_clr = DISABLE;
}
