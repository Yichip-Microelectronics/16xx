/*
File Name    : yc_uart.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : uart driver interface implement.
*/
#include "yc_uart.h"

/**
  * @brief  Deinit the uartx
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @return None
  */
void UART_DeInit(CORE_UARTA_Type* uartx)
{
    uartx->uart_ctrl.reg = 0;
}

/**
  * @brief  Initializes the uartx
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  initStruct : pointer to a initStruct structure
  * @return None
  */
void UART_Init(CORE_UARTA_Type* uartx, UART_InitStruct* initStruct)
{
    if(uartx == CORE_UARTA)
    {
      SYSCTRL_ClkEn(CLK_UARTA,ENABLE);
    }
    else if(uartx == CORE_UARTB)
    {
      SYSCTRL_ClkEn(CLK_UARTB,ENABLE);
    }
    else if(uartx == CORE_UARTC)
    {
      SYSCTRL_ClkEn(CLK_UARTC,ENABLE);
    }
    else if(uartx == CORE_UARTD)
    {
      SYSCTRL_ClkEn(CLK_UARTD,ENABLE);
    }
    uartx->uart_tx_start_addr.reg = initStruct->txBuffer;
    uartx->uart_tx_end_addr.reg = (initStruct->txBuffer + initStruct->txLen - 1);
    uartx->uart_rx_start_addr.reg = initStruct->rxBuffer;
    uartx->uart_rx_end_addr.reg = (initStruct->rxBuffer + initStruct->rxLen - 1);

    uartx->uart_tx_wptr.reg = uartx->uart_tx_start_addr.reg;
    uartx->uart_rx_rptr.reg = uartx->uart_rx_start_addr.reg;

    //TODO: check clk sourcer
    UART_SetBaudRate(uartx,initStruct->baudRate);

    uartx->uart_ctrl.bit.parity_bit_set = initStruct->parity == UART_PARITY_ODD;
    uartx->uart_ctrl.bit.praity_bit_en = initStruct->parity != UART_PARITY_NONE;
    uartx->uart_ctrl.bit.flow_control_en = initStruct->flowControl;
    uartx->uart_ctrl.bit.half_duplex_mode = initStruct->mode == UART_MODE_SINGLE_LINE;
    uartx->uart_ctrl.bit.baud_rate_preset = initStruct->baudRate != 0;
    if (initStruct->stopBits == UART_STOP_BIT_0)
    {
        uartx->uart_ctrl.bit.two_stop_bit_en = UART_STOP_BIT_1;
        uartx->uart_baud_rate.bit.uart_noss = ENABLE;
    }
    else
    {
        uartx->uart_ctrl.bit.two_stop_bit_en = initStruct->stopBits;
    }

    uartx->uart_ctrl.bit.tx_en = (initStruct->txBuffer != 0
        && initStruct->txLen != 0);
    uartx->uart_ctrl.bit.rx_en = (initStruct->rxBuffer != 0
        && initStruct->rxLen != 0);
}


/**
  * @brief  send one byte
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  data : the data to send.
  * @return None
  */
_ramcode void UART_SendData(CORE_UARTA_Type* uartx, uint8_t data)
{
    // Wait buffer enough
    while(uartx->uart_status.bit.uart_status_tx_full);
    uint32_t sys_irq_status = __disable_irq();
    uint8_t *p = (uint8_t *)uartx->uart_tx_wptr.reg;
    *p = data;
    if(++p > (uint8_t*)uartx->uart_tx_end_addr.reg)
    {
        p = (uint8_t *)uartx->uart_tx_start_addr.reg;
    }
    uartx->uart_tx_wptr.reg = (uint32_t)p;
    __enable_irq(sys_irq_status);
}

/**
  * @brief  receive one byte(must be sure UART_GetRxCount()>0 before UART_ReceiveData)
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @return The received data.
  */
uint8_t UART_ReceiveData(CORE_UARTA_Type* uartx)
{
    //wait until have a byte.
    while(uartx->uart_rxitems.reg == 0);
    uint32_t sys_irq_status = __disable_irq();
    uint8_t *p = (uint8_t *)uartx->uart_rx_rptr.reg;
    uint8_t data = *p;
    if(++p > (uint8_t *)uartx->uart_rx_end_addr.reg)
    {
        p = (uint8_t *)uartx->uart_rx_start_addr.reg;
    }
    uartx->uart_rx_rptr.reg = (uint32_t)p;
    __enable_irq(sys_irq_status);
    return data;
}

/**
  * @brief  receive data into buff
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param	buff : The buff where data receive to.
  * @param	len : The length of receive data.
  * @return The number received data
  */
uint32_t UART_ReceiveBuff(CORE_UARTA_Type* uartx, uint8_t* buff, uint32_t len)
{
    uint32_t rxTotalCnt = 0;
    uint32_t sys_irq_status = __disable_irq();
    while((uartx->uart_rxitems.reg != 0) && (rxTotalCnt < len))
    {
        uint8_t *p = (uint8_t *)uartx->uart_rx_rptr.reg;
        buff[rxTotalCnt++] = *p;
        if(++p > (uint8_t *)uartx->uart_rx_end_addr.reg)
        {
            p = (uint8_t *)uartx->uart_rx_start_addr.reg;
        }
        uartx->uart_rx_rptr.reg = (uint32_t)p;
    }
    __enable_irq(sys_irq_status);
    return rxTotalCnt;
}

/**
  * @brief  send buffer
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  buff : The send data buffer
  * @param	len : The send len
  * @return The number sended data
  */
uint32_t UART_SendBuff(CORE_UARTA_Type* uartx, uint8_t* buff, uint32_t len)
{
    int i=0;
    for(;i<len;i++)
    {
        UART_SendData(uartx, buff[i]);
    }
    return i;
}

/**
  * @brief  Get RX DMA buffer data num(received and not read)
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @return data num
  */
uint32_t UART_GetRxCount(CORE_UARTA_Type* uartx)
{
    return uartx->uart_rxitems.reg;
}

/**
  * @brief  Get TX DMA buffer data num(wait send data)
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @return data num
  */
uint32_t UART_GetTxCount(CORE_UARTA_Type* uartx)
{
    return uartx->uart_txitems.reg;
}

/**
  * @brief  Set the number of buffered data to trigger RX interrupt(trigger RX interrupt when UART_GetRxCount()==num)
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  num : interrupt Number of data,0 is disable is function
  * @return none
  */
void UART_SetRxNumIrq(CORE_UARTA_Type* uartx, uint8_t num)
{
    uartx->uart_ctrl.bit.rx_intr_threshold = num;
}

/**
  * @brief  Set RX character interval timeout interrupt time(Restart the time when receive new data)
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  time : timeout of trigger interrupt,0 is disable is function
  * @return None.
  */
void UART_SetRxTimeout(CORE_UARTA_Type* uartx, uint16_t time)
{
    uartx->uart_ctrl.bit.rx_intr_timeout = time;
}

/**
  * @brief  get uart status
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @return status(reference UART_status_define)
  */
uint8_t UART_GetIrqStatus(CORE_UARTA_Type* uartx)
{
    return (uartx->uart_status.reg&0x7f);
}

/**
  * @brief  Set TX interrupt ENABLE or DISABLE
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  newState : ENABLE or DISABLE
  * @return None.
  */
void UART_SetTxIrq(CORE_UARTA_Type* uartx, FunctionalState newState)
{
    uartx->uart_baud_rate.bit.tx_intr_en = newState;
}


/**
  * @brief  clear TX interrupt
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  none
  * @return None.
  */
void UART_ClearTxIrq(CORE_UARTA_Type* uartx)
{
    uartx->uart_baud_rate.bit.txintr_clr=1;
}

/**
  * @brief  clear RX num irq
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  none
  * @return None.
  */
void UART_ClearRxNumIrq(CORE_UARTA_Type* uartx)
{
    uartx->uart_baud_rate.bit.rxintr_clr=1;
}

/**
  * @brief  clear RX timeout irq
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  none
  * @return None.
  */
void UART_ClearRxTimeoutIrq(CORE_UARTA_Type* uartx)
{
    uartx->uart_baud_rate.bit.timeout_clr=1;
}

/**
  * @brief  Set auto detect baud rate ENABLE or DISABLE
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  newState : ENABLE or DISABLE
  * @return None.
  */
void UART_SetAutoBaud(CORE_UARTA_Type* uartx, FunctionalState newState)
{
    uartx->uart_ctrl.bit.baud_rate_preset = ~newState;
}

/**
  * @brief  Set uart baud rate
  * @param  uartx : Select the UART peripheral.
  *                 CORE_UARTA
  *                 CORE_UARTB
  *                 CORE_UARTC
  *                 CORE_UARTD
  * @param  baudRate : baud rate
  * @return None.
  */
void UART_SetBaudRate(CORE_UARTA_Type*  uartx, uint32_t baudRate)
{
    uint32_t uart_module_clk;

    if(CORE_CHIP_SYS->sys_clksel.bit.sys_clksel==SYS_CLKSEL_RC)
    {
        uart_module_clk=RC_CLK_SOURCE;
    }
    else
    {
        if(CORE_CHIP_SYS->sys_clksel.bit.uart_clksel==0)
        {
            uart_module_clk=EXTERNAL_CRYSTAL;
        }
        else
        {
            uart_module_clk=48000000;
        }
    }
    uartx->uart_baud_rate.bit.baud_rate = uart_module_clk/baudRate;
}

