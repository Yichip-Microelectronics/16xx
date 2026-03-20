#ifndef __USB_DEF_H__
#define __USB_DEF_H__

#include "register.h"
#ifdef __cplusplus
extern "C" {
#endif


//#define CPU_MHZ        (24*1000000)


#define USB_OTG_SPEED_PARAM_HIGH 0
#define USB_OTG_SPEED_PARAM_HIGH_IN_FULL 1
#define USB_OTG_SPEED_PARAM_FULL 3

#define USB_OTG_SPEED_HIGH      0
#define USB_OTG_SPEED_FULL      1

#define USB_OTG_ULPI_PHY      1
#define USB_OTG_EMBEDDED_PHY  2
#define USB_SETUP_PACKET_LEN  8





#define GAHBCFG_TXFEMPTYLVL_EMPTY              1
#define GAHBCFG_TXFEMPTYLVL_HALFEMPTY          0
#define GAHBCFG_GLBINT_ENABLE                  1
#define GAHBCFG_INT_DMA_BURST_SINGLE           0
#define GAHBCFG_INT_DMA_BURST_INCR             1
#define GAHBCFG_INT_DMA_BURST_INCR4            3
#define GAHBCFG_INT_DMA_BURST_INCR8            5
#define GAHBCFG_INT_DMA_BURST_INCR16           7
#define GAHBCFG_DMAENABLE                      1
#define GAHBCFG_TXFEMPTYLVL_EMPTY              1
#define GAHBCFG_TXFEMPTYLVL_HALFEMPTY          0
#define GRXSTS_PKTSTS_IN                       2
#define GRXSTS_PKTSTS_IN_XFER_COMP             3
#define GRXSTS_PKTSTS_DATA_TOGGLE_ERR          5
#define GRXSTS_PKTSTS_CH_HALTED                7

#define MODE_HNP_SRP_CAPABLE                   0
#define MODE_SRP_ONLY_CAPABLE                  1
#define MODE_NO_HNP_SRP_CAPABLE                2
#define MODE_SRP_CAPABLE_DEVICE                3
#define MODE_NO_SRP_CAPABLE_DEVICE             4
#define MODE_SRP_CAPABLE_HOST                  5
#define MODE_NO_SRP_CAPABLE_HOST               6
#define A_HOST                                 1
#define A_SUSPEND                              2
#define A_PERIPHERAL                           3
#define B_PERIPHERAL                           4
#define B_HOST                                 5

#define DEVICE_MODE                            0
#define HOST_MODE                              1
#define OTG_MODE                               2

#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     0
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     1
#define DSTS_ENUMSPD_LS_PHY_6MHZ               2
#define DSTS_ENUMSPD_FS_PHY_48MHZ              3

#define DCFG_FRAME_INTERVAL_80                 0
#define DCFG_FRAME_INTERVAL_85                 1
#define DCFG_FRAME_INTERVAL_90                 2
#define DCFG_FRAME_INTERVAL_95                 3

#define DEP0CTL_MPS_64                         0
#define DEP0CTL_MPS_32                         1
#define DEP0CTL_MPS_16                         2
#define DEP0CTL_MPS_8                          3

#define EP_SPEED_LOW                           0
#define EP_SPEED_FULL                          1
#define EP_SPEED_HIGH                          2

#define EP_TYPE_CTRL                           0
#define EP_TYPE_ISOC                           1
#define EP_TYPE_BULK                           2
#define EP_TYPE_INTR                           3
#define EP_TYPE_MSK                            3

#define STS_GOUT_NAK                           1
#define STS_DATA_UPDT                          2
#define STS_XFER_COMP                          3
#define STS_SETUP_COMP                         4
#define STS_SETUP_UPDT                         6
/**
  * @}
  */


/** @defgroup __HOST_DEFINES_
  * @{
  */
#define HC_PID_DATA0                           0
#define HC_PID_DATA2                           1
#define HC_PID_DATA1                           2
#define HC_PID_SETUP                           3

#define HPRT0_PRTSPD_HIGH_SPEED                1
#define HPRT0_PRTSPD_FULL_SPEED                2
#define HPRT0_PRTSPD_LOW_SPEED                 3
//#define HPRT0_PRTSPD_HIGH_SPEED                0
//#define HPRT0_PRTSPD_FULL_SPEED                1
//#define HPRT0_PRTSPD_LOW_SPEED                 2

#define HCFG_30_60_MHZ                         0
#define HCFG_48_MHZ                            1
#define HCFG_6_MHZ                             2

#define HCCHAR_CTRL                            0
#define HCCHAR_ISOC                            1
#define HCCHAR_BULK                            2
#define HCCHAR_INTR                            3

#define  MIN(a, b)      (((a) < (b)) ? (a) : (b))
#define NUM_EP_FIFO                         4

#define USB_OTG_MAX_TX_FIFOS                16

#define USB_OTG_FS_MAX_PACKET_SIZE          64
#define USB_OTG_MAX_EP0_SIZE                64


#define LOBYTE(x)                           ((uint8_t)((x) & 0xff))
#define HIBYTE(x)                           ((uint8_t)((x) >> 8 & 0xff))
typedef enum
{
    USB_OTG_HS_CORE_ID = 0,
    USB_OTG_FS_CORE_ID = 1
} USB_OTG_CORE_ID_TypeDef;

// #define USB_OTG_MODIFY_REG8(reg, clear_mask, set_mask)           HWRITE(reg, (((HREAD(reg)) & ~clear_mask) | set_mask))

// #define USB_OTG_MODIFY_REG16(reg, clear_mask, set_mask)           HWRITEW(reg, (((HREADW(reg)) & ~clear_mask) | set_mask))

typedef union _USB_OTG_IRQ_TypeDef
{
    uint16_t d16;
    struct
    {
        uint16_t setup                  : 1;
        uint16_t suspend                : 1;
        uint16_t nak                    : 1;
        uint16_t reset                  : 1;
        uint16_t ack                    : 1;
        uint16_t rx_ready               : 1;
        uint16_t rx_full                : 1;
        uint16_t clkcal                 : 1;
        uint16_t ep_txdoen              : 4;
        uint16_t tx_donex               : 1;

    } b;

} USB_OTG_IRQ_TypeDef;
enum USB_OTG_SPEED
{
    USB_SPEED_UNKNOWN = 0,
    USB_SPEED_LOW,
    USB_SPEED_FULL,
    USB_SPEED_HIGH
};
#ifdef __cplusplus
}
#endif

#endif  /* __USB_DEF_H__ */

