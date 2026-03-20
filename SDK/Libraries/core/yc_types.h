/*
File Name    : yc_type.h
Author       : Yichip
Version      : V1.0
Date         : 2022/11/21
Description  : data type define.
*/

#ifndef __YC_TYPE_H__
#define __YC_TYPE_H__

#include <string.h>
#include <stdlib.h>


/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)   /* ARM Compiler */
#include <stdarg.h>
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#define __SECTION(x)                  __attribute__((section(x)))
#define __UNUSED                      __attribute__((unused))
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __INLINE
  #define __INLINE                               __inline
#endif
#if defined(__CC_ARM)
  #ifndef   __PACKED
    #define __PACKED                               __attribute__((packed))
  #endif
#else
  #ifndef   __PACKED
    #define __PACKED                               __attribute__((packed, aligned(1)))
  #endif
#endif
#elif defined (__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#include <stdarg.h>
#ifndef __ASM
  #define __ASM __asm
#endif
#define __SECTION(x)                  @ x
#define __UNUSED
#ifndef   __USED
  #if __ICCARM_V8
    #define __USED __attribute__((used))
  #else
    #define __USED _Pragma("__root")
  #endif
#endif
#define PRAGMA(x)                   _Pragma(#x)
#ifndef __ALIGNED
  #if __ICCARM_V8
    #define __ALIGNED(x) __attribute__((aligned(x)))
  #elif (__VER__ >= 7080000)
    /* Needs IAR language extensions */
    #define __ALIGNED(x) __attribute__((aligned(x)))
  #else
    #warning No compiler specific solution for __ALIGNED.__ALIGNED is ignored.
    #define __ALIGNED(x)
  #endif
#endif
#ifndef   __WEAK
  #if __ICCARM_V8
    #define __WEAK __attribute__((weak))
  #else
    #define __WEAK _Pragma("__weak")
  #endif
#endif
#ifndef __INLINE
  #define __INLINE inline
#endif
#ifndef   __PACKED
  #if __ICCARM_V8
    #define __PACKED __attribute__((packed, aligned(1)))
  #else
    /* Needs IAR language extensions */
    #define __PACKED __packed
  #endif
#endif
#elif defined (__GNUC__)            /* GNU GCC Compiler */
#include <stdint.h>
#include <stdbool.h>
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#define __SECTION(x)                  __attribute__((section(x)))
#define __UNUSED                      __attribute__((unused))
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   asm
  #define asm                                __asm
#endif
// #ifndef uint32_t
//     typedef unsigned int uint32_t;
// #endif
// #ifndef int32_t
//     typedef int int32_t;
// #endif
#else
#error not supported tool chain
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define __O                         volatile        /*!< defines 'write only' permissions     */
#define __IO                        volatile        /*!< defines 'read / write' permissions   */
#define __I                         const volatile  /*!< defines 'read only' permissions      */

#ifndef FunctionalState
    typedef enum
    {
        DISABLE = 0,
        ENABLE  = 1
    } FunctionalState;
#define IS_FUNCTIONAL_STATE(state) ((state== DISABLE) || (state == ENABLE))
#endif

#ifndef ErrorStatus
    typedef enum
    {
        ERROR   = 0,
        SUCCESS = 1
    } ErrorStatus;
#endif

#ifndef FlagStatus
    typedef enum
    {
        RESET = 0,
        SET   = !RESET
    }   FlagStatus,
        BitAction,
        ITStatus;
#endif

/*bit define*/
#define BIT0                    (0x00000001U)
#define BIT1                    (0x00000002U)
#define BIT2                    (0x00000004U)
#define BIT3                    (0x00000008U)
#define BIT4                    (0x00000010U)
#define BIT5                    (0x00000020U)
#define BIT6                    (0x00000040U)
#define BIT7                    (0x00000080U)
#define BIT8                    (0x00000100U)
#define BIT9                    (0x00000200U)
#define BIT10                   (0x00000400U)
#define BIT11                   (0x00000800U)
#define BIT12                   (0x00001000U)
#define BIT13                   (0x00002000U)
#define BIT14                   (0x00004000U)
#define BIT15                   (0x00008000U)
#define BIT16                   (0x00010000U)
#define BIT17                   (0x00020000U)
#define BIT18                   (0x00040000U)
#define BIT19                   (0x00080000U)
#define BIT20                   (0x00100000U)
#define BIT21                   (0x00200000U)
#define BIT22                   (0x00400000U)
#define BIT23                   (0x00800000U)
#define BIT24                   (0x01000000U)
#define BIT25                   (0x02000000U)
#define BIT26                   (0x04000000U)
#define BIT27                   (0x08000000U)
#define BIT28                   (0x10000000U)
#define BIT29                   (0x20000000U)
#define BIT30                   (0x40000000U)
#define BIT31                   (0x80000000U)

#define BIT_SET(a,b)            ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b)          ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b)           ((a) ^= (1<<(b)))         //bit Negation
#define BIT_GET(a,b)            (((a) & (1<<(b)))>>(b))

#define LPM_RETENTION           __SECTION("lpm_retention")
#define _interrupt              __attribute__((interrupt))

#define ADDRB(x)      (*(volatile char*)(x))
#define ADDRW(x)      (*(volatile uint16_t*)(x))
#define ADDR(x)       (*(volatile int*)(x))
#define core_gpio_cfg(x)   *(volatile uint8_t*)(0x8300 + x)

#ifdef __cplusplus
}
#endif

#endif /* __YC_TYPE_H__ */
