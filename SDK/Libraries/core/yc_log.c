/*
File Name    : yc_log.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/25
Description  : yc log interface implement.
*/

#include <stdarg.h>
#include "yc_log.h"

#define PRINT_UART_BUF_ZISE (512)
uint8_t print_uart_tx_buf[PRINT_UART_BUF_ZISE] = {0};
uint8_t print_uart_rx_buf[PRINT_UART_BUF_ZISE] = {0};
UART_InitStruct gUART_InitStruct;

void printport_init(void)
{
    UART_DeInit(PRINT_PORT);
    UART_InitStruct uartInitStruct;

    uartInitStruct.baudRate            = PRINT_BAUD;
    uartInitStruct.flowControl         = UART_HARDWARE_FLOW_CONTROL_NONE;
    uartInitStruct.stopBits            = UART_STOP_BIT_1;
    uartInitStruct.mode                = UART_MODE_DUPLEX;
    uartInitStruct.parity              = UART_PARITY_NONE;
    uartInitStruct.txBuffer            = (uint32_t)print_uart_tx_buf;
    uartInitStruct.txLen               = sizeof(print_uart_tx_buf);
    uartInitStruct.rxBuffer            = (uint32_t)print_uart_rx_buf;
    uartInitStruct.rxLen               = sizeof(print_uart_rx_buf);

    if (CORE_UARTA == PRINT_PORT)
    {
        GPIO_Config(PRINT_RX_IO_PIN, GPIO_MODE_UARTA_RXD);
        GPIO_Config(PRINT_TX_IO_PIN, GPIO_MODE_UARTA_TXD);
    }
    else if(CORE_UARTB == PRINT_PORT)
    {
        GPIO_Config(PRINT_RX_IO_PIN, GPIO_MODE_UARTB_RXD);
        GPIO_Config(PRINT_TX_IO_PIN, GPIO_MODE_UARTB_TXD);
    }
    else if(CORE_UARTC == PRINT_PORT)
    {
        GPIO_Config(PRINT_RX_IO_PIN, GPIO_MODE_UARTC);
        GPIO_Config(PRINT_TX_IO_PIN, GPIO_MODE_UARTC);
    }
    else
    {
        GPIO_Config(PRINT_RX_IO_PIN, GPIO_MODE_UARTD);
        GPIO_Config(PRINT_TX_IO_PIN, GPIO_MODE_UARTD);
    }
    #ifdef YC_LOG_SWITCH
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    #endif
    UART_Init(PRINT_PORT, &uartInitStruct);
}

#ifdef YC_LOG_SWITCH
_ramcode
int _write(int fd, char *ptr, int len)
{
    UART_SendBuff(PRINT_PORT, (uint8_t *)ptr, len);
    return len;
}
_ramcode
int _read(int fd, const void *buf, int count)
{
    int CharCnt = 0x00;
    while (UART_GetRxCount(PRINT_PORT) == 0);
    CharCnt = UART_GetRxCount(PRINT_PORT);
    UART_ReceiveBuff(PRINT_PORT, (uint8_t *)buf, CharCnt);
    return CharCnt;
}

_ramcode
void printv(char *s, uint8_t *buf, uint32_t len)
{
    int i;
    printf("%s:\r\n", s);
    for (i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
        if (((i + 1) % 16) == 0 && i != 0)
        {
            printf("\r\n");
        }
    }
    if ((i % 16) != 0)
    {
        printf("\r\n");
    }
}
_ramcode
void _assert_handler(const char *file, int line, const char *func)
{
    printf("Assert at file: %s line:%d func: %s\n ", file, line, func);
    while (1)
        ;
}
#else
void _assert_handler(void)
{
    SYSCTRL_SoftReset();
}
#endif
