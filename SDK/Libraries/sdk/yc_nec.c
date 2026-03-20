/*
File Name    : yc_nec.c
Author       : Yichip
Version      : V1.0
Date         : 2022/12/12
Description  : nec driver interface implement.
*/

#include "yc_nec.h"
#include "yc_sysctrl.h"

/**
 * @brief  : NEC module init
 * @param  : initStruct  :point to NEC_InitStruct
 * @return : None
 */
void NEC_Init(NEC_InitStruct *initStruct)
{
    NEC_DeInit();
    SYSCTRL_ClkEn(CLK_NEC,ENABLE);
    _ASSERT((initStruct->bufSaddr%2) == 0);
    CORE_NEC->nec_startaddr.bit.nec_startaddr = initStruct->bufSaddr;
    CORE_NEC->nec_endaddr.bit.nec_endaddr = (initStruct->bufSaddr + ((initStruct->bufSize-2)));
    CORE_NEC->nec_rptr.bit.nec_rptr = (initStruct->bufSaddr);
    NEC_SetMode(initStruct->mode);
    NEC_SetClk(initStruct->clk);
}

/**
 * @brief : NEC DeInit.
 * @param :none
 * @return:None
 */
void NEC_DeInit(void)
{
    CORE_NEC->nec_ctrl.reg = 0;
}

/**
 * @brief  : set NEC module clock
 * @param  : clk  :clock frequency
 * @return : None
 */
void NEC_SetClk(uint32_t clk)
{
    uint32_t nec_clk_div = SYSCTRL_GetSystemClk() / clk;
    _ASSERT((nec_clk_div*clk) == SYSCTRL_GetSystemClk());//为确保解码准确率,时钟分频值必须能整除
    CORE_NEC->nec_ctrl.bit.clk_div = nec_clk_div; // 计数值小于32的都会被硬件过滤掉
}

/**
 * @brief  : set NEC module mode
 * @param  : mode  :NEC mode
 * @return : None
 */
void NEC_SetMode(NEC_ModeEnum mode)
{
    CORE_NEC->nec_mode.bit.nec_mode = mode;
    if(mode == NEC_RX)
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_ir_rx_en.bit.rg_lpm_ir_rx_en = ENABLE;
    }
    else
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_ir_rx_en.bit.rg_lpm_ir_rx_en = DISABLE;
    }
}

/**
 * @brief  : set NEC module rx single source
 * @param  : necSource  :NEC_FROM_GPIO or NEC_FROM_IR
 * @return : None
 */
void NEC_RxSrcSel(NEC_RxSrcEnum necSource)
{
    CORE_NEC->nec_ctrl.bit.in_sel = necSource;
}

/**
 * @brief  : set NEC module rx gain level
 * @param  : gainLevel  :GAIN_LOWEST GAIN_LOW GAIN_HIGH or GAIN_HIGHEST
 * @return : None
 */
void NEC_SetRxGainLevel(NEC_GainLevelEnum gainLevel)
{
    LPM_HW_CONTROL_NOEXP->rg_lpm_ir_rx_en.bit.rg_lpm_ir_rx_gc = gainLevel;
}

/**
 * @brief  : NEC module ptr init
 * @param  : None
 * @return : None
 */
void NEC_PtrInit(void)
{
    CORE_NEC->nec_rptr.bit.nec_rptr =  CORE_NEC->nec_startaddr.bit.nec_startaddr;
}

/**
 * @brief  : NEC module enable or disable
 * @param  : state  :ENABLE or DISABLE
 * @return : None
 */
void NEC_Cmd(FunctionalState state)
{
    CORE_NEC->nec_ctrl.bit.enable = state;
}

/**
 * @brief  : NEC module irq ctrl
 * @param  : irqType  :NEC_IRQ_DONE NEC_IRQ_HALF_DONE or (NEC_IRQ_DONE|NEC_IRQ_HALF_DONE)
 * @param  : state  :ENABLE or DISABLE
 * @return : None
 */
void NEC_SetIrq(uint8_t irqType,FunctionalState state)
{
    if (state)
    {
         CORE_NEC->nec_intr_en.reg |= (irqType&0x03);
    }
    else
    {
        CORE_NEC->nec_intr_en.reg &= ((~irqType)&0x03);
    }
}

/**
 * @brief  : NEC module irq clear
 * @param  : irqType  :NEC_IRQ_DONE NEC_IRQ_HALF_DONE or (NEC_IRQ_DONE|NEC_IRQ_HALF_DONE)
 * @return : None
 */
void NEC_ClearIrq(uint8_t irqType)
{
    uint8_t nec_irq_en = CORE_NEC->nec_intr_en.reg;
    CORE_NEC->nec_intr_en.reg ^= (irqType&0x03);
    if(CORE_NEC->nec_items.bit.nec_items == 0)
    {
        CORE_NEC->nec_intr_en.reg = 0;
    }
    else
    {
        CORE_NEC->nec_intr_en.reg = nec_irq_en;
    }
}

/**
 * @brief  : get NEC irq status
 * @param  : None
 * @retval : NEC irq status
 */
uint8_t NEC_GeIrqStatus(void)
{
    return (CORE_NEC->nec_status.reg&0x03);
}

/**
 * @brief  : write data into dma buffer
 * @param  : buf  :point to data buffer
 * @param  : len  :data length(uint16_t count)
 * @retval : write success data count
 */
int NEC_WriteData(uint16_t *buf,int len)
{
    int writed_len = 0;
    uint32_t wptr_value = CORE_NEC->nec_rptr.bit.nec_rptr;
    while(writed_len < len)
    {
        *((uint16_t*)wptr_value) = buf[writed_len];
        wptr_value +=2;
        writed_len++;
        if(wptr_value > CORE_NEC->nec_endaddr.reg)
        {
            wptr_value = CORE_NEC->nec_startaddr.reg;
        }
    }
    CORE_NEC->nec_rptr.bit.nec_rptr = wptr_value;
    return writed_len;
}

/**
 * @brief  : read data from dma buffer
 * @param  : none
 * @retval : read data
 */
uint16_t NEC_ReadData(void)
{
    uint32_t rptr_value = CORE_NEC->nec_rptr.bit.nec_rptr;
    uint16_t nec_data = *((uint16_t*)rptr_value);
    rptr_value +=2;
    if(rptr_value > CORE_NEC->nec_endaddr.reg)
    {
        rptr_value = CORE_NEC->nec_startaddr.reg;
    }
    CORE_NEC->nec_rptr.bit.nec_rptr = rptr_value;
    return nec_data;
}

/**
 * @brief  : get nec data count
 * @param  : none
 * @retval : nec data count
 *              tx:unsent uint16_t data count
 *              rx:received uint16_t data count
 */
uint32_t NEC_GetDataCount(void)
{
    return CORE_NEC->nec_items.bit.nec_items;
}
