#ifndef _USB_MSC_CORE_H_
#define _USB_MSC_CORE_H_

#include  "usbd_ioreq.h"


#define BOT_GET_MAX_LUN              0xFE
#define BOT_RESET                    0xFF
#define USB_MSC_CONFIG_DESC_SIZ      32

#define MSC_EPIN_SIZE                MSC_MAX_PACKET 
#define MSC_EPOUT_SIZE               MSC_MAX_PACKET 

#define MSC_IN_EP                    0x81
#define MSC_OUT_EP                   0x01
#ifdef USE_USB_OTG_HS  
#ifdef USE_ULPI_PHY
#define MSC_MAX_PACKET               512
#else
#define MSC_MAX_PACKET               64
#endif
#else  /*USE_USB_OTG_FS*/
#define MSC_MAX_PACKET                64   /*对应usb设备端点大小*/
#endif


#define MSC_MEDIA_PACKET             4096   /*对应存储介质扇区大小*/

extern USBD_Class_cb_TypeDef  USBD_MSC_cb;

#endif /* _USB_MSC_CORE_H_ */
