/*
 * Copyright (c) 2006-2020, YICHIP Development Team
 * @file     main.c
 * @brief    CAN configuration
 * Change Logs:
 * Date           Author             Version        Notes
 * 2023-06-13     dengzhiqian        V1.0.0         the first version
 */

/* Includes ------------------------------------------------------------------*/
/*
 *@Note
 本例程演示CAN内部回环测试
 接收数据线与CAN总线连接断开，并直接连接至发送数据线
*/
#include "yc_log.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"
#include "yc_can.h"
#include "yc_sysctrl.h"

CAN_TxMsgStruct TxMessage = {0};
CAN_RxMsgStruct RxMessage = {0};

#define CAN_EXID  (0x01)
#define CAN_STDID (0x00)

void Can_SendMsgSet(void)
{
    TxMessage.extId = CAN_EXID;
    TxMessage.ide = 0x01;
    TxMessage.rtr = CAN_RTR_DATA;
    TxMessage.dlc = 0x08;
    for (uint8_t i = 0; i < TxMessage.dlc; i++)
        TxMessage.data[i] = '0' + i;
}

void Can_Confing()
{
    /*enable can/can1 clk*/
    SYSCTRL_ClkEn(CLK_CAN, ENABLE);

    /*gpio set*/
    GPIO_Config(GPIO_21, GPIO_MODE_CAN_RX);
    GPIO_Config(GPIO_22, GPIO_MODE_CAN_TX);

    CAN_InitStruct CAN_InitStruct;
    /*
        Tq= Tcan*(CAN_Prescaler+2)
        Tbit = Tq+(CAN_BS1+1)*Tq+(CAN_BS2+1)*Tq
    */
    CAN_InitStruct.prescaler = 8;
    CAN_InitStruct.bs1 = 6;
    CAN_InitStruct.bs2 = 3;
    CAN_InitStruct.sjw = 2;
    CAN_InitStruct.sam = 0;
    CAN_InitStruct.mode = CAN_LOOPBACK_MODE;
    CAN_Init(CAN, &CAN_InitStruct);
}

void CAN_MsgInfoPrint(CAN_TxMsgStruct *txMsg, CAN_RxMsgStruct *rxMsg)
{
    if (txMsg != NULL)
    {
        yc_log("[TxMag]:\r\n");
        yc_log("IDE    :%s\r\n", txMsg->ide == CAN_ID_STD ? "CAN_ID_STD" : "CAN_ID_EXT");
        yc_log("RTR    :%s\r\n", txMsg->rtr == CAN_RTR_DATA ? "CAN_RTR_DATA" : "CAN_RTR_REMOTE");
        yc_log("DLC    :%ld\r\n", txMsg->dlc);
        if(txMsg->ide)
        {
            yc_log("ExtId    :0x%x\r\n", txMsg->extId);
        }
        else
        {
            yc_log("StdId : 0x%x\r\n", txMsg->stdId);
        }
        yc_log_buf("Data    ", (uint8_t *)txMsg->data, txMsg->dlc);
    }
    if (rxMsg != NULL)
    {
        yc_log("[RxMag]:\r\n");
        yc_log("IDE : %s\r\n", rxMsg->ide == CAN_ID_STD ? "CAN_ID_STD" : "CAN_ID_EXT");
        yc_log("RTR : %s\r\n", rxMsg->rtr == CAN_RTR_DATA ? "CAN_RTR_DATA" : "CAN_RTR_REMOTE");
        yc_log("DLC : %ld\r\n", rxMsg->dlc);
        if(rxMsg->ide)
        {
            yc_log("ExtId : 0x%x\r\n", rxMsg->extId);
        }
        else
        {
            yc_log("StdId : 0x%x\r\n", rxMsg->stdId);
        }
        yc_log_buf("Data", (uint8_t *)rxMsg->data, rxMsg->dlc);
    }
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("can loopback demo.");
    Can_Confing();
    Can_SendMsgSet();
    while (1)
    {
        CAN_SendData(CAN, &TxMessage);
        CAN_MsgInfoPrint(&TxMessage, NULL);
        for(uint8_t i = 0; i < 8; i++)
        {
            TxMessage.data[i] = TxMessage.data[i] + 1;
        }

        if(TxMessage.data[7] == 127)
        {
            for (uint8_t i = 0; i < TxMessage.dlc; i++)
                TxMessage.data[i] = '0' + i;
        }
        delay_ms(1000);

        if(CAN_ReceiveData(CAN, &RxMessage))
        {
            CAN_MsgInfoPrint(NULL, &RxMessage);
        }
    }
}