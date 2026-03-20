/*
File Name    : startup_rv32.S
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : startup file.
*/

#include "riscv_encoding.h"

#define MEM_RETENTION_FLAG   0x1004a    //wakeup_flag

.globl __heap_size
.equ __heap_size,0x2000

.global _start
_start:
    .section .init
    .option norvc/*禁止使用压缩指令集 */
    .space    4*32
/**
 * Reset Handler
 */
    .globl flash_start
flash_start:
    j chip_init
    nop

wakeup_entry:
    j wakeup_recover
    nop

    .global brk_table
brk_table:
    j BRK0_Handler
    j BRK1_Handler
    j BRK2_Handler
    j BRK3_Handler
    j BRK4_Handler
    j BRK5_Handler
    j BRK6_Handler
    j BRK7_Handler

    .option rvc/*启用压缩指令集 */

wakeup_recover:
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE
    /* Disable all interrupts controlled by CSR MIE */
    csrw CSR_MIE, 0x0

    /* Initialize GP */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /*
     * Set Trap Entry MTVEC to trap_entry
     */
    la t0, VECTOR_IRQ_TABLE
    ori t0,t0,1 //=1:vector mode =0:direct mode
    csrw CSR_MTVEC, t0

    /* ===== Startup Stage 2 ===== */

    /* Enable FPU */
    li t0, MSTATUS_FS
    csrs mstatus, t0
    csrw fcsr, x0

    li t0, 0x00000800
    csrs CSR_MIE,t0

    li t1,0x9f1c
    lw t2,0(t1)
    srli t3,t2,16   /*wakeup mode */
    beqz t3,flash_start
    li t3,0xffff
    and t2,t2,t3
    slli t2,t2,2
    beqz t2,flash_start
    mv   sp,t2
    lw   s0, 0(sp)
    lw   s1, 4(sp)
    lw   s2, 8(sp)
    lw   s3,12(sp)
    lw   s4,16(sp)
    lw   s5,20(sp)
    lw   s6,24(sp)
    lw   s7,28(sp)
    lw   s8,32(sp)
    lw   s9,36(sp)
    lw  s10,40(sp)
    lw  s11,44(sp)
    lw   ra,48(sp)
    addi  sp,sp,64
    ret

chip_init:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE
    /* Disable all interrupts controlled by CSR MIE */
    csrw CSR_MIE, 0x0

    /* Initialize GP and Stack Pointer SP */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, _sp

    /*
     * Set Trap Entry MTVEC to trap_entry
     */
    la t0, VECTOR_IRQ_TABLE
    ori t0,t0,1 //=1:vector mode =0:direct mode
    csrw CSR_MTVEC, t0

    /* ===== Startup Stage 2 ===== */

    /* Enable FPU */
    li t0, MSTATUS_FS
    csrs mstatus, t0
    csrw fcsr, x0

    /*judgement init*/
    li t0, MEM_RETENTION_FLAG /*reg*/
    lb t0,(t0)
    li t1, 0xc0  /*bit6 is wakeup flag,bit7 is retention ram err*/
    and t0,t0,t1
    li t1, 0x40 /*wakeup flag=1,retention ram err=0*/
    beq t0,t1,__load_data

__load_ramcode:
    la a0, _etext_ramcode
    la a1, _sramcode
    la a2, _eramcode
    bgeu a1,a2,__load_retention_data
__load_ramcode_loop:
    lw t0, (a0)
    sw t0, (a1)
    addi a0,a0,4
    addi a1,a1,4
    bltu a1,a2,__load_ramcode_loop

__load_retention_data:
    la a0, _etext_data_retention
    la a1, _sdata_retention
    la a2, _edata_retention
    bgeu a1,a2,__load_data
__load_retention_data_loop:
    lw t0, (a0)
    sw t0, (a1)
    addi a0,a0,4
    addi a1,a1,4
    bltu a1,a2,__load_retention_data_loop

__load_data:
    la a0, _etext_data
    la a1, _sdata
    la a2, _edata
    bgeu a1,a2,__clear_bss
__load_data_loop:
    lw t0, (a0)
    sw t0, (a1)
    addi a0,a0,4
    addi a1,a1,4
    bltu a1,a2,__load_data_loop

__clear_bss:
    la a0,_sbss
    la a1,_ebss
    bgeu a0,a1,__en_mie
__clear_bss_loop:
    sw zero,(a0)
    addi a0,a0,4
    bltu a0,a1,__clear_bss_loop

__en_mie:
    call __libc_init_array
    call SYSCTRL_SystemInit
    li t0, 0x00000800
    csrs CSR_MIE,t0

    li t0, MSTATUS_MIE
    csrs mstatus, t0

#ifdef RTOS_RTTHREAD
    // Call entry function when using RT-Thread
    call entry
#else
    call main
#endif

    /* never retch here*/
__exit:
    j __exit


.global __disable_irq
.type __disable_irq, %function
__disable_irq:
    csrrci a0, mstatus, 8
    andi a0,a0,8
    ret

.global __enable_irq
.type __enable_irq, %function
__enable_irq:
    csrs mstatus, a0
    ret

.align 3
.globl delay
delay:
    add a0,a0,-1
    bnez a0,delay
    ret

.align  2
.weak   Default_BrkHandler
.global Default_BrkHandler
.type   Default_BrkHandler, %function
Default_BrkHandler:
    j       Default_BrkHandler
    .size   Default_BrkHandler, . - Default_BrkHandler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_brk_handler handler_name
    .weak   \handler_name
    .globl  \handler_name
    .set    \handler_name, Default_BrkHandler
    .endm

    def_brk_handler    BRK0_Handler
    def_brk_handler    BRK1_Handler
    def_brk_handler    BRK2_Handler
    def_brk_handler    BRK3_Handler
    def_brk_handler    BRK4_Handler
    def_brk_handler    BRK5_Handler
    def_brk_handler    BRK6_Handler
    def_brk_handler    BRK7_Handler
