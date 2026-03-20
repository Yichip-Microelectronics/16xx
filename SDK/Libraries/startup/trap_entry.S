/*
File Name    : trap_entry.S
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : Interrupt and Exception Handling Template File.
*/

#include "riscv_encoding.h"

/**
 * \brief  Global interrupt disabled or enable
 * \details
 *  This function disable global interrupt.
 * \remarks
 *  - All the interrupt requests will be ignored by CPU.
 */
.macro DISABLE_MIE
    csrc mstatus, MSTATUS_MIE
.endm

.macro ENABLE_MIE
    csrs mstatus, MSTATUS_MIE
.endm

/**
 * \brief  Macro for context save
 * \details
 * This macro save ABI defined caller saved registers in the stack.
 * \remarks
 * - This Macro could use to save context when you enter to interrupt
 * or exception
*/
/* Save caller registers */
.macro SAVE_CONTEXT
    addi sp,sp,-40*REGBYTES
    sw x1 , 0 *REGBYTES(sp) /* ra */
    sw x4 , 1 *REGBYTES(sp) /* tp */
    sw x5 , 2 *REGBYTES(sp) /* t0 */
    sw x6 , 3 *REGBYTES(sp) /* t1 */
    sw x7 , 4 *REGBYTES(sp) /* t2 */
    sw x10, 5 *REGBYTES(sp) /* a0 */
    sw x11, 6 *REGBYTES(sp) /* a1 */
    sw x12, 7 *REGBYTES(sp) /* a2 */
    sw x13, 8 *REGBYTES(sp) /* a3 */
    sw x14, 9 *REGBYTES(sp) /* a4 */
    sw x15, 10*REGBYTES(sp) /* a5 */
    sw x16, 11*REGBYTES(sp) /* a6 */
    sw x17, 12*REGBYTES(sp) /* a7 */
    sw x28, 13*REGBYTES(sp) /* t3 */
    sw x29, 14*REGBYTES(sp) /* t4 */
    sw x30, 15*REGBYTES(sp) /* t5 */
    sw x31, 16*REGBYTES(sp) /* t6 */

    fsw f0, 17*REGBYTES(sp) /* ft0 */
    fsw f1, 18*REGBYTES(sp) /* ft1 */
    fsw f2, 19*REGBYTES(sp) /* ft2 */
    fsw f3, 20*REGBYTES(sp) /* ft3 */
    fsw f4, 21*REGBYTES(sp) /* ft4 */
    fsw f5, 22*REGBYTES(sp) /* ft5 */
    fsw f6, 23*REGBYTES(sp) /* ft6 */
    fsw f7, 24*REGBYTES(sp) /* ft7 */
    fsw f10,25*REGBYTES(sp) /* fa0 */
    fsw f11,26*REGBYTES(sp) /* fa1 */
    fsw f12,27*REGBYTES(sp) /* fa2 */
    fsw f13,28*REGBYTES(sp) /* fa3 */
    fsw f14,29*REGBYTES(sp) /* fa4 */
    fsw f15,30*REGBYTES(sp) /* fa5 */
    fsw f16,31*REGBYTES(sp) /* fa6 */
    fsw f17,32*REGBYTES(sp) /* fa7 */
    fsw f28,33*REGBYTES(sp) /* ft8 */
    fsw f29,34*REGBYTES(sp) /* ft9 */
    fsw f30,35*REGBYTES(sp) /* ft10*/
    fsw f31,36*REGBYTES(sp) /* ft11*/
.endm

/**
 * \brief  Macro for restore caller registers
 * \details
 * This macro restore ABI defined caller saved registers from stack.
 * \remarks
 * - You could use this macro to restore context before you want return
 * from interrupt or exeception
 */
/* Restore caller registers */
.macro RESTORE_CONTEXT
    lw x1 , 0 *REGBYTES(sp) /* ra */
    lw x4 , 1 *REGBYTES(sp) /* tp */
    lw x5 , 2 *REGBYTES(sp) /* t0 */
    lw x6 , 3 *REGBYTES(sp) /* t1 */
    lw x7 , 4 *REGBYTES(sp) /* t2 */
    lw x10, 5 *REGBYTES(sp) /* a0 */
    lw x11, 6 *REGBYTES(sp) /* a1 */
    lw x12, 7 *REGBYTES(sp) /* a2 */
    lw x13, 8 *REGBYTES(sp) /* a3 */
    lw x14, 9 *REGBYTES(sp) /* a4 */
    lw x15, 10*REGBYTES(sp) /* a5 */
    lw x16, 11*REGBYTES(sp) /* a6 */
    lw x17, 12*REGBYTES(sp) /* a7 */
    lw x28, 13*REGBYTES(sp) /* t3 */
    lw x29, 14*REGBYTES(sp) /* t4 */
    lw x30, 15*REGBYTES(sp) /* t5 */
    lw x31, 16*REGBYTES(sp) /* t6 */

    flw f0, 17*REGBYTES(sp) /* ft0 */
    flw f1, 18*REGBYTES(sp) /* ft1 */
    flw f2, 19*REGBYTES(sp) /* ft2 */
    flw f3, 20*REGBYTES(sp) /* ft3 */
    flw f4, 21*REGBYTES(sp) /* ft4 */
    flw f5, 22*REGBYTES(sp) /* ft5 */
    flw f6, 23*REGBYTES(sp) /* ft6 */
    flw f7, 24*REGBYTES(sp) /* ft7 */
    flw f10,25*REGBYTES(sp) /* fa0 */
    flw f11,26*REGBYTES(sp) /* fa1 */
    flw f12,27*REGBYTES(sp) /* fa2 */
    flw f13,28*REGBYTES(sp) /* fa3 */
    flw f14,29*REGBYTES(sp) /* fa4 */
    flw f15,30*REGBYTES(sp) /* fa5 */
    flw f16,31*REGBYTES(sp) /* fa6 */
    flw f17,32*REGBYTES(sp) /* fa7 */
    flw f28,33*REGBYTES(sp) /* ft8 */
    flw f29,34*REGBYTES(sp) /* ft9 */
    flw f30,35*REGBYTES(sp) /* ft10*/
    flw f31,36*REGBYTES(sp) /* ft11*/
    addi sp, sp, 40*REGBYTES
.endm

/**
 * \brief  Macro for save necessary CSRs to stack
 * \details
 * This macro store MCAUSE, MEPC to stack.
 */
.macro SAVE_CSR_CONTEXT
    csrr t0,mepc
    csrr t1,mcause
    sw   t0,37*REGBYTES(sp) /* mepc */
    sw   t1,38*REGBYTES(sp) /* mcause */
.endm

/**
 * \brief  Macro for restore necessary CSRs from stack
 * \details
 * This macro restore MEPC, MCAUSE from stack.
 */
.macro RESTORE_CSR_CONTEXT
    lw   t0,37*REGBYTES(sp) /* mepc */
    lw   t1,38*REGBYTES(sp) /* mcause */
    csrw mcause, t1
    csrw mepc,   t0
.endm

    .section .vtable
    .global VECTOR_IRQ_TABLE
    .option norvc/*禁止使用压缩指令集 */
VECTOR_IRQ_TABLE:
    j trap                     //0
    j Default_Handler          //1
    j Default_Handler          //2
    j _VECTOR_SOFT_IRQ         //3
    j Default_Handler          //4
    j Default_Handler          //5
    j Default_Handler          //6
    j _VECTOR_SYSTICK_IRQ      //7 timerirq
    j Default_Handler          //8
    j Default_Handler          //9
    j Default_Handler          //10
    j EXTI_IRQHandler          //11 extirq
    j Default_Handler          //12
    j Default_Handler          //13
    j Default_Handler          //14
    j Default_Handler          //15
    j _VECTOR_RTC_IRQ          //16
    j _VECTOR_QSPI2_IRQ        //17
    j _VECTOR_QSPI1_IRQ        //18
    j _VECTOR_QSPI_IRQ         //19
    j _VECTOR_UARTA_IRQ        //20
    j _VECTOR_SADC_IRQ         //21
    j _VECTOR_GDAC_IRQ         //22
    j _VECTOR_TIMERGR0_IRQ     //23
    j _VECTOR_WDT_IRQ          //24
    j _VECTOR_SDMA_IRQ         //25
    j _VECTOR_GPIO_IRQ         //26
    j _VECTOR_MIC_IRQ         //27
    j _VECTOR_AES_IRQ          //28
    j _VECTOR_USB_IRQ          //29
    j _VECTOR_BTTIMER_IRQ      //30
    j _VECTOR_BT_IRQ           //31

    .option rvc/*启用压缩指令集 */

    .align 2
EXTI_IRQHandler:

    SAVE_CONTEXT
    SAVE_CSR_CONTEXT

_IRQ:
    /* get irq */
    la t0,plic_irq_table
    li t1,0x00008E64        /* t1: irq num */
    lw t1,0(t1)
    slli t2, t1, 2
    add t0, t0, t2
    lw t2, (t0)
    sw t1,39*REGBYTES(sp)
    ENABLE_MIE

    jalr t2         /* jump to irq */

    DISABLE_MIE
    /* clear pending mask*/
    lw t1,39*REGBYTES(sp)
    li t0,0x00008E64
    sw t1,(t0)
    nop
    nop
    /*enter new irq or recover */
    lw t1,4(t0)
    bgt t1,x0,_IRQ
    /*enable pri mie */
    li t0,MSTATUS_PRV1
    csrs mstatus,t0

    RESTORE_CSR_CONTEXT
    RESTORE_CONTEXT
    mret

.brk:
    SAVE_CONTEXT
    la t0,brk_table
    li t1,0x00008e80    /*get brk num */
    lw t1,0(t1)
    andi t1,t1,7
    slli t2,t1,2
    add t0,t0,t2
    lw t2,(t0)

    jalr t2

    li t1,0x00008e80    /*clear brk num */
    sw x0,0(t1)

    RESTORE_CONTEXT
    dret

.align 2
.globl trap
.weak trap
.type trap,%function
trap:
    /*check brk */
    li tp,0x8e80
    lw tp,0(tp)
    blt x0,tp,.brk
    /*check trap */
    csrr tp,mcause
    bgez tp,__trap //mcause>=0
    /*check plic irq */
    csrr tp,mcause
    blt tp,x0,.Interrupt
    /*never reatch here */
    j .

__trap:
    addi sp,sp,-4
    sw t0,0x0(sp)

    li t0,3
    beq tp,t0,ebreak_handle
    li t0,11
    beq tp,t0,ecall_handle

    SAVE_CONTEXT
    SAVE_CSR_CONTEXT
    /*save regs */
    la ra, trap_c
    jr ra
    /*never reatch here */
    j .

.Interrupt:
    addi sp,sp,-4
    sw t0,0x0(sp)

    la t0,VECTOR_IRQ_TABLE
    slli tp,tp,2
    add tp,t0,tp
    /*recover t0 */
    lw t0,0(sp)
    addi sp,sp,4

    jr tp


    .global trap_c
    .weak trap_c
    .type trap_c,%function
trap_c:
    j .

.align  2
ebreak_handle:
    lw t0,0(sp)
    addi sp,sp,4
    la tp,EBREAK_IRQHandler
    jr tp

.align  2
ecall_handle:
    lw t0,0(sp)
    addi sp,sp,4
    la tp,ECALL_IRQHandler
    jr tp

.align  2
.weak _VECTOR_SOFT_IRQ
.global _VECTOR_SOFT_IRQ
_VECTOR_SOFT_IRQ:
    la tp,VECTOR_SOFT_IRQ
    jr tp

.align  2
.weak _VECTOR_SYSTICK_IRQ
.global _VECTOR_SYSTICK_IRQ
_VECTOR_SYSTICK_IRQ:
    la tp,VECTOR_SYSTICK_IRQ
    jr tp

.align  2
.weak _VECTOR_RTC_IRQ
.global _VECTOR_RTC_IRQ
_VECTOR_RTC_IRQ:
    la tp,VECTOR_RTC_IRQ
    jr tp

.align  2
.weak _VECTOR_QSPI2_IRQ
.global _VECTOR_QSPI2_IRQ
_VECTOR_QSPI2_IRQ:
    la tp,VECTOR_QSPI2_IRQ
    jr tp

.align  2
.weak _VECTOR_QSPI1_IRQ
.global _VECTOR_QSPI1_IRQ
_VECTOR_QSPI1_IRQ:
    la tp,VECTOR_QSPI1_IRQ
    jr tp

.align  2
.weak _VECTOR_QSPI_IRQ
.global _VECTOR_QSPI_IRQ
_VECTOR_QSPI_IRQ:
    la tp,VECTOR_QSPI_IRQ
    jr tp

.align  2
.weak _VECTOR_UARTA_IRQ
.global _VECTOR_UARTA_IRQ
_VECTOR_UARTA_IRQ:
    la tp,VECTOR_UARTA_IRQ
    jr tp

.align  2
.weak _VECTOR_SADC_IRQ
.global _VECTOR_SADC_IRQ
_VECTOR_SADC_IRQ:
    la tp,VECTOR_SADC_IRQ
    jr tp

.align  2
.weak _VECTOR_GDAC_IRQ
.global _VECTOR_GDAC_IRQ
_VECTOR_GDAC_IRQ:
    la tp,VECTOR_GDAC_IRQ
    jr tp

.align  2
.weak _VECTOR_TIMERGR0_IRQ
.global _VECTOR_TIMERGR0_IRQ
_VECTOR_TIMERGR0_IRQ:
    la tp,VECTOR_TIMERGR0_IRQ
    jr tp

.align  2
.weak _VECTOR_WDT_IRQ
.global _VECTOR_WDT_IRQ
_VECTOR_WDT_IRQ:
    la tp,VECTOR_WDT_IRQ
    jr tp

.align  2
.weak _VECTOR_SDMA_IRQ
.global _VECTOR_SDMA_IRQ
_VECTOR_SDMA_IRQ:
    la tp,VECTOR_SDMA_IRQ
    jr tp

.align  2
.weak  _VECTOR_GPIO_IRQ
.global _VECTOR_GPIO_IRQ
_VECTOR_GPIO_IRQ:
    la tp,VECTOR_GPIO_IRQ
    jr tp

.align  2
.weak _VECTOR_MIC_IRQ
.global _VECTOR_MIC_IRQ
_VECTOR_MIC_IRQ:
    la tp,VECTOR_MIC_IRQ
    jr tp

.align  2
.weak _VECTOR_AES_IRQ
.global _VECTOR_AES_IRQ
_VECTOR_AES_IRQ:
    la tp,VECTOR_AES_IRQ
    jr tp

.align  2
.weak _VECTOR_USB_IRQ
.global _VECTOR_USB_IRQ
_VECTOR_USB_IRQ:
    la tp,VECTOR_USB_IRQ
    jr tp

.align  2
.weak  _VECTOR_BTTIMER_IRQ
.global _VECTOR_BTTIMER_IRQ
_VECTOR_BTTIMER_IRQ:
    la tp,VECTOR_BTTIMER_IRQ
    jr tp

.align  2
.weak _VECTOR_BT_IRQ
.global _VECTOR_BT_IRQ
_VECTOR_BT_IRQ:
    la tp,VECTOR_BT_IRQ
    jr tp

.align  2
.weak   Default_Handler
.global Default_Handler
.type   Default_Handler, %function
Default_Handler:
    j       Default_Handler
    .size   Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .globl  \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler    EBREAK_IRQHandler
    def_irq_handler    ECALL_IRQHandler
    def_irq_handler    RSVD_IRQHandler
    def_irq_handler    BT_IRQHandler
    def_irq_handler    SOFT_IRQHandler
    def_irq_handler    BTTIMER_IRQHandler
    def_irq_handler    UARTA_IRQHandler
    def_irq_handler    USB_IRQHandler
    def_irq_handler    AES_IRQHandler
    def_irq_handler    IIC0_IRQHandler
    def_irq_handler    IIC1_IRQHandler
    def_irq_handler    HSPI_IRQHandler
    def_irq_handler    MIC_IRQHandler
    def_irq_handler    VIO_IRQHandler
    def_irq_handler    GPIO_IRQHandler
    def_irq_handler    SDMA_IRQHandler
    def_irq_handler    HWAKE_IRQHandler
    def_irq_handler    WDT_IRQHandler
    def_irq_handler    TIMERGR0_IRQHandler
    def_irq_handler    TIMERGR1_IRQHandler
    def_irq_handler    CHGR_IRQHandler
    def_irq_handler    GDAC_IRQHandler
    def_irq_handler    SADC_IRQHandler
    def_irq_handler    UARTC_IRQHandler
    def_irq_handler    UARTD_IRQHandler
    def_irq_handler    UARTB_IRQHandler
    def_irq_handler    NEC_IRQHandler
    def_irq_handler    QSPI_IRQHandler
    def_irq_handler    QSPI1_IRQHandler
    def_irq_handler    QSPI2_IRQHandler
    def_irq_handler    RTC_IRQHandler
    def_irq_handler    CAN_IRQHandler
    def_irq_handler    VECTOR_SOFT_IRQ
    def_irq_handler    VECTOR_SYSTICK_IRQ
    def_irq_handler    VECTOR_RTC_IRQ
    def_irq_handler    VECTOR_QSPI2_IRQ
    def_irq_handler    VECTOR_QSPI1_IRQ
    def_irq_handler    VECTOR_QSPI_IRQ
    def_irq_handler    VECTOR_UARTA_IRQ
    def_irq_handler    VECTOR_SADC_IRQ
    def_irq_handler    VECTOR_GDAC_IRQ
    def_irq_handler    VECTOR_TIMERGR0_IRQ
    def_irq_handler    VECTOR_WDT_IRQ
    def_irq_handler    VECTOR_SDMA_IRQ
    def_irq_handler    VECTOR_GPIO_IRQ
    def_irq_handler    VECTOR_MIC_IRQ
    def_irq_handler    VECTOR_AES_IRQ
    def_irq_handler    VECTOR_USB_IRQ
    def_irq_handler    VECTOR_BTTIMER_IRQ
    def_irq_handler    VECTOR_BT_IRQ

    .align 2
    .globl  plic_irq_table
plic_irq_table:
        .long       RSVD_IRQHandler
        .long       BT_IRQHandler
        .long       SOFT_IRQHandler
        .long       BTTIMER_IRQHandler
        .long       UARTA_IRQHandler
        .long       USB_IRQHandler
        .long       AES_IRQHandler
        .long       IIC0_IRQHandler
        .long       IIC1_IRQHandler
        .long       HSPI_IRQHandler
        .long       MIC_IRQHandler
        .long       VIO_IRQHandler
        .long       GPIO_IRQHandler
        .long       SDMA_IRQHandler
        .long       HWAKE_IRQHandler
        .long       WDT_IRQHandler
        .long       TIMERGR0_IRQHandler
        .long       TIMERGR1_IRQHandler
        .long       CHGR_IRQHandler
        .long       GDAC_IRQHandler
        .long       SADC_IRQHandler
        .long       RSVD_IRQHandler
        .long       UARTC_IRQHandler
        .long       UARTD_IRQHandler
        .long       UARTB_IRQHandler
        .long       NEC_IRQHandler
        .long       QSPI_IRQHandler
        .long       QSPI1_IRQHandler
        .long       QSPI2_IRQHandler
        .long       RTC_IRQHandler
        .long       CAN_IRQHandler
        .long       RSVD_IRQHandler

