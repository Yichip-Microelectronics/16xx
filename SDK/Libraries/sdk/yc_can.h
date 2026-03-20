/*
File Name    : yc_can.h
Author       : Yichip
Version      : V1.0
Date         : 2024/5/22
Description  : can driver interface define.
*/

#ifndef _YC_CAN_H_
#define _YC_CAN_H_

#include "register.h"
#include "yc_log.h"

/*ID*/
#define CAN_ID_STD                  ((uint32_t)0x00000000)	  	/*!< Standard Id */
#define CAN_ID_EXT                  ((uint32_t)0x00000001)	  	/*!< Extended Id */
#define CAN_RTR_DATA                ((uint32_t)0x00000000)	  	/*!<*/
#define CAN_RTR_REMOTE              ((uint32_t)0x00000001)		/*!<*/

/*mode*/
#define CAN_NORMAL                  (0)
#define CAN_SELF_RX_MODE            (1)
#define CAN_LOOPBACK_MODE           (2)
#define CAN_LISTEN_ONLY_MODE        (3)


/* CAN_IT */
#define CAN_IT_RC                   ((uint32_t)0x00000001)  	/* The reception is complete */
#define CAN_IT_RXVLD                ((uint32_t)0x00000002)      /* Receive FIFO is not empty */
#define CAN_IT_RXFULL               ((uint32_t)0x00000004)      /* Receive FIFO full interrupt */
#define CAN_IT_RXOVER               ((uint32_t)0x00000008)      /* Receive FIFO overflow interrupt */
#define CAN_IT_TC                   ((uint32_t)0x00000010)      /* Send complete，If the send is aborted, it will also be set to one */

/* CAN_filter_number */
#define CAN_FILTERMODE_IDMASK       ((uint8_t)0x00)         /*!< id/mask mode */
#define CAN_FILTERMODE_IDLIST       ((uint8_t)0x01)         /*!< identifier list mode */
#define IS_FILTER_MODE(MODE)        (((MODE) == CAN_FILTERMODE_IDMASK) ||\
                                    ((MODE) == CAN_FILTERMODE_IDLIST))

/* CAN_filter_number */
#define CAN_FILTERSCALE_16BIT       ((uint8_t)0x00) /*!< Two 16-bit filters */
#define CAN_FILTERSCALE_32BIT       ((uint8_t)0x01) /*!< One 32-bit filter */

#define IS_FILTER_SCALE(SCALE)      (((SCALE) == CAN_FILTERSCALE_16BIT) ||\
                                    ((SCALE) == CAN_FILTERSCALE_32BIT))

/* CAN_filter active or CAN_function state*/

/**
 * @brief  CAN init structure definition
 */

typedef struct
{
    uint16_t prescaler; /*!< These bits define the length of a time quanta.
                                Tq = (BRP[9:0]+1)*Tclk_can */

    uint8_t mode; /*!< Specifies the CAN operating mode.
                            This parameter can be a value of CAN_NORMAL/CAN_SELF_RX_MODE/CAN_LOOPBACK_MODE/CAN_LISTEN_ONLY_MODE
                            @ref CAN_operating_mode */

    uint8_t sjw; /*!< Specifies the maximum number of time quanta
                            the CAN hardware is allowed to lengthen or
                            shorten a bit to perform resynchronization.
                            This parameter can be a value of
                            @ref CAN_synchronisation_jump_width */

    uint8_t bs1; /*!< Specifies the number of time quanta in Bit
                            Segment 1. This parameter can be a value of
                            @ref CAN_time_quantum_in_bit_segment_1 */

    uint8_t bs2; /*!< Specifies the number of time quanta in Bit
                            Segment 2.
                            This parameter can be a value of
                            @ref CAN_time_quantum_in_bit_segment_2 */
    uint8_t sam; /*!< Sampling mode 0: sample once 1: sample three times*/
} CAN_InitStruct;

/**
 * @brief  CAN filter init structure definition
 */
typedef struct
{
	uint8_t filterMode;                 /*!< Specifies the filter mode to be initialized.
                                        This parameter can be a value of @ref CAN_filter_mode */

	uint8_t filterScale;                /*!< Specifies the filter scale.
                                        This parameter can be a value of @ref CAN_filter_scale */

	FunctionalState filterActivation; 	/*!< Enable or disable the filter.
                                        This parameter can be set either to ENABLE or DISABLE. */

  	uint16_t filterIdHigh;             /*!< Specifies the filter identification number (MSBs for a 32-bit
                                        configuration, first one for a 16-bit configuration).
                                        This parameter can be a value between 0x0000 and 0xFFFF */

 	 uint16_t filterIdLow;              /*!< Specifies the filter identification number (LSBs for a 32-bit
                                        configuration, second one for a 16-bit configuration).
                                        This parameter can be a value between 0x0000 and 0xFFFF */

  	uint16_t filterMaskIdHigh;         /*!< Specifies the filter mask number or identification number,
                                        according to the mode (MSBs for a 32-bit configuration,
                                        first one for a 16-bit configuration).
                                        This parameter can be a value between 0x0000 and 0xFFFF */

  	uint16_t filterMaskIdLow;          /*!< Specifies the filter mask number or identification number,
                                        according to the mode (LSBs for a 32-bit configuration,
                                        second one for a 16-bit configuration).
                                        This parameter can be a value between 0x0000 and 0xFFFF */

	uint8_t filterIde;                  /*!< Specifies the type of identifier for the message that
                                        will be transmitted. This parameter can be a value
                                        of @ref CAN_identifier_type */

	uint8_t filterRtr;              /*!< Specifies the type of frame for the message that will
                                        be transmitted. This parameter can be a value of
                                        @ref CAN_remote_transmission_request */
}CAN_FilterStruct;


typedef struct
{
    CAN_FilterStruct filterStruct[4];      /* There are four sets of filters, each corresponding to a structure*/

}CAN_FilterInitStruct;

/**
 * @brief  CAN Tx message structure definition
 */
typedef struct
{
    uint32_t stdId;         /*!< Specifies the standard identifier.
                            This parameter can be a value between 0 to 0x7FF. */

    uint32_t extId;         /*!< Specifies the extended identifier.
                            This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t ide;            /*!< Specifies the type of identifier for the message that
                            will be transmitted. This parameter can be a value
                            of @ref CAN_identifier_type */

    uint8_t rtr;            /*!< Specifies the type of frame for the message that will
                            be transmitted. This parameter can be a value of
                            @ref CAN_remote_transmission_request */

    uint8_t dlc;            /*!< Specifies the length of the frame that will be
                            transmitted. This parameter can be a value between
                            0 to 8 */

    uint8_t data[8];        /*!< Contains the data to be transmitted. It ranges from 0
                            to 0xFF. */
} CAN_TxMsgStruct;

/**
 * @brief  CAN Rx message structure definition
 */
typedef struct
{
    uint32_t stdId;         /*!< Specifies the standard identifier.
                            This parameter can be a value between 0 to 0x7FF. */

    uint32_t extId;         /*!< Specifies the extended identifier.
                            This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t ide;            /*!< Specifies the type of identifier for the message that
                            will be received. This parameter can be a value of
                            @ref CAN_identifier_type */

    uint8_t rtr;            /*!< Specifies the type of frame for the received message.
                            This parameter can be a value of
                            @ref CAN_remote_transmission_request */

    uint8_t dlc;            /*!< Specifies the length of the frame that will be received.
                            This parameter can be a value between 0 to 8 */

    uint8_t data[8];        /*!< Contains the data to be received. It ranges from 0 to
                            0xFF. */

    uint8_t fmi;            /*!< reserved */
} CAN_RxMsgStruct;

#define CAN_FLAG                    ((uint32_t)0x00000001)          /*!< Once a frame is transmitting, buff busy turns '1', and user should not change the buff   */
#define CAN_FLAG_TX_BUSY            ((uint32_t)0x00000001)          /*!< Once a frame is transmitting, buff busy turns '1', and user should not change the buff   */
#define CAN_FLAG_TX_PEND            ((uint32_t)0x00000002)          /*!< a frame is transmitting or wait to send  */
#define CAN_FLAG_RX_STU             ((uint32_t)0x00000004)          /*!< A frame is ongoing, and CAN now act as Receiver         */
#define CAN_FLAG_TX_STU             ((uint32_t)0x00000008)          /*!< A frame is ongoing, and CAN now act as Transmitter */
#define CAN_FLAG_TX_VAL             ((uint32_t)0x00000010)          /*!< The current value of TX data line.   */
#define CAN_FLAG_RX_VAL             ((uint32_t)0x00000020)          /*!< The current value of RX data line. */
#define CAN_FLAG_SAM_BIT            ((uint32_t)0x00000040)          /*!< Current sample value.         */
#define CAN_FLAG_IDE_STU            ((uint32_t)0x00000080)          /*!< The controller is idle.*/
#define CAN_FLAG_ERR_PASSIVE        ((uint32_t)0x00000100)          /*!< The current value of error_passive.   */
#define CAN_FLAG_BUF_OFF            ((uint32_t)0x00000200)          /*!< The current value of bus_off. */
#define CAN_FLAG_ERR_FRAME          ((uint32_t)0x00000400)          /*!< Error frames are currently being sent.         */
#define CAN_FLAG_OVERLOAD_FRAME     ((uint32_t)0x00000800)          /*!< Overload frames are currently being sent.*/

void CAN_DeInit(CAN_Type *canx);
void CAN_Init(CAN_Type *canx, CAN_InitStruct *initStruct);
void CAN_FilterInit(CAN_Type *canx, CAN_FilterInitStruct *filterInitStruct);
uint8_t CAN_SendData(CAN_Type *canx, CAN_TxMsgStruct *txMessage);
FlagStatus CAN_GetFlagStatus(CAN_Type *canx, uint32_t canFlag);
uint8_t CAN_MessagePending(CAN_Type *canx);
uint8_t CAN_ReceiveData(CAN_Type *canx, CAN_RxMsgStruct *rxMessage);
void CAN_SetIrq(CAN_Type* canx, uint32_t canIrq, FunctionalState newState);
uint8_t CAN_GetIrqStatus(CAN_Type* canx, uint32_t canIrq);
void CAN_ClearIrq(CAN_Type* canx, uint32_t canIrq);
void CAN_Sleep(CAN_Type* canx);
void CAN_WakeUp(CAN_Type* canx);


#endif