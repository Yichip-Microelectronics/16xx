/*
File Name    : yc_can.c
Author       : Yichip
Version      : V1.0
Date         : 2024/5/22
Description  : can driver interface source.
*/

#include "yc_can.h"

/**
 * @brief  Deinitializes the CAN peripheral registers to their default reset values.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @retval None.
 */
void CAN_DeInit(CAN_Type *canx)
{
    canx->rx_items.reg = 0;
    canx->command.reg = 0;
    canx->irq_en.reg = 0;
    canx->mode.reg = 0;
}
/**
 * @brief  Initializes the CAN peripheral according to the specified
 *         parameters in the CAN_InitStruct.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @param  CAN_InitStruct: pointer to a CAN_InitTypeDef structure that
 *                         contains the configuration information for the
 *                         CAN peripheral.
 * @retval none
 */
void CAN_Init(CAN_Type *canx, CAN_InitStruct *initStruct)
{
    canx->mode.bit.reset_mode = SET;
    canx->bus_timing_0.bit.brp = initStruct->prescaler;
    canx->bus_timing_0.bit.sjw = initStruct->sjw;
    canx->bus_timing_1.bit.tseg1 = initStruct->bs1;
    canx->bus_timing_1.bit.tseg2 = initStruct->bs2;
    canx->bus_timing_1.bit.sam = initStruct->sam;

    canx->mode.bit.listen_only_mode = CAN_NORMAL;
    switch (initStruct->mode)
    {
    case CAN_SELF_RX_MODE:
        canx->mode.bit.self_rx_mode = SET;
        break;
    case CAN_LOOPBACK_MODE:
        canx->mode.bit.loopback_mode = SET;
        break;
    case CAN_LISTEN_ONLY_MODE:
        canx->mode.bit.listen_only_mode = SET;
        break;
    default:
        break;
    }
    canx->mode.bit.reset_mode = RESET;
}

/**
 * @brief  Initializes the CAN peripheral according to the specified
 *         parameters in the CAN_FilterInitStruct.
 * @param  CANx:           where x can be CAN or CAN1 to to select the CAN
 *                         peripheral.
 * @param  CAN_FilterInitStruct: pointer to a CAN_FilterInitTypeDef
 *                               structure that contains the configuration
 *                               information.
 * @retval None.
 */

void CAN_FilterInit(CAN_Type *canx, CAN_FilterInitStruct *filterInitStruct)
{
    if(canx != CAN)
    {
        return;
    }
    uint32_t BASE = CAN_BASE + 0x90;

    canx->mode.bit.reset_mode = SET;
    canx->mode.bit.acp_filt_mode = SET;
    canx->filter_init.bit.filt_init = SET;

    canx->filter_mode.bit.filt_mode &= ~(0xf);
    canx->filter_scale.bit.filt_scale &= ~(0xf);
    canx->filter_active.bit.filt_active &= ~(0xf);

	for(uint8_t i = 0; i < 4; i++)
	{
        canx->filter_mode.bit.filt_mode |= (filterInitStruct->filterStruct[i].filterMode << i);
        canx->filter_scale.bit.filt_scale |= (filterInitStruct->filterStruct[i].filterScale << i);
        canx->filter_active.bit.filt_active |= (filterInitStruct->filterStruct[i].filterActivation << i);

        if(filterInitStruct->filterStruct[i].filterScale == CAN_FILTERSCALE_32BIT)
        {
            *(volatile uint32_t *)(BASE + 8 * i) = (uint32_t)(((filterInitStruct->filterStruct[i].filterIdHigh << 21) |
                (filterInitStruct->filterStruct[i].filterIdLow) << 3 | (filterInitStruct->filterStruct[i].filterIde << 2) |
                (filterInitStruct->filterStruct[i].filterRtr << 1)) & (0xfffffffe));
            *(volatile uint32_t *)(BASE + 4 + 8 * i) = (uint32_t)(((filterInitStruct->filterStruct[i].filterMaskIdHigh << 21) |
                (filterInitStruct->filterStruct[i].filterMaskIdLow) << 3| (filterInitStruct->filterStruct[i].filterIde << 2) |
                    (filterInitStruct->filterStruct[i].filterRtr << 1)) & (0xfffffffe));
        }
        else
        {
            *(volatile uint32_t *)(BASE + 8 * i) = (uint32_t)((((filterInitStruct->filterStruct[i].filterIdHigh << 5) | (filterInitStruct->filterStruct[i].filterIde << 3) |
                (filterInitStruct->filterStruct[i].filterRtr << 4)) & (0xfff8)) << 16) | (((filterInitStruct->filterStruct[i].filterIdLow << 5) |
                (filterInitStruct->filterStruct[i].filterIde << 3) | (filterInitStruct->filterStruct[i].filterRtr << 4)) & (0xfff8));
            *(volatile uint32_t *)(BASE + 4 + 8 * i) = (uint32_t)((((filterInitStruct->filterStruct[i].filterMaskIdHigh << 5)| (filterInitStruct->filterStruct[i].filterIde << 3) |
                (filterInitStruct->filterStruct[i].filterRtr << 4)) & (0xfff8)) << 16) | (((filterInitStruct->filterStruct[i].filterMaskIdLow << 5)|
                (filterInitStruct->filterStruct[i].filterIde << 3) |(filterInitStruct->filterStruct[i].filterRtr << 4)) & (0xfff8));
        }
    }
    canx->filter_init.bit.filt_init = RESET;
    canx->mode.bit.reset_mode = RESET;
}

/**
 * @brief  Checks whether the specified CAN flag is set or not.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @param  canFlag: specifies the flag to check.
 *                   This parameter can be one of the following flags:
 *                  CAN_FLAG_TX_BUSY
                    CAN_FLAG_TX_PEND
                    CAN_FLAG_RX_STU
                    CAN_FLAG_TX_STU
                    CAN_FLAG_TX_VAL
                    CAN_FLAG_RX_VAL
                    CAN_FLAG_SAM_BIT
                    CAN_FLAG_IDE_STU
                    CAN_FLAG_ERR_PASSIVE
                    CAN_FLAG_BUF_OFF
                    CAN_FLAG_ERR_FRAME
                    CAN_FLAG_OVERLOAD_FRAME
    * @retval The new state of CAN_FLAG(SET or RESET).*/
FlagStatus CAN_GetFlagStatus(CAN_Type *canx, uint32_t canFlag)
{
    FlagStatus bitstatus = RESET;
    if ((canx->status.reg & canFlag) != 0)
    {
        bitstatus = SET;
    }
    return bitstatus;
}

/*
 * @brief  Initiates the transmission of a message.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @param  TxMessage: pointer to a structure which contains CAN Id, CAN
 *                    DLC and CAN data.
 * @retval SET:send ok
 *
 */
uint8_t CAN_SendData(CAN_Type *canx, CAN_TxMsgStruct *txMessage)
{
    canx->mode.bit.reset_mode = RESET;
    if (txMessage->ide == RESET)
    {
        canx->tx_info_w0.bit.tx_ide = RESET;
        canx->tx_info_w0.bit.tx_id_28_18 = txMessage->stdId;
    }
    else
    {
        canx->tx_info_w0.bit.tx_ide = SET;
        canx->tx_info_w0.bit.tx_id_28_18 = (txMessage->extId >> 18) & 0x7ff;
        canx->tx_info_w0.bit.tx_id_17_0 = (txMessage->extId)  & 0x3ffff;
    }
    canx->tx_info_w0.bit.tx_rtr = txMessage->rtr;
    canx->tx_info_w1.bit.tx_dlc = txMessage->dlc;
    canx->tx_data_w0.reg = (((uint32_t)txMessage->data[3] << 24) |
                            ((uint32_t)txMessage->data[2] << 16) |
                            ((uint32_t)txMessage->data[1] << 8) |
                            ((uint32_t)txMessage->data[0]));

    canx->tx_data_w1.reg = (((uint32_t)txMessage->data[7] << 24) |
                            ((uint32_t)txMessage->data[6] << 16) |
                            ((uint32_t)txMessage->data[5] << 8) |
                            ((uint32_t)txMessage->data[4]));
    canx->command.bit.transmit_request = SET;
    uint32_t s_cnt = 0;
    while (CAN_GetFlagStatus(canx, CAN_FLAG_TX_BUSY) == SET)
    {
        s_cnt++;
        if (s_cnt > 0x3fff)
        {
            break;
        }
    }
    uint8_t err_status = CAN_GetFlagStatus(canx, CAN_FLAG_BUF_OFF) | CAN_GetFlagStatus(canx, CAN_FLAG_ERR_PASSIVE);
    return err_status;
}

/**
 * @brief  Returns the number of pending messages.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @retval SET:have data
 */
uint8_t CAN_MessagePending(CAN_Type *canx)
{
    if (canx->rx_items.reg != 0)
    {
        return SET;
    }
    return RESET;
}
/**
 * @brief  Receives a message.
 * @param  CANx: only CAN to select the CAN peripheral.
 * @param  RxMessage:  pointer to a structure receive message which contains
 *                     CAN Id, CAN DLC, CAN datas and FMI number.
 * @retval SET Rec ok.
 */
uint8_t CAN_ReceiveData(CAN_Type *canx, CAN_RxMsgStruct *rxMessage)
{
    if (CAN_MessagePending(canx) != SET)
    {
        return RESET;
    }

    rxMessage->ide = canx->rx_info_w0.bit.rx_ide;
    if (rxMessage->ide == CAN_ID_STD)
    {
        rxMessage->stdId = canx->rx_info_w0.bit.rx_id_28_18;
    }
    else
    {
        rxMessage->extId = canx->rx_info_w0.bit.rx_id_17_0 | (canx->rx_info_w0.bit.rx_id_28_18 << 18);
    }
    rxMessage->rtr = canx->rx_info_w0.bit.rx_rtr;
    rxMessage->dlc = canx->rx_info_w1.bit.rx_dlc;
    for (uint8_t i = 0; i < rxMessage->dlc; i++)
    {
        if(canx == CAN)
        {
            rxMessage->data[i] = *(volatile uint8_t *)(0x8c78 + i);
        }
    }
    canx->command.bit.release_rx_buff = SET;
    return SET;
}

/**
  * @brief  Enables or disables the specified CANx interrupts.
  * @param  CANx: only CAN to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt sources to be enabled or disabled.
  *   This parameter can be:
  *        -CAN_IT_TC,
  *        -CAN_IT_RxOver,
  *        -CAN_IT_RxFull,
  *        -CAN_IT_RxVld,
  *        -CAN_IT_RC,
  * @param  NewState: new state of the CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */

void CAN_SetIrq(CAN_Type* canx, uint32_t canIrq, FunctionalState newState)
{
    if(newState == ENABLE)
	{
        canx->irq_en.reg |= canIrq;
	}
	else
	{
        canx->irq_en.reg &= ~canIrq;
	}
}

/**
  * @brief  Checks whether the specified CANx interrupt has occurred or not.
  * @param  CANx: only CAN to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt source to check.
  *   This parameter can be one of the following flags:
  *        -CAN_IT_TC,
  *        -CAN_IT_RxOver,
  *        -CAN_IT_RxFull,
  *        -CAN_IT_RxVld,
  *        -CAN_IT_RC,
  * @retval The current  state of CAN_IT (SET or RESET).
  */
uint8_t CAN_GetIrqStatus(CAN_Type* canx, uint32_t canIrq)
{
	ITStatus itStatus = RESET;
    if ((canx->irq.reg & canIrq) != 0)
	{
        itStatus = canx->irq.reg & canIrq;
	}
	return itStatus;
}

/**
  * @brief  Checks whether the specified CANx interrupt has occurred or not.
  * @param  CANx: only CAN to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt source to check.
  *   This parameter can be one of the following flags:
  *        -CAN_IT_TC,
  *        -CAN_IT_RxOver,
  *        -CAN_IT_RxFull,
  *        -CAN_IT_RxVld,
  *        -CAN_IT_RC,
  *   */
void CAN_ClearIrq(CAN_Type* canx, uint32_t canIrq)
{
    if(canx->irq.reg & canIrq)
	{
        canx->irq.reg |= canIrq;
	}
}
/**
  * @brief  Enters the low power mode.
  * @param  CANx: only CAN to select the CAN peripheral.
  * @retval none
  */
void CAN_Sleep(CAN_Type* canx)
{
    canx->command.bit.sleep_mode |= (CAN_FLAG << 16);
}

/**
  * @brief  Wakes the CAN up.
  * @param  CANx: only CAN to select the CAN peripheral.
  * @retval none
  */
void CAN_WakeUp(CAN_Type* canx)
{
    canx->command.bit.sleep_mode &= (~(CAN_FLAG << 16));
}