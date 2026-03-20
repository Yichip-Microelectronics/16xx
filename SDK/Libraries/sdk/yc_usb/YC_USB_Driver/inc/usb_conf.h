#ifndef __USB_CONF__H__
#define __USB_CONF__H__
#ifndef USE_USB_OTG_FS
    //#define USE_USB_OTG_FS
#endif /* USE_USB_OTG_FS */

#ifdef USE_USB_OTG_FS
    #define USB_OTG_FS_CORE
#endif
#ifndef USE_USB_OTG_HS
    //#define USE_USB_OTG_HS
#endif /* USE_USB_OTG_HS */

#ifndef USE_ULPI_PHY
    //#define USE_ULPI_PHY
#endif /* USE_ULPI_PHY */

#ifndef USE_EMBEDDED_PHY
    //#define USE_EMBEDDED_PHY
#endif /* USE_EMBEDDED_PHY */

#ifdef USE_USB_OTG_HS
    #define USB_OTG_HS_CORE
#endif

/*******************************************************************************
*                      FIFO Size Configuration in Device mode
*
*  (i) Receive data FIFO size = RAM for setup packets +
*                   OUT endpoint control information +
*                   data OUT packets + miscellaneous
*      Space = ONE 32-bits words
*     --> RAM for setup packets = 10 spaces
*        (n is the nbr of CTRL EPs the device core supports)
*     --> OUT EP CTRL info      = 1 space
*        (one space for status information written to the FIFO along with each
*        received packet)
*     --> data OUT packets      = (Largest Packet Size / 4) + 1 spaces
*        (MINIMUM to receive packets)
*     --> OR data OUT packets  = at least 2*(Largest Packet Size / 4) + 1 spaces
*        (if high-bandwidth EP is enabled or multiple isochronous EPs)
*     --> miscellaneous = 1 space per OUT EP
*        (one space for transfer complete status information also pushed to the
*        FIFO with each endpoint's last packet)
*
*  (ii)MINIMUM RAM space required for each IN EP Tx FIFO = MAX packet size for
*       that particular IN EP. More space allocated in the IN EP Tx FIFO results
*       in a better performance on the USB and can hide latencies on the AHB.
*
*  (iii) TXn min size = 16 words. (n  : Transmit FIFO index)
*   (iv) When a TxFIFO is not used, the Configuration should be as follows:
*       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txm can use the space allocated for Txn.
*       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txn should be configured with the minimum space of 16 words
*  (v) The FIFO is used optimally when used TxFIFOs are allocated in the top
*       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
*   (vi) In HS case12 FIFO locations should be reserved for internal DMA registers
*        so total FIFO size should be 1012 Only instead of 1024
*******************************************************************************/

/****************** USB OTG HS CONFIGURATION **********************************/
#ifdef USB_OTG_HS_CORE
    #define RX_FIFO_HS_SIZE                          512
    #define TX0_FIFO_HS_SIZE                         128
    #define TX1_FIFO_HS_SIZE                         372
    #define TX2_FIFO_HS_SIZE                          0
    #define TX3_FIFO_HS_SIZE                          0
    #define TX4_FIFO_HS_SIZE                          0
    #define TX5_FIFO_HS_SIZE                          0

    // #define USB_OTG_HS_SOF_OUTPUT_ENABLED

    #ifdef USE_ULPI_PHY
        #define USB_OTG_ULPI_PHY_ENABLED
    #endif
    #ifdef USE_EMBEDDED_PHY
        #define USB_OTG_EMBEDDED_PHY_ENABLED
    #endif
    #define USB_OTG_HS_INTERNAL_DMA_ENABLED
    #define USB_OTG_HS_DEDICATED_EP1_ENABLED
    #define USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
#endif

/****************** USB OTG FS CONFIGURATION **********************************/
// total fifo size <= 448 (512 - 64)
#define RX1_FIFO_FS_SIZE                            0
#define RX2_FIFO_FS_SIZE                            0
#define RX3_FIFO_FS_SIZE                            0
#define RX4_FIFO_FS_SIZE                            0
#define RX5_FIFO_FS_SIZE                            0
#define RX6_FIFO_FS_SIZE                            0
#define RX7_FIFO_FS_SIZE                            0

#define TX1_FIFO_FS_SIZE                            64
#define TX2_FIFO_FS_SIZE                            0
#define TX3_FIFO_FS_SIZE                            0
#define TX4_FIFO_FS_SIZE                            0
#define TX5_FIFO_FS_SIZE                            0
#define TX6_FIFO_FS_SIZE                            0
#define TX7_FIFO_FS_SIZE                            0

#define TX0_FIFO_FS_SIZE                           64
#define RX0_FIFO_FS_SIZE                           64

#define IS_FIFO_SIZE(size)          (((size) == 0)   || \
                                     ((size) == 8)   || \
                                     ((size) == 16)  || \
                                     ((size) == 32)  || \
                                     ((size) == 64)  || \
                                     ((size) == 128) || \
                                     ((size) == 256) || \
                                     ((size) == 512))
//#define USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
// #define USB_OTG_FS_SOF_OUTPUT_ENABLED

/****************** USB OTG MISC CONFIGURATION ********************************/
#define VBUS_SENSING_ENABLED

/****************** USB OTG MODE CONFIGURATION ********************************/
//#define USE_HOST_MODE
#define USE_DEVICE_MODE
//#define USE_OTG_MODE

/****************** USB OTG DMA MODE CONFIGURATION ***************************/
//#define USB_OTG_DMA_ENABLE              /* Enable DMA mode */

/* BULK Transfer */
//#define USE_USB_DMA_MODE1

/* Only one endpoind can be enable */
#define USB_OTG_DEV_DMA_EP_NUM              0x81
/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */

#define USBD_CFG_MAX_NUM           1

#ifndef USBD_ITF_MAX_NUM
#define USBD_ITF_MAX_NUM           2
#endif

#define USBD_SELF_POWERED

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
    #if defined   (__GNUC__)        /* GNU Compiler */
        #define __ALIGN_END    __attribute__ ((aligned (4)))
        #define __ALIGN_BEGIN
    #else
        #define __ALIGN_END
        #if defined   (__CC_ARM)      /* ARM Compiler */
            #define __ALIGN_BEGIN    __align(4)
        #elif defined (__ICCARM__)    /* IAR Compiler */
            #define __ALIGN_BEGIN
        #elif defined  (__TASKING__)  /* TASKING Compiler */
            #define __ALIGN_BEGIN    __align(4)
        #endif /* __CC_ARM */
    #endif /* __GNUC__ */
#else
    #define __ALIGN_BEGIN
    #define __ALIGN_END
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

#endif //__USB_CONF__H__
