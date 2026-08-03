/*
File Name    : yc_hspi.c
Author       : Yichip
Version      : V1.0
Date         : 2024/4/28
Description  : hspi driver interface implement.
*/


/* Includes ------------------------------------------------------------------*/

#include "yc_hspi.h"
#include "yc_sysctrl.h"

/**
 * @brief Initializes the SPI according to the specified parameters in the SPI_InitStruct.
 *
 * @param initStruct pointer to a SPI_InitTypeDef structure that contains
 *                       the configuration information for the SPI.
 * @return None
 */
void HSPI_Init(HSPI_InitStruct* initStruct)
{
    SYSCTRL_ClkEn(CLK_SPI,ENABLE);
    CORE_SPI->spi_ctrl.bit.clk_div = initStruct->clkDiv;
    CORE_SPI->spi_ctrl.bit.cpha = initStruct->cpha;
    CORE_SPI->spi_ctrl.bit.cpol = initStruct->cpol;
    CORE_SPI->spi_ctrl.bit.fifo_sw_en = initStruct->fifoSwEn;
    CORE_SPI->spi_ctrl.bit.mram_sel = 1;
    CORE_SPI->spi_ctrl.bit.ncs_dly = initStruct->ncsDly;
    CORE_SPI->spi_ctrl.bit.rx_adj_clk = initStruct->rxAdjClk;
    CORE_SPI->spi_ctrl.bit.rx_adj_en = initStruct->rxAdjen;
    CORE_SPI->spi_ctrl.bit.trx_dly = initStruct->txRxDly;
    CORE_SPI->spi_ctrl.bit.tx_rx_seq = initStruct->txRxSeq;
    CORE_SPI->spi_ctrl.bit.slave_mode = initStruct -> slaveMode;
    CORE_SPI->spi_ctrl.bit.bidi_mode = initStruct->halfDuplexEn;
    CORE_SPI->spi_ctrl.bit.bidi_oe = initStruct->halfDuplexOe;
    CORE_SPI->spi_fifo_ctrl.bit.tx_fifo_waterlevel = initStruct->txFifoWaterLevel;
    CORE_SPI->spi_fifo_ctrl.bit.rx_fifo_waterlevel = initStruct->rxFifoWaterLevel;
}

/**
 * @brief Deinitializes the SPI registers to their default reset values.
 *
 * @param None
 *
 * @return None
 */
void HSPI_DeInit(void)
{
    CORE_SPI->spi_ctrl.reg = 0;
}

/**
 * @brief Enables or disables the SPI module.
 *
 * @param newState ENABLE or DISABLE
 *
 * @return None
 */
void HSPI_EnableCmd(FunctionalState newState)
{
    CORE_SPI->spi_ctrl.bit.spi_en = newState;
}

/**
 * @brief Enables or disables the SPI DMA.
 *
 * @param newState ENABLE or DISABLE
 *
 * @return None
 */
void HSPI_DmaEnableCmd(FunctionalState newState)
{

    CORE_SPI->spi_dma.bit.dma_en = newState;
    CORE_SPI->spi_dma.bit.dma_start_auto=newState;
}
/**
 * @brief Get HSPI clock
 *
 * @param None
 *
 * @return HSPI clock
 */
uint32_t HSPI_GetClk(void)
{
    uint32_t sys_clksel = CORE_CHIP_SYS->sys_clksel.bit.sys_clksel;
    uint32_t spi_clk = 48000000;

    switch (sys_clksel)
    {
    case SYS_CLKSEL_RC:
        spi_clk = RC_CLK_SOURCE;
        break;
    case SYS_CLKSEL_DPLL:
        spi_clk = 96000000;
        break;
    case SYS_CLKSEL_CRYSTAL:
        spi_clk = EXTERNAL_CRYSTAL;
        break;
    default:
        spi_clk = 32768;
        break;
    }

    return spi_clk;
}

/**
 * @brief Enables or disables the SPI FIFO auto start.
 *
 * @param newState ENABLE or DISABLE
 *
 * @return None
 */
void HSPI_FifoAutoStartCmd(FunctionalState newState)
{
    CORE_SPI->spi_fifo.bit.fifo_rst_auto = newState;
}
/**
 * @brief Enables or disables the SPI slave.
 *
 * @param newState ENABLE or DISABLE
 *
 * @return None
 */
void HSPI_SlaveEnableCmd(FunctionalState newState)
{
    CORE_SPI->spi_fifo.bit.slv_fifo_en =newState;
}

/**
 * @brief Enables or disables the SPI interface interrupts.
 *
 * @param type interrupt type
 *
 * @param newState ENABLE or DISABLE
 *
 * @return None
 */
void HSPI_SetIrq(HSPI_StatusEnum type, FunctionalState newState)
{
    switch(type)
    {
        case DMA_DONE:
               CORE_SPI->spi_ie.bit.dma_done_ie = newState;
               break;
        case TXFIFO_LOW:
               CORE_SPI->spi_ie.bit.tx_fifo_lo_ie = newState;
               break;
        case TXFIFO_OVERFLOW:
               CORE_SPI->spi_ie.bit.tx_fifo_ovfl_ie = newState;
               break;
        case RXFIFO_HIGH:
               CORE_SPI->spi_ie.bit.rx_fifo_hi_ie = newState;
               break;
        case RXFIFO_OVERFLOW:
               CORE_SPI->spi_ie.bit.rx_fifo_ovfl_ie = newState;
               break;
        case START_OVER_QUICK:
               CORE_SPI->spi_ie.bit.start_over_quick_ie = newState;
               break;
        case TX_EMPTY:
               CORE_SPI->spi_ie.bit.tx_empty_ie = newState;
               break;
        case RX_NOT_EMPTY:
               CORE_SPI->spi_ie.bit.rx_not_empty_ie = newState;
               break;
        case RX_OVERRUN:
               CORE_SPI->spi_ie.bit.rx_overrun_ie = newState;
            break;
        case ALL_SPI_STATUS:
            CORE_SPI->spi_ie.reg = 0x1FF;
               break;
        default:
               break;
    }
}

/**
 * @brief Clears the SPI Status and interrupt
 *
 * @param type spi status type
 *
 * @return None
 */
void HSPI_ClearIrq(HSPI_StatusEnum type)
{
    switch(type)
    {
        case DMA_DONE:
               CORE_SPI->spi_ris.bit.dma_done_ris = 1;
               break;
        case TXFIFO_LOW:
               CORE_SPI->spi_ris.bit.tx_fifo_lo_ris = 1;
               break;
        case TXFIFO_OVERFLOW:
               CORE_SPI->spi_ris.bit.tx_fifo_ovfl_ris = 1;
               break;
        case RXFIFO_HIGH:
               CORE_SPI->spi_ris.bit.rx_fifo_hi_ris = 1;
               break;
        case RXFIFO_OVERFLOW:
               CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris = 1;
               break;
        case START_OVER_QUICK:
            CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris = 1;
            break;
        case TX_EMPTY:
            CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris = 1;
            break;
        case RX_NOT_EMPTY:
            CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris = 1;
            break;
        case RX_OVERRUN:
            CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris = 1;
            break;
        case ALL_SPI_STATUS:
            CORE_SPI->spi_ris.reg = 0x1ff;
            break;
           default:
               break;
    }
}


/**
 * @brief get spi status
 *
 * @param type spi status type
 *
 * @return FlagStatus SET or RESET
 */
FlagStatus HSPI_GetSpiStatus(HSPI_StatusEnum type)
{

    FlagStatus bitStatus = RESET;

    switch(type)
    {
           case DMA_DONE:
               bitStatus = CORE_SPI->spi_ris.bit.dma_done_ris;
               break;
           case TXFIFO_LOW:
               bitStatus = CORE_SPI->spi_ris.bit.tx_fifo_lo_ris;
               break;
           case TXFIFO_OVERFLOW:
               bitStatus = CORE_SPI->spi_ris.bit.tx_fifo_ovfl_ris;
               break;
           case RXFIFO_HIGH:
               bitStatus = CORE_SPI->spi_ris.bit.rx_fifo_hi_ris;
               break;
           case RXFIFO_OVERFLOW:
               bitStatus = CORE_SPI->spi_ris.bit.rx_fifo_ovfl_ris;
               break;
           case START_OVER_QUICK:
               bitStatus = CORE_SPI->spi_ris.bit.start_over_quick_ris;
               break;
            case TX_EMPTY:
                bitStatus = CORE_SPI->spi_ris.bit.tx_empty_ris ;
                break;
            case RX_NOT_EMPTY:
                bitStatus=CORE_SPI->spi_ris.bit.rx_not_empty_ris ;
                break;
            case RX_OVERRUN:
                bitStatus=CORE_SPI->spi_ris.bit.rx_overrun_ris;
               break;
            case ALL_SPI_STATUS:
                bitStatus=CORE_SPI->spi_ris.reg;
            default:
               break;
    }
    return  bitStatus;
}

/**
 * @brief get spi irq status
 *
 * @param type spi status type
 *
 * @return FlagStatus SET or RESET
 */
FlagStatus HSPI_GetIrqStatus(HSPI_StatusEnum type)
{
    FlagStatus bitStatus = RESET;
    switch(type)
    {
        case DMA_DONE:
            bitStatus = CORE_SPI->spi_mis.bit.dma_done_mis;
            break;
        case TXFIFO_LOW:
            bitStatus = CORE_SPI->spi_mis.bit.tx_fifo_lo_mis;
            break;
        case TXFIFO_OVERFLOW:
            bitStatus = CORE_SPI->spi_mis.bit.tx_fifo_ovfl_mis;
            break;
        case RXFIFO_HIGH:
            bitStatus = CORE_SPI->spi_mis.bit.rx_fifo_hi_mis;
            break;
        case RXFIFO_OVERFLOW:
            bitStatus = CORE_SPI->spi_mis.bit.rx_fifo_ovfl_mis;
            break;
        case START_OVER_QUICK:
            bitStatus = CORE_SPI->spi_mis.bit.start_over_quick_mis;
            break;
        case TX_EMPTY:
            bitStatus = CORE_SPI->spi_mis.bit.tx_empty_mis ;
            break;
        case RX_NOT_EMPTY:
            bitStatus=CORE_SPI->spi_mis.bit.rx_not_empty_mis ;
            break;
        case RX_OVERRUN:
            bitStatus=CORE_SPI->spi_mis.bit.rx_overrun_mis;
            break;
        case ALL_SPI_STATUS:
            bitStatus=CORE_SPI->spi_mis.reg;
               break;
            default:
               break;
    }

    return  bitStatus;
}
/**
 * @brief Get all spi irq flag status
 * @param none
 * @return get spi flag status
 */
uint16_t HSPI_GetAllIrqStatus(void)
{
    return CORE_SPI->spi_mis.reg;
}
/**
 * @brief move rx fifo ptr one byte(only take affect when fifo_sw_en '1')
 *
 * @param None
 *
 * @return None
 */
void HSPI_MoveRxFifoPtr(void)
{

    CORE_SPI->spi_fifo.bit.rx_fifo_rd = 1;
}

/**
 * @brief move tx fifo ptr one byte(only take affect when fifo_sw_en '1')
 *
 * @param None
 *
 * @return None
 */
void HSPI_MoveTxFifoPtr(void)
{

    CORE_SPI->spi_fifo.bit.tx_fifo_rd = 1;
}

/**
 * @brief read rx fifo current data(move ptr use SPI_MoveRxFifoPtr)
 *
 * @param None
 *
 * @return data
 */
uint8_t HSPI_ReadRxFifoData(void)
{

    return CORE_SPI->spi_rx_fifo.bit.rx_fifo_data ;
}

/**
 * @brief Write rx fifo current data(move ptr use SPI_MoveRxFifoPtr)
 *
 * @param data To write rxdata
 *
 * @return None
 */
void HSPI_WriteRxFifoData(uint8_t data)
{

    CORE_SPI->spi_rx_fifo.bit.rx_fifo_data = data;
}

/**
 * @brief read tx fifo current data(move ptr use SPI_MoveTxFifoPtr)
 *
 * @param None
 *
 * @return data
 */
uint8_t HSPI_ReadTxFifoData(void)
{

    return CORE_SPI->spi_tx_fifo.bit.tx_fifo_data ;
}

/**
 * @brief Write tx fifo current data(move ptr use SPI_MoveTxFifoPtr)
 *
 * @param data To write txdata
 *
 * @return None
 */
void HSPI_WriteTxFifoData(uint8_t data)
{

    CORE_SPI->spi_tx_fifo.bit.tx_fifo_data = data;
}

/**
 * @brief spi send one byte data
 *
 * @param data the data to send
 *
 * @return None
 */
void HSPI_SendData(uint8_t data)
{
    HSPI_SendAndReceiveData(&data, 1, 0, 0);
}

/**
 * @brief spi receive one byte data
 *
 * @param None
 *
 * @return data
 */
uint8_t HSPI_ReceiveData(void)
{
    uint8_t tempData=0;
    HSPI_SendAndReceiveData(0, 0, &tempData, 1);
    return tempData;
}

/**
 * @brief spi send buf
 *
 * @param buff the buf to send
 *
 * @param len the send len
 *
 * @return None
 */
void HSPI_SendBuff(uint8_t *buff, uint32_t len)
{
    HSPI_SendAndReceiveData(buff, len, 0, 0);
}

/**
 * @brief spi receive buf
 *
 * @param buff the buf to receive data
 *
 * @param len the receive len
 *
 * @return None
 */
void HSPI_ReceiveBuff(uint8_t *buff, uint32_t len)
{
    HSPI_SendAndReceiveData(0, 0, buff, len);
}

/**
 * @brief spi Send and recerive data.
 *
 * @param txbuff pointer to a txbuff  that contains the data you want send.
 * @param txlen the length of send data
 * @param rxbuff pointer to a txbuff  that contains the data you want receive.
 * @param rxlen the length of receive data
 */
void HSPI_SendAndReceiveData(uint8_t *txbuff, uint32_t txlen, uint8_t *rxbuff, uint32_t rxlen)
{

    CORE_SPI->spi_dma_tx_len.bit.tx_len = txlen;
    CORE_SPI->spi_dma_tx_saddr.bit.tx_start_addr = (uint32_t)txbuff;
    CORE_SPI->spi_dma_rx_saddr.bit.rx_start_addr = (uint32_t)rxbuff;
    CORE_SPI->spi_dma_rx_len.bit.rx_len = rxlen;

    CORE_SPI->spi_ctrl.bit.spi_start = 1;
    #ifndef HSPI_NO_WAIT
    while (!(HSPI_GetSpiStatus(DMA_DONE)));
    HSPI_ClearIrq(DMA_DONE);
    #endif

}

