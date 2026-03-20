#include "yc_log.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"
#include "yc_can.h"
#include "yc_sysctrl.h"

CAN_TxMsgStruct TxMessage = {0};
CAN_RxMsgStruct RxMessage = {0};
CAN_FilterInitStruct filterInitStruct = {0};
typedef enum
{
    CAN_FILTER0 = 0,
    CAN_FILTER1,
    CAN_FILTER2,
    CAN_FILTER3
}CAN_FilterEnum;
#define CAN_EXID            (0x01)
#define CAN_STDID           (0x00)
#define CAN_DATA            (0x00)
#define CAN_RTR             (0x01)
#define CAN_FLITER_MODE     (0)
#define CAN_INTR_MODE       (0)

void Can_SendMsgSet(void)
{
    TxMessage.extId = 0x00;                /* 扩展帧ID为29位，范围为0~0x1FFFFFFF */
    TxMessage.ide = CAN_EXID;           /* ide位的作用是确定是扩展帧(1)还是标准帧(0)*/
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
        Tbit = Tq+(bs1 + 1)*Tq+(bs2 + 1)*Tq
    */
    CAN_InitStruct.prescaler = 8;
    CAN_InitStruct.bs1 = 6;
    CAN_InitStruct.bs2 = 3;
    CAN_InitStruct.sjw = 2;
    CAN_InitStruct.sam = 0;
    CAN_InitStruct.mode = CAN_NORMAL;
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

void CAN_FilterConfig(uint8_t group, uint8_t ide,uint8_t filterMode,uint8_t rtr,FunctionalState filterActivation,uint8_t filterScale, uint16_t filterIdLow, uint16_t filterIdHigh, uint16_t filterMaskIdLow, uint16_t filterMaskIdHigh)
{

    filterInitStruct.filterStruct[group].filterIde = ide;
    filterInitStruct.filterStruct[group].filterMode = filterMode;
    filterInitStruct.filterStruct[group].filterRtr = rtr;
    filterInitStruct.filterStruct[group].filterScale = filterScale;
    filterInitStruct.filterStruct[group].filterActivation = filterActivation;
    filterInitStruct.filterStruct[group].filterIdLow = filterIdLow;
    filterInitStruct.filterStruct[group].filterIdHigh = filterIdHigh;
    filterInitStruct.filterStruct[group].filterMaskIdLow = filterMaskIdLow;
    filterInitStruct.filterStruct[group].filterMaskIdHigh = filterMaskIdHigh;


}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("can normal mode demo");
    Can_Confing();
    Can_SendMsgSet();
    GPIO_Config(KEY1_PIN,GPIO_MODE_PULL_UP);
    #if CAN_FLITER_MODE
    /* 过滤器的配置，32bit的列表模式下，只接受ID为0x12和0x34的两个标准帧 */
    CAN_FilterConfig(CAN_FILTER0, CAN_ID_STD, CAN_FILTERMODE_IDLIST, CAN_DATA, ENABLE, CAN_FILTERSCALE_32BIT, 0x0000, 0x0012, 0x0000, 0x0034);
    /* 过滤器的配置，32bit的列表模式下，只接受ID为0x7fc、0x7fd、0x7fe、0x7ff的四个标准帧 */
    CAN_FilterConfig(CAN_FILTER1, CAN_ID_STD, CAN_FILTERMODE_IDMASK, CAN_DATA, ENABLE, CAN_FILTERSCALE_32BIT, 0x0000, 0x07ff, 0x0000, 0xfffc);
    /* 过滤器的配置，16bit的列表模式下，只接受的四个标准帧 */
    CAN_FilterConfig(CAN_FILTER2, CAN_ID_STD, CAN_FILTERMODE_IDLIST, CAN_DATA, ENABLE, CAN_FILTERSCALE_16BIT, 0x0023, 0x0022, 0x0056, 0x0078);
    /* 过滤器的配置，16bit的掩码模式下，只接受ID为0x1a、0x1b、0x2a、0x2b的四个标准帧 */
    CAN_FilterConfig(CAN_FILTER3, CAN_ID_STD, CAN_FILTERMODE_IDMASK, CAN_DATA, ENABLE, CAN_FILTERSCALE_16BIT, 0x001a, 0x00fc, 0x002a, 0x00fc);

    CAN_FilterInit(CAN, &filterInitStruct);
    #endif
    #if CAN_INTR_MODE
    CAN_SetIrq(CAN, CAN_IT_RXVLD|CAN_IT_TC, ENABLE);
    PLIC_SetPriority(CAN_IRQn, 2);
    PLIC_EnableIRQ(CAN_IRQn);
    #endif

    while (1)
    {
        if(GPIO_ReadDataBit(KEY1_PIN)==0)
        {
                if( CAN_SendData(CAN,&TxMessage)==0)
            #if CAN_INTR_MODE == 0
                {
                    CAN_MsgInfoPrint(&TxMessage,NULL);
                    TxMessage.extId = (TxMessage.extId+1)&0X1FFFFFFF;
                }
            #endif
            delay_ms(10);
        }
        #if CAN_INTR_MODE == 0
        if(CAN_ReceiveData(CAN, &RxMessage))
        {
            CAN_MsgInfoPrint(NULL, &RxMessage);
            memset(&RxMessage, 0, sizeof(RxMessage));
        }
        #endif

    }
}

void CAN_IRQHandler(void)
{

    if(CAN_GetIrqStatus(CAN, CAN_IT_RXVLD))
    {
        yc_log("CAN Rx Fifo Not Empty Irqhandler!\n");
        if(CAN_ReceiveData(CAN, &RxMessage))
        {
            CAN_MsgInfoPrint(NULL, &RxMessage);
            memset(&RxMessage, 0, sizeof(RxMessage));
        }
        CAN_ClearIrq(CAN, CAN_IT_RXVLD);
    }

    if(CAN_GetIrqStatus(CAN, CAN_IT_TC))
    {
        yc_log("CAN Tx Complete Irqhandler!\n");
        CAN_MsgInfoPrint(&TxMessage, NULL);
        TxMessage.extId = (TxMessage.extId+1)&0X1FFFFFFF;
        CAN_ClearIrq(CAN, CAN_IT_TC);
    }
}