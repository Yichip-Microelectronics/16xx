/*
File Name    : yc_gpio.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/28
Description  : gpio driver interface define.
*/

#ifndef __YC_GPIO_H_
#define __YC_GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include "board_config.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define GPIO_NUM            (38)
/**
 * @brief Configuration Mode enumeration
 *
 */
typedef enum
{
    GPIO_MODE_INPUT         = 0x00, /* 浮空输入 */
    GPIO_MODE_QSPI_NCS      = 0x02,
    GPIO_MODE_QSPI_SCK      = 0x03,
    GPIO_MODE_QSPI_IO0      = 0x04,
    GPIO_MODE_QSPI_IO1      = 0x05,
    GPIO_MODE_QSPI_IO2      = 0x06,
    GPIO_MODE_QSPI_IO3      = 0x07,
    GPIO_MODE_UARTA_TXD     = 0x08,
    GPIO_MODE_UARTA_RXD     = 0x09,
    GPIO_MODE_UARTA_RTS     = 0x0a,
    GPIO_MODE_UARTA_CTS     = 0x0b,
    GPIO_MODE_UARTB_TXD     = 0x0c,
    GPIO_MODE_UARTB_RXD     = 0x0d,
    GPIO_MODE_UARTB_RTS     = 0x0e,
    GPIO_MODE_UARTB_CTS     = 0x0f,
    GPIO_MODE_PWM_OUT       = 0x10, /* 从GPIO0开始，每8个IO为一组，分别对应PWM0-PWM7 */
    GPIO_MODE_NEC_REC       = 0x11,
    GPIO_MODE_QSPI1_NCS     = 0x12,
    GPIO_MODE_QSPI1_SCK     = 0x13,
    GPIO_MODE_QSPI1_IO0     = 0x14,
    GPIO_MODE_QSPI1_IO1     = 0x15,
    GPIO_MODE_QSPI1_IO2     = 0x16,
    GPIO_MODE_QSPI1_IO3     = 0x17,
    GPIO_MODE_NEC_SEND      = 0x18,
    GPIO_MODE_DAC_OUT       = 0x1f, /* 从GPIO0开始，每2个IO为一组，分别对应p和n */
    GPIO_MODE_PWM0_1_OUT    = 0x20, /* 从GPIO0开始，每2个IO为一组，分别对应PWM0-PWM1 */
    GPIO_MODE_PWM0_3_OUT    = 0x21, /* 从GPIO0开始，每4个IO为一组，分别对应PWM0-PWM3 */
    GPIO_MODE_SPID_MOSI     = 0x22,
    GPIO_MODE_SPID_MISO     = 0x23,
    GPIO_MODE_SPID_NCS      = 0x24,
    GPIO_MODE_SPID_SCK      = 0x25,
    GPIO_MODE_UARTC         = 0x29, /* 从GPIO0开始，每4个IO为一组，分别对应UART_TX,UART_RX,UART_RTS,UART_CTS*/
    GPIO_MODE_UARTD         = 0x2a, /* 从GPIO0开始，每4个IO为一组，分别对应UART_TX,UART_RX,UART_RTS,UART_CTS*/
    GPIO_MODE_IIC0_SCL      = 0x2c,
    GPIO_MODE_IIC0_SDA      = 0x2d,
    GPIO_MODE_AUDIO_CLK     = 0x31,
    GPIO_MODE_CAN_RX        = 0x32,
    GPIO_MODE_CAN_TX        = 0x33,
    GPIO_MODE_MICCLK_PDM    = 0x34,
    GPIO_MODE_MICDAT_PDM    = 0x35,
    GPIO_MODE_QSPI2_NCS     = 0x36,
    GPIO_MODE_QSPI2_SCK     = 0x37,
    GPIO_MODE_QSPI2_IO0     = 0x38,
    GPIO_MODE_QSPI2_IO1     = 0x39,
    GPIO_MODE_QSPI2_IO2     = 0x3a,
    GPIO_MODE_QSPI2_IO3     = 0x3b,
    GPIO_MODE_IIC1_SCL      = 0x3c,
    GPIO_MODE_IIC1_SDA      = 0x3d,
    GPIO_MODE_OUT_LOW       = 0x3e,
    GPIO_MODE_OUT_HIGH      = 0x3f,
    GPIO_MODE_PULL_UP       = 0x40,
    GPIO_MODE_PULL_DOWN     = 0x80,
    GPIO_MODE_HI_Z          = 0xc0  /* gpio高阻态 */
}GPIO_ModeEnum;

/**
 * @brief gpio long press reset time config
 *
 */
typedef enum
{
    LONG_PRESS_TIMEOUT_64S = 0,
    LONG_PRESS_TIMEOUT_56S,
    LONG_PRESS_TIMEOUT_48S,
    LONG_PRESS_TIMEOUT_40S,
    LONG_PRESS_TIMEOUT_32S,
    LONG_PRESS_TIMEOUT_24S,
    LONG_PRESS_TIMEOUT_16S,
    LONG_PRESS_TIMEOUT_8S
}GPIO_RstTimeEnum;

#define IS_GPIO_LONGPRESS_TIMEOUT(LONGPRESS_TIMEOUT)    (((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_64S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_56S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_48S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_40S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_32S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_24S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_16S) || \
                                                        ((LONGPRESS_TIMEOUT) == LONG_PRESS_TIMEOUT_8S))

/**
 * @brief gpio long press reset select
 *
 */
typedef enum
{
    GPIO_20_LONG_PRESS_RESET = 2,
    GPIO_26_LONG_PRESS_RESET,
    GPIO_31_LONG_PRESS_RESET
}GPIO_RstNumEnum;

#define IS_GPIO_LONGPRESS_RESET(LONGPRESS_RESET)        (((LONGPRESS_RESET) == GPIO_20_LONG_PRESS_RESET) || \
                                                        ((LONGPRESS_RESET) == GPIO_26_LONG_PRESS_RESET) || \
                                                        ((LONGPRESS_RESET) == GPIO_31_LONG_PRESS_RESET))

/**
 * @brief gpio interrupt type
 *
 */
typedef enum
{
    HIGH_INTERRUPT = 0,
    LOW_INTERRUPT,
    RISING_INTERRUPT,
    FALL_INTERRUPT
}GPIO_IrqEnum;

#define IS_INTERRUPT(intr_type)                         (((intr_type) == LOW_INTERRUPT) || \
                                                        ((intr_type) == HIGH_INTERRUPT) || \
                                                        ((intr_type) == RISING_INTERRUPT) || \
                                                        ((intr_type) == FALL_INTERRUPT))


typedef enum
{
    GPIO_0 = 0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
    GPIO_8,
    GPIO_9,
    GPIO_10,
    GPIO_11,
    GPIO_12,
    GPIO_13,
    GPIO_14,
    GPIO_15,
    GPIO_16,
    GPIO_17,
    GPIO_18,
    GPIO_19,
    GPIO_20,
    GPIO_21,
    GPIO_22,
    GPIO_23,
    GPIO_24,
    GPIO_25,
    GPIO_26,
    GPIO_27,
    GPIO_28,
    GPIO_29,
    GPIO_30,
    GPIO_31,
    GPIO_32,
    GPIO_33,
    GPIO_34,
    GPIO_35,
    GPIO_36,
    GPIO_37
}GPIO_NumEnum;

#define IS_GPIO_PIN(PIN)                                (((PIN) >= 0) && ((PIN < GPIO_NUM)))

typedef enum
{
    GPIOA = 0,
    GPIOB = 1,
    GPIOC = 2,
    GPIOD = 3,
    GPIOE = 4
} GPIO_PortEnum; /* 8 IO of each group */

#define IS_GPIO_PORT(port)                              (port <= GPIOE)

/**
 * @brief gpio wakeup mode enum
 *
 */
typedef enum
{
    SLEEP_PULLUP = 1,
    SLEEP_LOWWAKE,
    SLEEP_PULLUP_LOWWAKE,
    SLEEP_HIGHWAKE,
    SLEEP_PULLDOWN_HIGHWAKE,
    SLEEP_PULLUP_QDECWAKE,
    SLEEP_TKEYBOARD_PRESSWAKE
} GPIO_WakeUpEnum;

void GPIO_DeInit(void);
void GPIO_Config(GPIO_NumEnum gpiox, GPIO_ModeEnum func);
void GPIO_AnalogEn(GPIO_NumEnum gpiox, FunctionalState newState);
void GPIO_DsConfig(GPIO_NumEnum gpiox, FunctionalState newState);
uint8_t GPIO_ReadDataBit(GPIO_NumEnum gpiox);
uint8_t GPIO_ReadData(GPIO_PortEnum portx);
uint8_t GPIO_GetIrqStatus(GPIO_NumEnum gpiox);
uint8_t GPIO_GetPortIrqStatus(GPIO_PortEnum portx);
void GPIO_SetIrqMode(GPIO_NumEnum gpiox, GPIO_IrqEnum type);
void GPIO_SetIrq(GPIO_NumEnum gpiox, FunctionalState newState);
void GPIO_ClearIrq(GPIO_NumEnum gpiox);
void GPIO_SetLongPressReset(GPIO_RstNumEnum GPIOx, FunctionalState newState);
void GPIO_SetLongPressResetTimeout(GPIO_RstTimeEnum time);
uint32_t GPIO_Capture(GPIO_NumEnum gpiox,uint32_t minFrequency);
void GPIO_WriteBit(GPIO_NumEnum gpiox, BitAction status);
void GPIO_SetWakeupType(GPIO_NumEnum gpiox, GPIO_WakeUpEnum wakeType);


#ifdef __cplusplus
}
#endif

#endif  /* __YC_GPIO_H_ */

/**
  * @}
  */


/******************************************************************END OF FILE****/
