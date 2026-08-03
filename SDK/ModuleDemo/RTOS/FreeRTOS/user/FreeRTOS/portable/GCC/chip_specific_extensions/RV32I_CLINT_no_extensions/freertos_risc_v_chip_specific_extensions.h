/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code that tailors the port to a specific RISC-V chip:
 *
 * + FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S contains the code that
 *   is common to all currently supported RISC-V chips.  There is only one
 *   portASM.S file because the same file is built for all RISC-V target chips.
 *
 * + Header files called freertos_risc_v_chip_specific_extensions.h contain the
 *   code that tailors the FreeRTOS kernel's RISC-V port to a specific RISC-V
 *   chip.  There are multiple freertos_risc_v_chip_specific_extensions.h files
 *   as there are multiple RISC-V chip implementations.
 *
 * !!!NOTE!!!
 * TAKE CARE TO INCLUDE THE CORRECT freertos_risc_v_chip_specific_extensions.h
 * HEADER FILE FOR THE CHIP IN USE.  This is done using the assembler's (not the
 * compiler's!) include path.  For example, if the chip in use includes a core
 * local interrupter (CLINT) and does not include any chip specific register
 * extensions then add the path below to the assembler's include path:
 * FreeRTOS\Source\portable\GCC\RISC-V-RV32\chip_specific_extensions\RV32I_CLINT_no_extensions
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmADDITIONAL_F_REG_SIZE        4

#define portasmHAS_SIFIVE_CLINT             0
#define portasmHAS_MTIME                    1
#define portasmADDITIONAL_CONTEXT_SIZE      ((32*portasmADDITIONAL_F_REG_SIZE)/(__riscv_xlen/8)) /* Must be even number on 32-bit cores. */
#define portasmADDITIONAL_CONTEXT_SIZE_BYTE (32*portasmADDITIONAL_F_REG_SIZE)

.macro portasmSAVE_ADDITIONAL_REGISTERS
    addi sp, sp, -portasmADDITIONAL_CONTEXT_SIZE_BYTE
    fsw f0,  1*portasmADDITIONAL_F_REG_SIZE(sp) /* ft0 */
    fsw f1,  2*portasmADDITIONAL_F_REG_SIZE(sp) /* ft1 */
    fsw f2,  3*portasmADDITIONAL_F_REG_SIZE(sp) /* ft2 */
    fsw f3,  4*portasmADDITIONAL_F_REG_SIZE(sp) /* ft3 */
    fsw f4,  5*portasmADDITIONAL_F_REG_SIZE(sp) /* ft4 */
    fsw f5,  6*portasmADDITIONAL_F_REG_SIZE(sp) /* ft5 */
    fsw f6,  7*portasmADDITIONAL_F_REG_SIZE(sp) /* ft6 */
    fsw f7,  8*portasmADDITIONAL_F_REG_SIZE(sp) /* ft7 */
    fsw f8,  9*portasmADDITIONAL_F_REG_SIZE(sp) /* fs0 */
    fsw f9, 10*portasmADDITIONAL_F_REG_SIZE(sp) /* fs1 */
    fsw f10,11*portasmADDITIONAL_F_REG_SIZE(sp) /* fa0 */
    fsw f11,12*portasmADDITIONAL_F_REG_SIZE(sp) /* fa1 */
    fsw f12,13*portasmADDITIONAL_F_REG_SIZE(sp) /* fa2 */
    fsw f13,14*portasmADDITIONAL_F_REG_SIZE(sp) /* fa3 */
    fsw f14,15*portasmADDITIONAL_F_REG_SIZE(sp) /* fa4 */
    fsw f15,16*portasmADDITIONAL_F_REG_SIZE(sp) /* fa5 */
    fsw f16,17*portasmADDITIONAL_F_REG_SIZE(sp) /* fa6 */
    fsw f17,18*portasmADDITIONAL_F_REG_SIZE(sp) /* fa7 */
    fsw f18,19*portasmADDITIONAL_F_REG_SIZE(sp) /* fs2 */
    fsw f19,20*portasmADDITIONAL_F_REG_SIZE(sp) /* fs3 */
    fsw f20,21*portasmADDITIONAL_F_REG_SIZE(sp) /* fs4 */
    fsw f21,22*portasmADDITIONAL_F_REG_SIZE(sp) /* fs5 */
    fsw f22,23*portasmADDITIONAL_F_REG_SIZE(sp) /* fs6 */
    fsw f23,24*portasmADDITIONAL_F_REG_SIZE(sp) /* fs7 */
    fsw f24,25*portasmADDITIONAL_F_REG_SIZE(sp) /* fs8 */
    fsw f25,26*portasmADDITIONAL_F_REG_SIZE(sp) /* fs9 */
    fsw f26,27*portasmADDITIONAL_F_REG_SIZE(sp) /* fs10*/
    fsw f27,28*portasmADDITIONAL_F_REG_SIZE(sp) /* fs11*/
    fsw f28,29*portasmADDITIONAL_F_REG_SIZE(sp) /* ft8 */
    fsw f29,30*portasmADDITIONAL_F_REG_SIZE(sp) /* ft9 */
    fsw f30,31*portasmADDITIONAL_F_REG_SIZE(sp) /* ft10*/
    fsw f31,32*portasmADDITIONAL_F_REG_SIZE(sp) /* ft11*/
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
    flw f0,  1*portasmADDITIONAL_F_REG_SIZE(sp) /* ft0 */
    flw f1,  2*portasmADDITIONAL_F_REG_SIZE(sp) /* ft1 */
    flw f2,  3*portasmADDITIONAL_F_REG_SIZE(sp) /* ft2 */
    flw f3,  4*portasmADDITIONAL_F_REG_SIZE(sp) /* ft3 */
    flw f4,  5*portasmADDITIONAL_F_REG_SIZE(sp) /* ft4 */
    flw f5,  6*portasmADDITIONAL_F_REG_SIZE(sp) /* ft5 */
    flw f6,  7*portasmADDITIONAL_F_REG_SIZE(sp) /* ft6 */
    flw f7,  8*portasmADDITIONAL_F_REG_SIZE(sp) /* ft7 */
    flw f8,  9*portasmADDITIONAL_F_REG_SIZE(sp) /* fs0 */
    flw f9, 10*portasmADDITIONAL_F_REG_SIZE(sp) /* fs1 */
    flw f10,11*portasmADDITIONAL_F_REG_SIZE(sp) /* fa0 */
    flw f11,12*portasmADDITIONAL_F_REG_SIZE(sp) /* fa1 */
    flw f12,13*portasmADDITIONAL_F_REG_SIZE(sp) /* fa2 */
    flw f13,14*portasmADDITIONAL_F_REG_SIZE(sp) /* fa3 */
    flw f14,15*portasmADDITIONAL_F_REG_SIZE(sp) /* fa4 */
    flw f15,16*portasmADDITIONAL_F_REG_SIZE(sp) /* fa5 */
    flw f16,17*portasmADDITIONAL_F_REG_SIZE(sp) /* fa6 */
    flw f17,18*portasmADDITIONAL_F_REG_SIZE(sp) /* fa7 */
    flw f18,19*portasmADDITIONAL_F_REG_SIZE(sp) /* fs2 */
    flw f19,20*portasmADDITIONAL_F_REG_SIZE(sp) /* fs3 */
    flw f20,21*portasmADDITIONAL_F_REG_SIZE(sp) /* fs4 */
    flw f21,22*portasmADDITIONAL_F_REG_SIZE(sp) /* fs5 */
    flw f22,23*portasmADDITIONAL_F_REG_SIZE(sp) /* fs6 */
    flw f23,24*portasmADDITIONAL_F_REG_SIZE(sp) /* fs7 */
    flw f24,25*portasmADDITIONAL_F_REG_SIZE(sp) /* fs8 */
    flw f25,26*portasmADDITIONAL_F_REG_SIZE(sp) /* fs9 */
    flw f26,27*portasmADDITIONAL_F_REG_SIZE(sp) /* fs10*/
    flw f27,28*portasmADDITIONAL_F_REG_SIZE(sp) /* fs11*/
    flw f28,29*portasmADDITIONAL_F_REG_SIZE(sp) /* ft8 */
    flw f29,30*portasmADDITIONAL_F_REG_SIZE(sp) /* ft9 */
    flw f30,31*portasmADDITIONAL_F_REG_SIZE(sp) /* ft10*/
    flw f31,32*portasmADDITIONAL_F_REG_SIZE(sp) /* ft11*/
    addi sp, sp, portasmADDITIONAL_CONTEXT_SIZE_BYTE
    .endm

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
