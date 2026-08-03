/*
File Name    : yc_vector.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/24
Description  : vector irq driver interface implement.
*/

#include "yc_vector.h"

void static inline set_csr_bit(const int csr, int bit)
{
    __ASM("csrs %0,%1" :: "i"(csr), "r"(bit));
}

void static inline clr_csr_bit(const int csr, int bit)
{
    __ASM("csrc %0,%1" :: "i"(csr), "r"(bit));
}

/**
 * @method VECTOR_EnableIRQ
 * @brief  enable vector interrupt
 * @param  IRQn:vector irq num(VECTOR_IRQn_Enum)
 * @retval none
 */
void inline VECTOR_EnableIRQ(VECTOR_IRQn_Enum IRQn)
{
    _ASSERT(IS_VECTOR_IRQN(IRQn));
    set_csr_bit(CSR_MIE,1 << IRQn);
}

/**
 * @method VECTOR_DisableIRQ
 * @brief  disable vector interrupt
 * @param  IRQn:vector irq num(VECTOR_IRQn_Enum)
 * @retval none
 */
void inline VECTOR_DisableIRQ(VECTOR_IRQn_Enum IRQn)
{
    _ASSERT(IS_VECTOR_IRQN(IRQn));
    clr_csr_bit(CSR_MIE,1 << IRQn);
}
