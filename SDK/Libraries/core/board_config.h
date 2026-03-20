/*
File Name    : board_config.h
Author       : Yichip
Version      : V1.0
Date         : 2022/12/03
Description  : Development board peripheral interface definition
*/

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "yc_types.h"
#include "rom_api.h"
#include "register.h"
#include "yc_gpio.h"
#include "yc_log.h"
#include "yc_sysctrl.h"

/*Board define*/
#define BOARD_2020_022_1508FPGA_V_1_2       (0x000102030405)
#define BOARD_2025_223_YC1161AA_EVB_V_1_0   (0x0c100f0d0b0e)
#define BOARD_2025_298_YC1615AA_EVB_V_1_1   (0x0e0a08100f05)


#ifndef BOARD_TYPE
#define BOARD_TYPE                          BOARD_2025_223_YC1161AA_EVB_V_1_0
#endif

#if BOARD_TYPE==BOARD_2025_298_YC1615AA_EVB_V_1_1
#define PWR_USE_BUCK                        (1)
#endif

#ifndef SYS_CLK_SOURCE
#define SYS_CLK_SOURCE                      SYS_CLKSEL_DPLL
#endif

#ifndef SYS_CLK_DIV
#define SYS_CLK_DIV                         (0)
#endif

#ifndef NO_CRYSTAL
#define NO_CRYSTAL                          (0)
#endif

#ifndef PWR_USE_BUCK
#define PWR_USE_BUCK                        (0)
#endif

#ifndef MCU_DPLL_CLK_SEL
#define MCU_DPLL_CLK_SEL                    DPLL_CLKSEL_48MHZ
#endif

#ifndef FLASH_CLK_SEL
#define FLASH_CLK_SEL                       DPLL_CLKSEL_48MHZ
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if (BOARD_TYPE == BOARD_2020_022_1508FPGA_V_1_2)

#define RC_CLK_SOURCE                       (24000000) /*RC时钟定义*/
#define EXTERNAL_CRYSTAL                    (24000000) /*外部晶体时钟定义*/
#define EXTERNAL_CRYSTAL_MS                 (24000)
#define EXTERNAL_CRYSTAL_US                 (24)

/*Print Port*/
#define PRINT_PORT                          CORE_UARTA
#define PRINT_IRQ_PORT                      UARTA_IRQn
#define PRINT_BAUD                          (921600)
#define PRINT_RX_IO_PIN                     GPIO_9
#define PRINT_TX_IO_PIN                     GPIO_10

/*led*/
#define GPIO_LED_PIN                        GPIO_7

/*GPIO IRQ*/
#define GPIO_HIGH_IRQ_PIN                   GPIO_21
#define GPIO_LOW_IRQ_PIN                    GPIO_22
#define GPIO_RISING_IRQ_PIN                 GPIO_23
#define GPIO_FALL_IRQ_PIN                   GPIO_24
#define GPIO_RISING_FALL_IRQ_PIN            GPIO_25

#define GPIO_FREQUENCY_CAPTURE_PIN          GPIO_25

/*SPI*/
#define SPI_NCS_PIN                         GPIO_27
#define SPI_SCK_PIN                         GPIO_28
#define SPI_MOSI_PIN                        GPIO_29
#define SPI_MISO_PIN                        GPIO_30

/*QSPI*/
#define QSPI_PORT                           CORE_QSPI1
#define QSPI_IRQ_PORT                       QSPI1_IRQn
#define QSPI_VECTOR_PORT                    VECTOR_QSPI1_IRQn
#define QSPI_NCS_PIN                        GPIO_27
#define QSPI_SCK_PIN                        GPIO_28
#define QSPI_IO0_PIN                        GPIO_29
#define QSPI_IO1_PIN                        GPIO_30
#define QSPI_IO2_PIN                        GPIO_33
#define QSPI_IO3_PIN                        GPIO_34

/*IIC*/
#define IIC_PORT                            CORE_IIC
#define IIC_IRQ_PORT                        IIC0_IRQn
#define IIC_SCL_PIN                         GPIO_33
#define IIC_SDA_PIN                         GPIO_34

/*GPIO LONG PRESS RESET*/
#define GPIO_Long_Press_Reset_1             GPIO_20
// #define GPIO_Long_Press_Reset_2            GPIO_26
// #define GPIO_Long_Press_Reset_3            GPIO_31

/*PWM*/
#define GPIO_PWM0_PIN                       GPIO_24
#define GPIO_PWM1_PIN                       GPIO_25
#define GPIO_PWM2_PIN                       GPIO_26
#define GPIO_PWM3_PIN                       GPIO_27

/*DAC*/
#define GPIO_DAC_OUT_P_PIN                  GPIO_24
#define GPIO_DAC_OUT_N_PIN                  GPIO_25

/*PDM MIC*/
#define GPIO_PDM_CLK_PIN                    GPIO_22
#define GPIO_PDM_DATA_PIN                   GPIO_23

#define IIS_MCLK_OUT_PIN                    GPIO_21
#define IIS_BCLK_OUT_PIN                    GPIO_22
#define IIS_LRCLK_OUT_PIN                   GPIO_23
#define IIS_DOUT_PIN                        GPIO_24

#define IIS_BCLKIN_PIN                      GPIO_26
#define IIS_LRCLKIN_PIN                     GPIO_27
#define IIS_DIN_PIN                         GPIO_28

/* IR/NEC (send and receive)*/
#define NEC_PIN                             GPIO_8

/*key*/
#define KEY1_PIN                            GPIO_27

/*  YC7816 IO  */
#define YC7816_RST_IO                       GPIO_8
#define YC7816_DATA_IO                      GPIO_29
#define YC7816_CLK_IO                       GPIO_21
#define YC7816_DETECT_IO                    GPIO_24
#define YC7816_CARD_3V3                     GPIO_23
#define YC7816_CLK_TIM                      TIM6
// #define YC7816_ATR_START_TIM                TIM7
#define GPIO_7816_IRQ_PIN                   GPIO_24

/*  YC_ST7735S IO  */
#define YC_ST7735S_IO_RST                   GPIO_18
#define YC_ST7735S_IO_RS                    GPIO_19          //低电平：寄存器    高电平：数据
#define YC_ST7735S_IO_LED                   GPIO_20         //背光控制， 高电平点亮，低电平灭

#define YC_ST7735S_IO_CLK                   GPIO_35
#define YC_ST7735S_IO_SDI                   GPIO_36
#define YC_ST7735S_IO_CS                    GPIO_37




#elif(BOARD_TYPE == BOARD_2025_298_YC1615AA_EVB_V_1_1)

#define RC_CLK_SOURCE                       (24000000) /*RC时钟定义*/
#define EXTERNAL_CRYSTAL                    (24000000) /*外部晶体时钟定义*/
#define EXTERNAL_CRYSTAL_MS                 (24000)
#define EXTERNAL_CRYSTAL_US                 (24)

/*Print Port*/
#define PRINT_PORT                          CORE_UARTA
#define PRINT_IRQ_PORT                      UARTA_IRQn
#define PRINT_BAUD                          (921600)
#define PRINT_RX_IO_PIN                     GPIO_11
#define PRINT_TX_IO_PIN                     GPIO_4

/*led*/
#define GPIO_LED_PIN                        GPIO_22

/*GPIO IRQ*/
#define GPIO_HIGH_IRQ_PIN                   GPIO_0
#define GPIO_LOW_IRQ_PIN                    GPIO_1
#define GPIO_RISING_IRQ_PIN                 GPIO_2
#define GPIO_FALL_IRQ_PIN                   GPIO_3
#define GPIO_RISING_FALL_IRQ_PIN            GPIO_11

#define GPIO_FREQUENCY_CAPTURE_PIN          GPIO_11

/*SPI*/
#define SPI_NCS_PIN                         GPIO_24
#define SPI_SCK_PIN                         GPIO_20
#define SPI_MOSI_PIN                        GPIO_21
#define SPI_MISO_PIN                        GPIO_23

/*QSPI*/
#define QSPI_PORT                           CORE_QSPI1
#define QSPI_IRQ_PORT                       QSPI1_IRQn
#define QSPI_NCS_PIN                        GPIO_24
#define QSPI_SCK_PIN                        GPIO_20
#define QSPI_IO0_PIN                        GPIO_21
#define QSPI_IO1_PIN                        GPIO_23
#define QSPI_IO2_PIN                        GPIO_22
#define QSPI_IO3_PIN                        GPIO_25

/*IIC*/
#define IIC_PORT                            CORE_IIC
#define IIC_IRQ_PORT                        IIC0_IRQn
#define IIC_SCL_PIN                         GPIO_30
#define IIC_SDA_PIN                         GPIO_29

/*GPIO LONG PRESS RESET*/
#define GPIO_Long_Press_Reset_1             GPIO_20
// #define GPIO_Long_Press_Reset_2            GPIO_26
// #define GPIO_Long_Press_Reset_3            GPIO_31

/*PWM*/
#define GPIO_PWM0_PIN                       GPIO_24
#define GPIO_PWM1_PIN                       GPIO_25
#define GPIO_PWM2_PIN                       GPIO_26
#define GPIO_PWM3_PIN                       GPIO_27


/*DAC*/
#define GPIO_DAC_OUT_P_PIN                  GPIO_30
#define GPIO_DAC_OUT_N_PIN                  GPIO_29

/*PDM MIC*/
#define GPIO_PDM_CLK_PIN                    GPIO_22
#define GPIO_PDM_DATA_PIN                   GPIO_23

/*ISO7816*/
#define UART_7816_RST_IO                    GPIO_34
#define UART_7816_DATA_IO                   GPIO_29
#define UART_7816_CLK_IO                    GPIO_30

#define UART_7816_DETECT_IO                 GPIO_32
#define UART_7816_CARD_3V3                  GPIO_23
#define UART_7816_CLK_TIM                   TIM6
#define UART_7816_BAUD                      (12800)


/*  YC_ST7735S IO  */
#define YC_ST7735S_IO_RST                   GPIO_18
#define YC_ST7735S_IO_RS                    GPIO_19          //低电平：寄存器    高电平：数据
#define YC_ST7735S_IO_LED                   GPIO_20         //背光控制， 高电平点亮，低电平灭
#define YC_ST7735S_IO_CLK                   GPIO_35
#define YC_ST7735S_IO_SDI                   GPIO_36
#define YC_ST7735S_IO_CS                    GPIO_37

/* IR/NEC (send and receive)*/
#define NEC_PIN                             GPIO_5

/*key*/
#define KEY1_PIN                            GPIO_29

#elif(BOARD_TYPE == BOARD_2025_223_YC1161AA_EVB_V_1_0)
#define RC_CLK_SOURCE                       (24000000) /*RC时钟定义*/
#define EXTERNAL_CRYSTAL                    (24000000) /*外部晶体时钟定义*/
#define EXTERNAL_CRYSTAL_MS                 (24000)
#define EXTERNAL_CRYSTAL_US                 (24)

/*Print Port*/
#define PRINT_PORT                          CORE_UARTA
#define PRINT_IRQ_PORT                      UARTA_IRQn
#define PRINT_BAUD                          (921600)
#define PRINT_RX_IO_PIN                     GPIO_5
#define PRINT_TX_IO_PIN                     GPIO_4

/*led*/
#define GPIO_LED_PIN                        GPIO_24

/*GPIO IRQ*/
#define GPIO_HIGH_IRQ_PIN                   GPIO_0
#define GPIO_LOW_IRQ_PIN                    GPIO_1
#define GPIO_RISING_IRQ_PIN                 GPIO_2
#define GPIO_FALL_IRQ_PIN                   GPIO_3
#define GPIO_RISING_FALL_IRQ_PIN            GPIO_4

#define GPIO_FREQUENCY_CAPTURE_PIN          GPIO_4

/*SPI*/
#define SPI_NCS_PIN                         GPIO_22
#define SPI_SCK_PIN                         GPIO_35
#define SPI_MOSI_PIN                        GPIO_26
#define SPI_MISO_PIN                        GPIO_21

/*QSPI*/
#define QSPI_PORT                           CORE_QSPI1
#define QSPI_IRQ_PORT                       QSPI1_IRQn
#define QSPI_NCS_PIN                        GPIO_22
#define QSPI_SCK_PIN                        GPIO_35
#define QSPI_IO0_PIN                        GPIO_26
#define QSPI_IO1_PIN                        GPIO_21
#define QSPI_IO2_PIN                        GPIO_33
#define QSPI_IO3_PIN                        GPIO_28

/*IIC*/
#define IIC_PORT                            CORE_IIC
#define IIC_IRQ_PORT                        IIC0_IRQn
#define IIC_SCL_PIN                         GPIO_36
#define IIC_SDA_PIN                         GPIO_30

/*GPIO LONG PRESS RESET*/
#define GPIO_Long_Press_Reset_1             GPIO_20
// #define GPIO_Long_Press_Reset_2            GPIO_26
// #define GPIO_Long_Press_Reset_3            GPIO_31

/*PWM*/
#define GPIO_PWM0_PIN                       GPIO_24
#define GPIO_PWM1_PIN                       GPIO_25
#define GPIO_PWM2_PIN                       GPIO_26
#define GPIO_PWM3_PIN                       GPIO_27


/*DAC*/
#define GPIO_DAC_OUT_P_PIN                  GPIO_24
#define GPIO_DAC_OUT_N_PIN                  GPIO_25

/*PDM MIC*/
#define GPIO_PDM_CLK_PIN                    GPIO_22
#define GPIO_PDM_DATA_PIN                   GPIO_23

/*ISO7816*/
#define UART_7816_RST_IO                    GPIO_34
#define UART_7816_DATA_IO                   GPIO_29
#define UART_7816_CLK_IO                    GPIO_30

#define UART_7816_DETECT_IO                 GPIO_32
#define UART_7816_CARD_3V3                  GPIO_23
#define UART_7816_CLK_TIM                   TIM6
#define UART_7816_BAUD                      (12800)


/*  YC_ST7735S IO  */
#define YC_ST7735S_IO_RST                   GPIO_18
#define YC_ST7735S_IO_RS                    GPIO_19          //低电平：寄存器    高电平：数据
#define YC_ST7735S_IO_LED                   GPIO_20         //背光控制， 高电平点亮，低电平灭
#define YC_ST7735S_IO_CLK                   GPIO_35
#define YC_ST7735S_IO_SDI                   GPIO_36
#define YC_ST7735S_IO_CS                    GPIO_37

/* IR/NEC (send and receive)*/
#define NEC_PIN                             GPIO_9

/*key*/
#define KEY1_PIN                            GPIO_27

#endif

#ifdef __cplusplus
}
#endif

#endif
