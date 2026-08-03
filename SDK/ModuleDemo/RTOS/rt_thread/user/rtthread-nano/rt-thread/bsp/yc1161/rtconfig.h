/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Basic Configuration
// <o>Maximal level of thread priority <8-256>
//  <i>Default: 32
#define RT_THREAD_PRIORITY_MAX  32
// <o>OS tick per second
//  <i>Default: 1000   (1ms)
#define RT_TICK_PER_SECOND  1000
// <o>Alignment size for CPU architecture data access
//  <i>Default: 4
#define RT_ALIGN_SIZE   4
// <o>the max length of object name<2-16>
//  <i>Default: 8
#define RT_NAME_MAX    8
// <c1>Using RT-Thread components initialization
//  <i>Using RT-Thread components initialization
#define RT_USING_COMPONENTS_INIT
// </c>

#define RT_USING_USER_MAIN

// <o>the stack size of main thread<1-4086>
//  <i>Default: 512
#define RT_MAIN_THREAD_STACK_SIZE     1024

// </h>

// <h>Debug Configuration
// <c1>enable kernel debug configuration
//  <i>Default: enable kernel debug configuration
#define RT_DEBUG
// </c>
// <o>enable components initialization debug configuration<0-1>
//  <i>Default: 0
#define RT_DEBUG_INIT 0
// <c1>thread stack over flow detect
//  <i> Diable Thread stack over flow detect
//#define RT_USING_OVERFLOW_CHECK
// </c>
// </h>

// <h>Hook Configuration
// <c1>using hook
//  <i>using hook
//#define RT_USING_HOOK
// </c>
// <c1>using idle hook
//  <i>using idle hook
//#define RT_USING_IDLE_HOOK
// </c>
// </h>

// <e>Software timers Configuration
// <i> Enables user timers
#define RT_USING_TIMER_SOFT         0
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif
// <o>The priority level of timer thread <0-31>
//  <i>Default: 4
#define RT_TIMER_THREAD_PRIO        4
// <o>The stack size of timer thread <0-8192>
//  <i>Default: 512
#define RT_TIMER_THREAD_STACK_SIZE  512
// </e>

// <h>IPC(Inter-process communication) Configuration
// <c1>Using Semaphore
//  <i>Using Semaphore
#define RT_USING_SEMAPHORE
// </c>
// <c1>Using Mutex
//  <i>Using Mutex
#define RT_USING_MUTEX
// </c>
// <c1>Using Event
//  <i>Using Event
#define RT_USING_EVENT
// </c>
// <c1>Using MailBox
//  <i>Using MailBox
//#define RT_USING_SIGNALS
// </c>
// <c1>Using Signals
//  <i>Using Signals
#define RT_USING_MAILBOX
// </c>
// <c1>Using Message Queue
//  <i>Using Message Queue
#define RT_USING_MESSAGEQUEUE
// </c>
// </h>

// <h>Memory Management Configuration
// <c1>Memory Pool Management
//  <i>Memory Pool Management
// #define RT_USING_MEMPOOLs
// </c>
// <c1>Dynamic Heap Management(Algorithm: small memory )
//  <i>Dynamic Heap Management
#define RT_USING_HEAP
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
// </c>
// <c1>using tiny size of memory
//  <i>using tiny size of memory
//#define RT_USING_TINY_SIZE
// </c>
// </h>

// <h>Console Configuration
// <c1>Using console
//  <i>Using console
#define RT_USING_CONSOLE
// </c>
// <o>the buffer size of console <1-1024>
//  <i>the buffer size of console
//  <i>Default: 128  (128Byte)
#define RT_CONSOLEBUF_SIZE          256
// </h>

#define RT_USING_FINSH
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE      80
#define FINSH_USING_HISTORY
// <h>FinSH Configuration
// <c1>include finsh config
//  <i>Select this choice if you using FinSH 
// #include "finsh_config.h"
// </c>
// </h>

// <h>Device Configuration
// <c1>using device framework
//  <i>using device framework
//#define RT_USING_DEVICE
// </c>
// </h>

#define OS_TIMEOUT_NEVER    -1
#define TMRTASK_DEBUG       1

#define __ramcode__  __attribute__((section ("._SECTION_RAM_CODE_DEFINE")))

#define MYNEWT_VAL_BLE_MONITOR_UART 1
#define BLE_MONITOR_UART_BUFFER_SIZE   1024
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_SCHED_ITEM 0
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_SCHED_RUN 1
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_HCI_CMD -1
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_HCI_EV -1
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_RFMGMT -1
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_USER    2
#define NIMBLE_DEBUG_LEVEL_D                    0

#define MYNEWT_VAL_BLE_HS_DEBUG    0
#define DBGLOG  rt_kprintf
#define DBGLOGV(buf,len,desc) do {  \
    rt_kprintf("%s:\n", desc);      \
    for(int i = 0; i < len; i++) {  \
        rt_kprintf("%02x ", ((uint8_t*)buf)[i]); \
    }                               \
    rt_kprintf("\n"); \
}while(0)

#define PKG_NIMBLE_ROLE_PERIPHERAL 1
#define PKG_NIMBLE_ROLE_BROADCASTER 1
#define PKG_NIMBLE_HOST_THREAD_STACK_SIZE 4*1024
#define PKG_NIMBLE_HOST_THREAD_PRIORITY 6
#define PKG_NIMBLE_CTLR_THREAD_STACK_SIZE 5*1024
#define PKG_NIMBLE_CTLR_THREAD_PRIORITY 6
#define MYNEWT_VAL_BLE_PUBLIC_DEV_ADDR (((uint8_t[6]){0xe7, 0xff, 0x1a, 0x32, 0x16, 0x63}))
#define MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME  ("yc1161_NimBLE")
#define MYNEWT_VAL_BLEUART_MAX_INPUT 128
#define MYNEWT_VAL_BLE_LL_MFRG_ID (0x50e)
#define MYNEWT_VAL_BLE_LL_RNG_BUFSIZE (32)

#define MYNEWT_VAL_BLE_LL_ROLE_PERIPHERAL   1
#define MYNEWT_VAL_BLE_LL_ROLE_BROADCASTER   1
#define MYNEWT_VAL_BLE_CONTROLLER 1
#define MYNEWT_VAL_BLE_LL_NUM_COMP_PKT_ITVL_MS (2000)
#define MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE (251)
#define MYNEWT_VAL_BLE_LL_SUPP_MAX_TX_BYTES MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE
#define MYNEWT_VAL_BLE_LL_SUPP_MAX_RX_BYTES MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE
#define MYNEWT_VAL_BLE_LL_CONN_INIT_MAX_TX_BYTES (27)
#define MYNEWT_VAL_TRNG 1
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_ENCRYPTION 1


#ifndef MYNEWT_VAL_BLE_LL_CONN_INIT_SLOTS
#define MYNEWT_VAL_BLE_LL_CONN_INIT_SLOTS (4)
#endif

#ifndef MYNEWT_VAL_BLE_LL_CONN_INIT_MIN_WIN_OFFSET
#define MYNEWT_VAL_BLE_LL_CONN_INIT_MIN_WIN_OFFSET (0)
#endif

#ifndef MYNEWT_VAL_BLE_LL_TX_PWR_DBM
#define MYNEWT_VAL_BLE_LL_TX_PWR_DBM (0)
#endif

#ifndef MYNEWT_VAL_BLE_LL_CONN_PHY_DEFAULT_PREF_MASK
#define MYNEWT_VAL_BLE_LL_CONN_PHY_DEFAULT_PREF_MASK (0x07)
#endif

/* Value copied from BLE_LL_OUR_SCA */
#ifndef MYNEWT_VAL_BLE_LL_SCA
#define MYNEWT_VAL_BLE_LL_SCA (60)
#endif
// <<< end of configuration section >>>

#endif
