/*
File Name    : yc_plic.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/24
Description  : plic irq driver interface implement.
*/
#include "yc_plic.h"

/**
 * @method PLIC_EnableIRQ(You must set the priority before enable interrupt)
 * @brief  enable interrupt
 * @param  IRQn:irq num(IRQn_Enum)
 * @retval none
 */
void PLIC_EnableIRQ(IRQn_Enum IRQn)
{
    _ASSERT(IS_IRQN(IRQn));
    PLIC_RV_INT->rv_irq_enab.reg |= (uint32_t)(1UL << IRQn);
}

/**
 * @method PLIC_DisableIRQ
 * @brief  disable interrupt
 * @param  IRQn:irq num(IRQn_Enum)
 * @retval none
 */
void PLIC_DisableIRQ(IRQn_Enum IRQn)
{
    _ASSERT(IS_IRQN(IRQn));
    PLIC_RV_INT->rv_irq_enab.reg &= ~(uint32_t)(1UL << IRQn);
}

/**
 * @method PLIC_SetPriority
 * @brief  set interrupt priority
 * @param  IRQn:irq num(IRQn_Enum)
 * @param  priority: 1-low_priority;2-medium_priority;3-high_priority
 * @retval none
 */
void PLIC_SetPriority(IRQn_Enum IRQn, uint32_t priority)
{
    _ASSERT(IS_IRQN(IRQn));
    _ASSERT(IS_PRIORITY(priority));
    PLIC_RV_INT->rv_irq_prio[IRQn/4] &= ~(uint8_t)(3 << ((IRQn % 4)*2));
    PLIC_RV_INT->rv_irq_prio[IRQn/4] |= (uint8_t)(priority << ((IRQn % 4)*2));
}
