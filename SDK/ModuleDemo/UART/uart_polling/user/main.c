#include "yc_uart.h"

int main(void)
{
    SYSCTRL_ChipInit();

    printport_init();

    yc_log("uart_polling demo");
    while(1)
    {
        if(UART_GetRxCount(PRINT_PORT)>0)
        {
            UART_SendData(PRINT_PORT,UART_ReceiveData(PRINT_PORT));
        }
    }
}

