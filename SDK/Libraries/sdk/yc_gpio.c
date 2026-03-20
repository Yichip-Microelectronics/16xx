/*
File Name    : yc_gpio.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/28
Description  : gpio driver interface implement.
*/


/* Includes ------------------------------------------------------------------*/


#include "yc_gpio.h"
#include "yc_log.h"
#include "math.h"
#include "yc_sysctrl.h"

#define CAPTURE_TIME 10 /* 捕获10ms */
/**
 * @method GPIO_DeInit
 * @brief  config all gpio into floating_input mode
 * @param  none
 * @retval none
 */
void GPIO_DeInit(void)
{
    for (uint8_t i = 0; i < GPIO_NUM; i++)
    {
        CORE_GPIO->gpio_cfg[i] = GPIO_MODE_HI_Z;
    }
}

/**
 * @method GPIO_Config
 * @brief  config gpio mode
 * @param  gpiox:pin num
 * @param  func:Parameter values are GPIO_PortEnum enumeration members
 *                  根据硬件需求GPIO_MODE_PULL_DOWN、GPIO_MODE_PULL_UP可以和某些功能组合传参,例如:
 *                  下拉输入:(GPIO_MODE_PULL_DOWN|GPIO_MODE_INPUT)
 *                  上拉输入:(GPIO_MODE_PULL_UP|GPIO_MODE_INPUT)
 *                  串口RX加内部上拉:(GPIO_MODE_PULL_UP|UART_RXD)
 *                  IIC加内部上拉:(GPIO_MODE_PULL_UP|GPIO_MODE_IIC0_SCL)、(GPIO_MODE_PULL_UP|GPIO_MODE_IIC0_SDA)
 * @retval none
 */
_ramcode void GPIO_Config(GPIO_NumEnum gpiox, GPIO_ModeEnum func)
{

    CORE_GPIO->gpio_cfg[gpiox] = func;
}

/**
 * @method GPIO_AnalogEn
 * @brief  enable or disable gpio analog input
 * @param  gpiox:pin num
 * @param  newState: enable or disable
 * @retval none
 */
void GPIO_AnalogEn(GPIO_NumEnum gpiox, FunctionalState newState)
{
    if (newState != DISABLE)
    {
        CORE_GPIO->gpio_ae[gpiox/8] |= 1 << (gpiox % 8);
    }
    else
    {
        CORE_GPIO->gpio_ae[gpiox/8] &= ~(1 << (gpiox % 8));
    }
}

/**
 * @method GPIO_DsConfig
 * @brief  gpio drive strength select
 * @param  gpiox:pin num
 * @param  newState: enable:Strong drive strength; disable:weak drive strength
 * @retval none
 */
void GPIO_DsConfig(GPIO_NumEnum gpiox, FunctionalState newState)
{
    if (newState != DISABLE)
    {
        CORE_GPIO->gpio_ds[gpiox/8] |= 1 << (gpiox % 8);
    }
    else
    {
        CORE_GPIO->gpio_ds[gpiox/8] &= ~(1 << (gpiox % 8));
    }
}

/**
 * @method GPIO_ReadDataBit
 * @brief  read one gpio input level
 * @param  gpiox:pin num
 * @retval input level:0 is low level,1 is high level
 */
uint8_t GPIO_ReadDataBit(GPIO_NumEnum gpiox)
{
    _ASSERT(IS_GPIO_PIN(gpiox));

    if ((CORE_GPIO->gpio_in[gpiox/8] & (1 << (gpiox % 8))) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @method GPIO_ReadData
 * @brief  read GPIO_Port input level
 * @param  portx: where x can be (GPIOA...GPIOE) to select the GPIO group.
 * @retval GPIO_Port input data.
 */
uint8_t GPIO_ReadData(GPIO_PortEnum portx)
{
    _ASSERT(IS_GPIO_PORT(portx));
    return CORE_GPIO->gpio_in[portx];
}


/**
 * @method GPIO_GetIrqStatus
 * @brief  get gpio interrupt status
 * @param  gpiox:pin num
 * @retval interrupt status
 */
uint8_t GPIO_GetIrqStatus(GPIO_NumEnum gpiox)
{
    _ASSERT(IS_GPIO_PIN(gpiox));
    if ((CORE_GPIO_INT->gpio_isr[gpiox/8] & (1 << (gpiox % 8))) != 0)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/**
 * @method GPIO_GetPortIrqStatus
 * @brief  get GPIO_Port interrupt status
 * @param  portx: GPIO port from GPIO_PortEnum
 * @retval GPIO_Port interrupt status,GPIOA:GPIO0-GPIO7;
 *                                    GPIOB:GPIO8-GPIO15;
 *                                    .....
 *                                    0 is no interrupt,1 is interrupt
 */
uint8_t GPIO_GetPortIrqStatus(GPIO_PortEnum portx)
{
    _ASSERT(IS_GPIO_PORT(portx));

    return CORE_GPIO_INT->gpio_isr[portx];
}

/**
 * @method GPIO_SetIrqMode
 * @brief  set gpio interrupt mode
 * @param  gpiox:pin num
 * @param  type:GPIO_IntrTypeDef
 * @retval none
 */
void GPIO_SetIrqMode(GPIO_NumEnum gpiox, GPIO_IrqEnum type)
{
    _ASSERT(IS_GPIO_PIN(gpiox));
    _ASSERT(IS_INTERRUPT(type));
    uint8_t int_mode=CORE_GPIO_INT->gpio_ts[gpiox / 4];
    int_mode &= ~(3 << ((gpiox % 4) * 2));
    int_mode |= (type << ((gpiox % 4) * 2));
    CORE_GPIO_INT->gpio_ts[gpiox / 4]=int_mode;
}

/**
 * @method GPIO_SetIrq
 * @brief  gpio interrupt en ctrl
 * @param  gpiox:pin num
 * @param  newState:FunctionalState
 * @retval none
 */
void GPIO_SetIrq(GPIO_NumEnum gpiox, FunctionalState newState)
{
    _ASSERT(IS_GPIO_PIN(gpiox));

    if (newState)
    {
        uint32_t sys_irq_status = __disable_irq();
        CORE_GPIO_INT->gpio_ien[gpiox/8] |= (1 << (gpiox % 8));
        CORE_GPIO_INT->gpio_ien[gpiox/8] &= ~(1 << (gpiox % 8));
        CORE_GPIO_INT->gpio_ien[gpiox/8] |= (1 << (gpiox % 8));
        __enable_irq(sys_irq_status);
    }
    else
    {
        CORE_GPIO_INT->gpio_ien[gpiox/8] &= ~(1 << (gpiox % 8));
    }
}

/**
 * @method GPIO_ClearIrq
 * @brief  clear gpio interrupt
 * @param  gpiox:pin num
 * @retval none
 */
void GPIO_ClearIrq(GPIO_NumEnum gpiox)
{
    _ASSERT(IS_GPIO_PIN(gpiox));
    CORE_GPIO_INT->gpio_ien[gpiox/8] &= ~(1 << (gpiox % 8));
    CORE_GPIO_INT->gpio_ien[gpiox/8] |= (1 << (gpiox % 8));
}

/**
 * @method GPIO_SetLongPressReset
 * @brief  gpio long press reset en ctrl
 * @param  GPIOx:long press reset source
 * @param  newState:FunctionalState
 * @retval none
 */
void GPIO_SetLongPressReset(GPIO_RstNumEnum GPIOx, FunctionalState newState)
{
    if (newState == ENABLE)
    {
        LPM_CONTROL->ice_mode.reg |= (1 << 3);
        CORE_CHIP_SYS->io_rst_src_en.reg |= (1 << GPIOx);
    }
    else
    {
        CORE_CHIP_SYS->io_rst_src_en.reg &= ~(1 << GPIOx);
    }
}

/**
 * @method GPIO_SetLongPressResetTimeOut
 * @brief  gpio long press reset time config
 * @param  time:GPIO_LongPress_TimeOut
 * @retval none
 */
void GPIO_SetLongPressResetTimeout(GPIO_RstTimeEnum time)
{
    if (IS_GPIO_LONGPRESS_TIMEOUT(time))
    {
        CORE_CHIP_SYS->io_rst_wdat.bit.io_rst_wdat = time;
    }
}


/**
 * @brief  pretest gpio cature count value
 * @param  minFrequency:minimum frequency unit:hz
 * @retval gpi_cali_cnt:input one clock cycle, system clk running count value
 */
int32_t GPIO_CapturePretest(GPIO_NumEnum gpiox,uint32_t minFrequency)
{
    uint32_t            pretest_overtime = 0;
    uint32_t            pretest_timeout  = (uint32_t)((double)SYSCTRL_GetMcuClk() / (double)minFrequency + 0.5F);
    if (pretest_timeout > (0x7fffffff - 1000000))
    {
        return -1;
    }
    CORE_GPI_CALI->gpi_counter_cali.bit.gpi_counter_cali = 0;
    CORE_GPI_CALI->gpi_counter_cali.bit.gpi_cali_start   = 1;
    CORE_GPI_CALI->gpi_cali_sel.bit.gpi_cali_sel         = gpiox;
    while (!(CORE_GPI_CALI->gpi_cali_done.bit.gpi_cali_done))
    {
        pretest_overtime += 3;
        if (pretest_overtime > pretest_timeout)
        {
            return -1;
        }
    }
    return CORE_GPI_CALI->gpi_cali_done.bit.gpi_cali_cnt;
}

/**
 * @brief  Calculate the clock frequency of the actual input
 * @param  minFrequency:minimum frequency, unit:hz
 * @retval gpio_capture_count:the actual input clock frequency
 */
uint32_t GPIO_Capture(GPIO_NumEnum gpiox,uint32_t minFrequency)
{
    uint32_t            pretest_cnt           = 0;
    uint32_t            gpi_count             = 1;
    uint32_t            gpi_count_10ms        = 0;
    int32_t             gpi_cali              = 0;
    uint32_t            gpio_capture_count    = 0;
    uint32_t            capture_overtime      = 0;
    uint32_t            capture_timeout_count = 0;
    pretest_cnt                               = GPIO_CapturePretest(gpiox,minFrequency);
    if (pretest_cnt == -1)
    {
        return 0;
    }

    gpi_count_10ms = (((SYSCTRL_GetSystemClk() / 1000) * CAPTURE_TIME) / pretest_cnt);
    if (gpi_count_10ms > 1)
    {
        capture_timeout_count = (SYSCTRL_GetMcuClk() / 1000) * (2 * CAPTURE_TIME);
        gpi_cali              = (uint32_t)((log2f((double)gpi_count_10ms) + 0.5) / 1);
        if (gpi_cali > 15)
        {
            gpi_cali = 15;
        }
        gpi_count = (uint32_t)pow(2, (double)gpi_cali);

        CORE_GPI_CALI->gpi_counter_cali.bit.gpi_counter_cali = (uint8_t)gpi_cali;
        CORE_GPI_CALI->gpi_counter_cali.bit.gpi_cali_start   = 1;
        CORE_GPI_CALI->gpi_cali_sel.bit.gpi_cali_sel         = gpiox;
        while (!(CORE_GPI_CALI->gpi_cali_done.bit.gpi_cali_done))
        {
            capture_overtime += 6;
            if (capture_overtime > capture_timeout_count)
            {
                return 0;
            }
        }
    }
    gpio_capture_count = (uint32_t)((((double)gpi_count) * ((double)SYSCTRL_GetSystemClk())) / ((double)CORE_GPI_CALI->gpi_cali_done.bit.gpi_cali_cnt) + 0.5);
    return gpio_capture_count;
}


/**
 * @brief  set gpio output status
 * @param  gpiox:gpio number
 * @param  status:output status
 * @retval None
 */
void GPIO_WriteBit(GPIO_NumEnum gpiox, BitAction status)
{
    if(status == RESET)
    {
        GPIO_Config(gpiox,GPIO_MODE_OUT_LOW);
    }
    else
    {
        GPIO_Config(gpiox,GPIO_MODE_OUT_HIGH);
    }
}

/**
 * @brief Gpio set wakeup type config
 * @param gpiox:gpio number(GPIO_NumEnum enum)
 * @param wakeup_type: gpio wakeup type
 * @retval none
 */
void GPIO_SetWakeupType(GPIO_NumEnum gpiox, GPIO_WakeUpEnum wakeType)
{
    if(CORE_GPIO->gpio_cfg[gpiox] == GPIO_MODE_HI_Z)
    {
        CORE_GPIO->gpio_cfg[gpiox] =GPIO_MODE_INPUT;
    }
    uint8_t value = LPM_PUWAKEUP->gpio_wake_cfg[gpiox/2];
    value &= (~(0xf << ((gpiox%2)*4)));
    value |= (wakeType << ((gpiox%2)*4));
    LPM_PUWAKEUP->gpio_wake_cfg[gpiox/2] = value;
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/
