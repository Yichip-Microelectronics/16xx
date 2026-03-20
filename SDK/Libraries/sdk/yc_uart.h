
/*
File Name    : yc_uart.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : uart driver interface define.
*/

#ifndef _YC_UART_H_
#define _YC_UART_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup UART_Stop_Bits
  * @{
  */
#define UART_STOP_BIT_0                         (0xff)
#define UART_STOP_BIT_1                         (0)
#define UART_STOP_BIT_2                         (1)
#define IS_UART_STOPBITS(STOPBITS)              (((STOPBITS) == UART_STOP_BIT_0) || \
                                                 ((STOPBITS) == UART_STOP_BIT_1) || \
                                                 ((STOPBITS) == UART_STOP_BIT_2) )

  /** @defgroup UART_Parity
  * @{
  */
#define UART_PARITY_NONE                        (0)
#define UART_PARITY_EVEN                        (1)
#define UART_PARITY_ODD                         (2)
#define IS_UART_PARITY(PARITY)                  (((PARITY) == UART_PARITY_NONE) || \
                                                 ((PARITY) == UART_PARITY_EVEN) || \
                                                 ((PARITY) == UART_PARITY_ODD))

/** @defgroup UART_Mode
  * @{
  */
#define UART_MODE_SINGLE_LINE                   (1)
#define UART_MODE_DUPLEX                        (0)
#define IS_UART_MODE(MODE)                      (((MODE) == UART_MODE_SINGLE_LINE) ||\
                                                 ((MODE) == UART_MODE_DUPLEX))

/** @defgroup UART_Hardware_Flow_Control
  * @{
  */
#define UART_HARDWARE_FLOW_CONTROL_NONE           (0)
#define UART_HARDWARE_FLOW_CONTROL_ENABLE         (1)

#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)  (((CONTROL) == UART_HARDWARE_FLOW_CONTROL_NONE) || \
                                                 ((CONTROL) == UART_HARDWARE_FLOW_CONTROL_ENABLE))


/** @defgroup UART_status_define
  * @{
  */
#define UART_STATUS_TX_BUSY                     (1<<0)
#define UART_STATUS_TX_FULL                     (1<<1)
#define UART_STATUS_RX_EMPTY                    (1<<2)
#define UART_STATUS_RX_FULL                     (1<<3)
#define UART_STATUS_TX_INTR                     (1<<4)
#define UART_STATUS_RX_INTR                     (1<<5)
#define UART_STATUS_TIMEOUTINTR                 (1<<6)

/**
* @brief  UART Init Structure definition
*/

typedef struct
{
    uint32_t baudRate;                  /*!< This member configures the UART communication baud rate(2400 - 3M bps)*/

    uint8_t stopBits;                   /*!< UART_STOP_BIT_0 UART_STOP_BIT_1 or UART_STOP_BIT_2 */

    uint8_t parity;                     /*!< UART_PARITY_NONE UART_PARITY_EVEN or UART_PARITY_ODD */

    uint8_t mode;                /*!< UART_MODE_SINGLE_LINE or UART_MODE_DUPLEX */

    uint8_t flowControl; /*!< UART_HARDWARE_FLOW_CONTROL_NONE or UART_HARDWARE_FLOW_CONTROL_ENABLE */

    uint32_t txLen; /*!< Specifies Tx DMA buff len */

    uint32_t rxLen; /*!< Specifies Rx DMA buff len */

    uint32_t txBuffer; /*!Tx ring buffer start addr */

    uint32_t rxBuffer; /*!Rx ring buffer start addr */
} UART_InitStruct;


void UART_DeInit(CORE_UARTA_Type* uartx);
void UART_Init(CORE_UARTA_Type*  uartx, UART_InitStruct* initStruct);
void UART_SendData(CORE_UARTA_Type*  uartx, uint8_t data);
uint8_t UART_ReceiveData(CORE_UARTA_Type*  uartx);
uint32_t UART_ReceiveBuff(CORE_UARTA_Type*  uartx, uint8_t* buff, uint32_t len);
uint32_t UART_SendBuff(CORE_UARTA_Type*  uartx, uint8_t* buff, uint32_t len);
uint32_t UART_GetRxCount(CORE_UARTA_Type*  uartx);
uint32_t UART_GetTxCount(CORE_UARTA_Type*  uartx);
void UART_SetRxNumIrq(CORE_UARTA_Type*  uartx, uint8_t num);
void UART_SetRxTimeout(CORE_UARTA_Type*  uartx, uint16_t time);
uint8_t UART_GetIrqStatus(CORE_UARTA_Type*  uartx);
void UART_SetTxIrq(CORE_UARTA_Type*  uartx, FunctionalState newState);
void UART_ClearTxIrq(CORE_UARTA_Type*  uartx);
void UART_ClearRxNumIrq(CORE_UARTA_Type*  uartx);
void UART_ClearRxTimeoutIrq(CORE_UARTA_Type*  uartx);
void UART_SetBaudRate(CORE_UARTA_Type*  uartx, uint32_t baudRate);
void UART_SetAutoBaud(CORE_UARTA_Type*  uartx, FunctionalState newState);

#ifdef __cplusplus
}
#endif

#endif /* _YC_UART_H_ */




