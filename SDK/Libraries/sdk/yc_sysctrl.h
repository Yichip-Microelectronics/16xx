/*
File Name    : yc_sysctrl.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/01
Description  : sysctrl driver interface define.
*/

#ifndef _yc_SYSCTRL_H_
#define _yc_SYSCTRL_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define SYS_EVENT_SOFT_RESET            (1<<0)
#define SYS_EVENT_IO_RESET              (1<<1)
#define SYS_EVENT_WDT_RESET             (1<<2)
#define SYS_EVENT_CODE_CRC_ERR          (1<<4)  /*固件crc校验错误*/
#define SYS_EVENT_CPU_INST_ERR          (1<<5)  /*CPU指令异常*/
#define SYS_EVENT_CPU_DATA_ERR          (1<<6)  /*CPU取数异常*/
#define SYS_EVENT_VIO_ERR               (1<<7)  /*VIO低压*/
#define SYS_EVENT_ALL                   (0xFF)

typedef enum
{
    SYS_CLKSEL_RC = 0,  /* 24M */
    SYS_CLKSEL_DPLL,    /* 24M,48M,64M,96M,128M,192M,256M */
    SYS_CLKSEL_CRYSTAL, /* normal 24M, or the select crystal */
    SYS_CLKSEL_LPO,     /* 32768hz*/
} SYS_ClkEnum;
typedef enum
{
    DPLL_CLKSEL_24MHZ = 0,
    DPLL_CLKSEL_48MHZ,
    DPLL_CLKSEL_64MHZ,
    DPLL_CLKSEL_96MHZ,
    DPLL_CLKSEL_128MHZ,
    DPLL_CLKSEL_192MHZ,
    DPLL_CLKSEL_256MHZ
} DPLL_ClkEnum;

typedef enum
{
    CLK_CPU = 8,
    CLK_RAM,
    CLK_USB,
    CLK_UARTA,
    CLK_UARTB,
    CLK_AES,
    CLK_GDAC,
    CLK_BTDMA,
    CLK_SPI,
    CLK_IIC0,
    CLK_IIC1,
    CLK_MICDSM,
    CLK_MIC,
    CLK_ICE,
    CLK_NEC,
    CLK_UARTC,
    CLK_UARTD,
    CLK_LPM,
    CLK_QSPI,
    CLK_CAN
} CLK_EnEnum;

extern void delay(uint32_t cnt);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void SYSCTRL_ClkEn(CLK_EnEnum type,FunctionalState newState);
void SYSCTRL_WaitLpoEdge(int edgeNum);
void SYSCTRL_CrystalCtrl(FunctionalState state);
void SYSCTRL_DpllInit(void);
uint32_t SYSCTRL_GetMcuClk(void);
uint32_t SYSCTRL_GetSystemClk(void);
void SYSCTRL_ClkInit(SYS_ClkEnum sysClkSource, uint8_t sysClkDiv, DPLL_ClkEnum mcuDpllClkSel, DPLL_ClkEnum flashClkSel);
void SYSCTRL_ChipInit(void);
void SYSCTRL_SystemInit(void);
void SYSCTRL_SoftReset(void);
uint8_t SYSCTRL_GetSysEvent(void);
void SYSCTRL_ClearSysEvent(uint8_t event);
#ifdef __cplusplus
}
#endif

#endif //_yc_SYSCTRL_H_

/******************************************************************END OF FILE****/
