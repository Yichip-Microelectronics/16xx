/*
*********************************************************************************************************
*                                              uC/OS-II
*                                        The Real-Time Kernel
*
*                    Copyright 1992-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                              RISC-V Port
*
* Filename  : os_cpu.h
* Version   : V2.93.00
*********************************************************************************************************
* For       : Nuclei RISC-V RV32 and RV64
* Toolchain : GNU C Compiler
*********************************************************************************************************
* Note(s)   : Hardware FP is not supported, Modified by Nuclei to support Nuclei RISC-V Cores.
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif


#include <stdint.h>
#include "os_cpu_port.h"
#include "riscv_encoding.h"
#include "yc_types.h"

/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                    /* See Note #1.                                       */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef  unsigned  char        BOOLEAN;          /* Unsigned  8 bit boolean or logical                 */
typedef  unsigned  char        INT8U;            /* Unsigned  8 bit quantity                           */
typedef  signed    char        INT8S;            /* Signed    8 bit quantity                           */
typedef  unsigned  short       INT16U;           /* Unsigned 16 bit quantity                           */
typedef  signed    short       INT16S;           /* Signed   16 bit quantity                           */
typedef  unsigned  int         INT32U;           /* Unsigned 32 bit quantity                           */
typedef  signed    int         INT32S;           /* Signed   32 bit quantity                           */
typedef  unsigned  long  long  INT64U;           /* 64-bit unsigned integer                            */
typedef  signed    long  long  INT64S;           /* 64-bit   signed integer                            */
typedef  float                 FP32;             /* Single precision floating point                    */
typedef  double                FP64;             /* Double precision floating point                    */

#if __riscv_xlen == 64

typedef  uint64_t         OS_STK;           /* Each stack entry is 64-bit wide                    */
typedef  uint64_t         OS_CPU_SR;        /* Define size of Machine status register (64 bits)   */

#else

typedef  uint32_t         OS_STK;           /* Each stack entry is 32-bit wide                    */
typedef  uint32_t         OS_CPU_SR;        /* Define size of Machine status register (32 bits)   */

#endif

#ifndef __RISCV_XLEN
  /** \brief Refer to the width of an integer register in bits(either 32 or 64) */
  #ifndef __riscv_xlen
    #define __RISCV_XLEN    32
  #else
    #define __RISCV_XLEN    __riscv_xlen
  #endif
#endif /* __RISCV_XLEN */

/** \brief Type of Control and Status Register(CSR), depends on the XLEN defined in RISC-V */
#if __RISCV_XLEN == 32
  typedef uint32_t rv_csr_t;
#elif __RISCV_XLEN == 64
  typedef uint64_t rv_csr_t;
#else
  typedef uint32_t rv_csr_t;
#endif

#define __STR(s)                #s
#define STRINGIFY(s)            __STR(s)


/*
*********************************************************************************************************
*                                               RISC-V
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3u

#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  do { cpu_sr = OS_CPU_SR_Save();} while (0)
#define  OS_EXIT_CRITICAL()   do { OS_CPU_SR_Restore(cpu_sr);} while (0)
#else
#define  OS_ENTER_CRITICAL()  portENTER_CRITICAL()
#define  OS_EXIT_CRITICAL()   portEXIT_CRITICAL()
#endif

/*
*********************************************************************************************************
*                                        RISC-V Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH          1u          /* Stack grows from HIGH to LOW memory */

#define  OS_TASK_SW()           portYIELD()
#define  OSIntCtxSw()           portYIELD()

#ifndef OS_TICKS_PER_SEC
#warning "Use default OS_TICKS_PER_SEC=100"
#define OS_TICKS_PER_SEC            100
#endif

#ifndef configTICK_RATE_HZ
    #define configTICK_RATE_HZ  OS_TICKS_PER_SEC
#endif

/**
 * \brief CSR operation Macro for csrrc instruction.
 * \details
 * Read the content of csr register to __v,
 * then set csr register to be __v & ~val, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrrc instruction
 * \return the CSR register value before written
 */
#define __RV_CSR_READ_CLEAR(csr, val)                           \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrrc %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrw instruction.
 * \details
 * Write the content of val to csr register
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   value to store into the CSR register
 */
#define __RV_CSR_WRITE(csr, val)                                \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrw " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u                      /* See os_cpu_a.S   */
portFORCE_INLINE OS_CPU_SR OS_CPU_SR_Save (void)
{
    return __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

portFORCE_INLINE void OS_CPU_SR_Restore(OS_CPU_SR cpu_sr)
{
    __RV_CSR_WRITE(CSR_MSTATUS, cpu_sr);
}
#endif

void       OSCtxSw          (void);
void       OSStartHighRdy   (void);


/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                 /* End of 'extern'al C lang linkage.                 */
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
