/**/
#include "riscv_encoding.h"

#define portRegNum          (30+32)

#define portCONTEXT_SIZE    ( portRegNum * REGBYTES )

#********************************************************************************************************
#                                          PUBLIC FUNCTIONS
#********************************************************************************************************

    .extern  OSTCBCur
    .extern  OSTCBHighRdy

    .global prvPortStartFirstTask


#********************************************************************************************************
#                                     CODE GENERATION DIRECTIVES
#********************************************************************************************************

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

.macro SAVE_CONTEXT_OS
    addi sp, sp, -portCONTEXT_SIZE
    STORE x1,  1  * REGBYTES(sp)    /* RA */
    STORE x5,  2  * REGBYTES(sp)
    STORE x6,  3  * REGBYTES(sp)
    STORE x7,  4  * REGBYTES(sp)
    STORE x8,  5  * REGBYTES(sp)
    STORE x9,  6  * REGBYTES(sp)
    STORE x10, 7  * REGBYTES(sp)
    STORE x11, 8  * REGBYTES(sp)
    STORE x12, 9  * REGBYTES(sp)
    STORE x13, 10 * REGBYTES(sp)
    STORE x14, 11 * REGBYTES(sp)
    STORE x15, 12 * REGBYTES(sp)
    STORE x16, 13 * REGBYTES(sp)
    STORE x17, 14 * REGBYTES(sp)
    STORE x18, 15 * REGBYTES(sp)
    STORE x19, 16 * REGBYTES(sp)
    STORE x20, 17 * REGBYTES(sp)
    STORE x21, 18 * REGBYTES(sp)
    STORE x22, 19 * REGBYTES(sp)
    STORE x23, 20 * REGBYTES(sp)
    STORE x24, 21 * REGBYTES(sp)
    STORE x25, 22 * REGBYTES(sp)
    STORE x26, 23 * REGBYTES(sp)
    STORE x27, 24 * REGBYTES(sp)
    STORE x28, 25 * REGBYTES(sp)
    STORE x29, 26 * REGBYTES(sp)
    STORE x30, 27 * REGBYTES(sp)
    STORE x31, 28 * REGBYTES(sp)

    fsw f0, 29*REGBYTES(sp) /* ft0 */
    fsw f1, 30*REGBYTES(sp) /* ft1 */
    fsw f2, 31*REGBYTES(sp) /* ft2 */
    fsw f3, 32*REGBYTES(sp) /* ft3 */
    fsw f4, 33*REGBYTES(sp) /* ft4 */
    fsw f5, 34*REGBYTES(sp) /* ft5 */
    fsw f6, 35*REGBYTES(sp) /* ft6 */
    fsw f7, 36*REGBYTES(sp) /* ft7 */
    fsw f8, 37*REGBYTES(sp) /* fs0 */
    fsw f9, 38*REGBYTES(sp) /* fs1 */
    fsw f10,39*REGBYTES(sp) /* fa0 */
    fsw f11,40*REGBYTES(sp) /* fa1 */
    fsw f12,41*REGBYTES(sp) /* fa2 */
    fsw f13,42*REGBYTES(sp) /* fa3 */
    fsw f14,43*REGBYTES(sp) /* fa4 */
    fsw f15,44*REGBYTES(sp) /* fa5 */
    fsw f16,45*REGBYTES(sp) /* fa6 */
    fsw f17,46*REGBYTES(sp) /* fa7 */
    fsw f18,47*REGBYTES(sp) /* fs2 */
    fsw f19,48*REGBYTES(sp) /* fs3 */
    fsw f20,49*REGBYTES(sp) /* fs4 */
    fsw f21,50*REGBYTES(sp) /* fs5 */
    fsw f22,51*REGBYTES(sp) /* fs6 */
    fsw f23,52*REGBYTES(sp) /* fs7 */
    fsw f24,53*REGBYTES(sp) /* fs8 */
    fsw f25,54*REGBYTES(sp) /* fs9 */
    fsw f26,55*REGBYTES(sp) /* fs10*/
    fsw f27,56*REGBYTES(sp) /* fs11*/
    fsw f28,57*REGBYTES(sp) /* ft8 */
    fsw f29,58*REGBYTES(sp) /* ft9 */
    fsw f30,59*REGBYTES(sp) /* ft10*/
    fsw f31,60*REGBYTES(sp) /* ft11*/
.endm


.macro RESTORE_CONTEXT_OS
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)

    flw f0, 29*REGBYTES(sp) /* ft0 */
    flw f1, 30*REGBYTES(sp) /* ft1 */
    flw f2, 31*REGBYTES(sp) /* ft2 */
    flw f3, 32*REGBYTES(sp) /* ft3 */
    flw f4, 33*REGBYTES(sp) /* ft4 */
    flw f5, 34*REGBYTES(sp) /* ft5 */
    flw f6, 35*REGBYTES(sp) /* ft6 */
    flw f7, 36*REGBYTES(sp) /* ft7 */
    flw f8, 37*REGBYTES(sp) /* fs0 */
    flw f9, 38*REGBYTES(sp) /* fs1 */
    flw f10,39*REGBYTES(sp) /* fa0 */
    flw f11,40*REGBYTES(sp) /* fa1 */
    flw f12,41*REGBYTES(sp) /* fa2 */
    flw f13,42*REGBYTES(sp) /* fa3 */
    flw f14,43*REGBYTES(sp) /* fa4 */
    flw f15,44*REGBYTES(sp) /* fa5 */
    flw f16,45*REGBYTES(sp) /* fa6 */
    flw f17,46*REGBYTES(sp) /* fa7 */
    flw f18,47*REGBYTES(sp) /* fs2 */
    flw f19,48*REGBYTES(sp) /* fs3 */
    flw f20,49*REGBYTES(sp) /* fs4 */
    flw f21,50*REGBYTES(sp) /* fs5 */
    flw f22,51*REGBYTES(sp) /* fs6 */
    flw f23,52*REGBYTES(sp) /* fs7 */
    flw f24,53*REGBYTES(sp) /* fs8 */
    flw f25,54*REGBYTES(sp) /* fs9 */
    flw f26,55*REGBYTES(sp) /* fs10*/
    flw f27,56*REGBYTES(sp) /* fs11*/
    flw f28,57*REGBYTES(sp) /* ft8 */
    flw f29,58*REGBYTES(sp) /* ft9 */
    flw f30,59*REGBYTES(sp) /* ft10*/
    flw f31,60*REGBYTES(sp) /* ft11*/

    addi sp, sp, portCONTEXT_SIZE
.endm


    .section      .ramcode
    .align 2
    .global EXTI_IRQHandler
    .type EXTI_IRQHandler,%function
EXTI_IRQHandler:
    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the necessary CSR registers */
    SAVE_CSR_CONTEXT

    /* switch to interrupt stack when first enter */
    la a1, OSIntNesting
    lw a0, 0(a1)
    bgt a0,x0,_enter_exti_irq
    /* set to isr stack sp, _sp is not used in RTOS */
    csrrw sp,mscratch,sp
_enter_exti_irq:
    call OSIntEnter
    addi    sp, sp, -1 * REGBYTES
_exti_loop:
    /* get irq */
    la t0,plic_irq_table
    li t1,0x00008E64        /* t1: irq num */
    lw t1,0(t1)
    slli t2, t1, 2
    add t0, t0, t2
    lw t2, (t0)
    sw t1,0*REGBYTES(sp)

    csrs mstatus, MSTATUS_MIE   /*ENABLE_MIE */
    jalr t2         /* jump to irq */
    csrc mstatus, MSTATUS_MIE   /*DISABLE_MIE */

    /* clear pending mask*/
    lw t1,0*REGBYTES(sp)
    li t0,0x00008E64
    sw t1,(t0)
    nop
    nop
    /*enter new irq or recover */
    lw t1,4(t0)
    bgt t1,x0,_exti_loop
    addi    sp, sp, 1 * REGBYTES

    call OSIntExit
    la a1, OSIntNesting
    lw a0, 0(a1)

    bgt a0,x0,_exit_exti_irq
    /* switch to thread stack when last irq exit */
    csrrw sp,mscratch,sp
_exit_exti_irq:
    /* Restore the necessary CSR registers */
    RESTORE_CSR_CONTEXT
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT
    mret

/* Start the first task.  This also clears the bit that indicates the FPU is
	in use in case the FPU was used before the scheduler was started - which
	would otherwise result in the unnecessary leaving of space in the stack
	for lazy saving of FPU registers. */
.section    .text
.align 3
prvPortStartFirstTask:
    /* Setup Interrupt Stack using
       The stack that was used by main()
       before the scheduler is started is
       no longer required after the scheduler is started.
       Interrupt stack pointer is stored in CSR_MSCRATCH */
    la t0, _sp
    csrw mscratch, t0
    LOAD sp, OSTCBHighRdy           /* Load OSTCBHighRdy. */
    LOAD sp, 0x0(sp)                /* Read sp from first TCB member */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    RESTORE_CONTEXT_OS

    mret


  .align 2
  .global VECTOR_SOFT_IRQ
VECTOR_SOFT_IRQ:
    SAVE_CONTEXT_OS

    /* Push mstatus to stack */
    csrr t0, CSR_MSTATUS
    STORE t0,  (portRegNum - 1)  * REGBYTES(sp)

    /* Push additional registers */

    /* Store sp to task stack */
    LOAD t0, OSTCBCur
    STORE sp, 0(t0)

    csrr t0, CSR_MEPC
    STORE t0, 0(sp)

    call xPortTaskSwitch

    /* Switch task context */
    LOAD t0, OSTCBHighRdy           /* Load pxCurrentTCB. */
    LOAD sp, 0x0(t0)                /* Read sp from first TCB member */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop additional registers */

    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /*clear soft irq flag*/
    li t0,0x8045
    sb x0,0(t0)
    /* Restore Registers from Stack */
    RESTORE_CONTEXT_OS
    mret

