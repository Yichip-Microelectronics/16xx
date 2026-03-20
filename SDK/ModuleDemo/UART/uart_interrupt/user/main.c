#include "yc_plic.h"
#include "yc_uart.h"
#include "board_config.h"
#include "yc_vector.h"

#define VECTOR_UARTA        0

#define RX_IRQ_NUM          10
#define RX_TIMEOUT          0x1000 /* 16 bits, max:0xffff */

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("uart_interrupt demo");
    UART_SetRxNumIrq(PRINT_PORT,RX_IRQ_NUM);//设置接收到多少个字符触发RX中断,希望接收到固定长度时触发中断场景使用
    UART_SetRxTimeout(PRINT_PORT,RX_TIMEOUT);//设置RX字符超时间隔中断的超时时间,包长不确定且希望接收完整包数据触发中断场景使用
    UART_SetTxIrq(PRINT_PORT,ENABLE);
    #if VECTOR_UARTA
    VECTOR_EnableIRQ(VECTOR_UARTA_IRQn);
    #else
    PLIC_SetPriority(PRINT_IRQ_PORT,2);
    PLIC_EnableIRQ(PRINT_IRQ_PORT);
    #endif
    while(1)
    {

    }
}

#if VECTOR_UARTA
_interrupt void VECTOR_UARTA_IRQ(void)
#else
void UARTA_IRQHandler(void)
#endif
{
    uint8_t pRxBuf[256] = {0};
    uint16_t irq_status=UART_GetIrqStatus(PRINT_PORT);
    if(irq_status&UART_STATUS_RX_INTR)
    {
        yc_log("RX num irq[%dbytes]",RX_IRQ_NUM);
        UART_ReceiveBuff(PRINT_PORT,pRxBuf,RX_IRQ_NUM);
        UART_ClearRxNumIrq(PRINT_PORT);
    }
    if(irq_status&UART_STATUS_TIMEOUTINTR)
    {
        int rx_data_num=UART_GetRxCount(PRINT_PORT);
        yc_log("RX timeout irq[%dbytes]",rx_data_num);
        UART_ReceiveBuff(PRINT_PORT,pRxBuf,rx_data_num);
        UART_ClearRxTimeoutIrq(PRINT_PORT);
    }
    if(irq_status&UART_STATUS_TX_INTR)
    {
        yc_log("TX irq");
        while(UART_GetTxCount(PRINT_PORT)>0);//为避免打印"TX irq"日志导致再次触发TX中断,此处等待"TX irq"日志发送完成再清除TX中断
        UART_ClearTxIrq(PRINT_PORT);
    }
}
