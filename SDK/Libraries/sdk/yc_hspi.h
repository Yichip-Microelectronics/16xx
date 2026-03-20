/*
File Name    : yc_hspi.h
Author       : Yichip
Version      : V1.0
Date         : 2024/4/28
Description  : hspi driver interface define.
*/


/* Includes ------------------------------------------------------------------*/

#ifndef __YC_HSPI_H
#define __YC_HSPI_H

#include "register.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HSPI_TXRX_SEQUENCE_CONCURRENT       (0) /*Sends and receives packets simultaneously. The length is tx_len*/
#define HSPI_TXRX_SEQUENCE_TX_FIRST         (1) /*First tx_len's sends (skips send when tx_len is 0), and then rx_len's receives (skips receive when rx_len is 0)*/
#define IS_HSPI_TxRx_SEQUENCE(SEQUENCE)     (((SEQUENCE) == HSPI_TXRX_SEQUENCE_CONCURRENT) ||\
                                            ((SEQUENCE)  == HSPI_TXRX_SEQUENCE_CONCURRENT))

#define HSPI_CPHA_FIRST_EDGE                (0)
#define HSPI_CPHA_SECOND_EDGE               (1)
#define IS_HSPI_CPHA(EDGE)                  (((EDGE) == HSPI_CPHA_FIRST_EDGE) ||\
                                            ((EDGE)  == HSPI_CPHA_SECOND_EDGE))

#define HSPI_CPOL_LOW                       (0)
#define HSPI_CPOL_HIGH                      (1)
#define IS_HSPI_CPOL(PHASE)                 (((PHASE) == HSPI_CPOL_LOW) ||\
                                            ((PHASE)  == HSPI_CPOL_HIGH))

// #define START_OVER_QUICK                    (1<<5)
// #define RXFIFO_OVERFLOW                     (1<<4)
// #define RXFIFO_HIGH                         (1<<3)
// #define TXFIFO_OVERFLOW                     (1<<2)
// #define TXFIFO_LOW                          (1<<1)
// #define DMA_DOWN_IRQ                        (1<<0)

#define IS_HSPI_IRQ_CONFIG(IRQ)             ((((IRQ) & 0xc0) == 0x00) && ((IRQ) != 0x00))
#define IX_HSPI_IRQ_CLEAR(IRQ)              ((((IRQ) & 0xc0) == 0x00) && ((IRQ) != 0x00))

/**
* @brief HSPI_STATUS
*/
typedef enum
{
    DMA_DONE = 0,
    TXFIFO_LOW,
    TXFIFO_OVERFLOW,
    RXFIFO_HIGH,
    RXFIFO_OVERFLOW,
    START_OVER_QUICK,
    TX_EMPTY,
    RX_NOT_EMPTY,
    RX_OVERRUN,
    ALL_SPI_STATUS = 0xff
} HSPI_StatusEnum;

typedef struct
{
    uint8_t rxAdjClk;           /*!< Specifies the received phase adjustment value,
                                the sampling time is delayed (RxAdjClk + 1)*spi_clk clock cycles,
                                shall not be greater than ClkDiv value.
                                This parameter can be a value between 0x00 and 0x0F */
    uint8_t clkDiv;             /*!< Specifies the clock frequency division factor, division value is (ClkDiv + 1)*2.
                                This parameter can be a value between 0x00 and 0x0F */
    uint8_t txRxDly;            /*!< Specifies the send and receive protection interval, interval value is spi_clk*(TxRxDly + 1)*4.
                                This parameter can be a value between 0x00 and 0x0F */
    uint8_t ncsDly;             /*!< Specifies the delay time of NCS,delay value is spi_clk*(NcsDly + 1).
                                This parameter can be a value between 0x00 and 0x07 */
    uint16_t rxAdjen;           /*!< Specifies the Receive phase adjustment enable switch.
                                This parameter can be a value between 0x00 and 0x01 */
    uint16_t txRxSeq;           /*!< Specifies the tx and rx sequence control:
                                This parameter can be a value of SPI_Transmit_Mode_Full_Duplex or SPI_Transmit_Mode_Half_Duplex */
    uint16_t cpha;              /*!< Specifies the clock active edge for the bit capture.
                                This parameter can be a value of @ref SPI_Clock_Phase */
    uint16_t cpol;              /*!< Specifies the serial clock steady state.
                                This parameter can be a value of @ref SPI_Clock_Polarity */
    uint8_t rxFifoWaterLevel;   /*!< Specifies the rx fifo waterlevel.
                                This parameter can be a value between 0x00 and 0x07 */
    uint8_t txFifoWaterLevel;   /*!< Specifies the tx fifo waterlevel.
                                This parameter can be a value between 0x00 and 0x07 */

    uint8_t fifoSwEn;           /*!< Specifies the fifo_sw_en. */
    uint8_t halfDuplexEn;     /*!< single line half duplex mode enable(MOSI is data line in master mode;MISO is data line in slave mode) */
    uint8_t halfDuplexOe;     /*!< single line half duplex mode data out enable(this bit only take affect in slave mode) */
    uint8_t slaveMode;         /*!< slave mode enable */
} HSPI_InitStruct;


void HSPI_Init(HSPI_InitStruct* initStruct);
void HSPI_DeInit(void);
void HSPI_EnableCmd(FunctionalState newState);
uint32_t HSPI_GetClk(void);
void HSPI_DmaEnableCmd(FunctionalState newState);
void HSPI_FifoAutoStartCmd(FunctionalState newState);
void HSPI_SlaveEnableCmd(FunctionalState newState);
void HSPI_SetIrq(HSPI_StatusEnum type, FunctionalState newState);
void HSPI_ClearIrq(HSPI_StatusEnum type);
FlagStatus HSPI_GetSpiStatus(HSPI_StatusEnum type);
FlagStatus HSPI_GetIrqStatus(HSPI_StatusEnum type);
uint16_t HSPI_GetAllIrqStatus(void);
void HSPI_MoveRxFifoPtr(void);
void HSPI_MoveTxFifoPtr(void);
uint8_t HSPI_ReadRxFifoData(void);
void HSPI_WriteRxFifoData(uint8_t data);
uint8_t HSPI_ReadTxFifoData(void);
void HSPI_WriteTxFifoData(uint8_t data);
void HSPI_SendData(uint8_t data);
uint8_t HSPI_ReceiveData(void);
void HSPI_SendBuff(uint8_t *buff, uint32_t len);
void HSPI_ReceiveBuff(uint8_t *buff, uint32_t len);
void HSPI_SendAndReceiveData(uint8_t *txbuff, uint32_t txlen, uint8_t *rxbuff, uint32_t rxlen);


#ifdef __cplusplus
}
#endif
#endif  /* __YC_HSPI_H */
