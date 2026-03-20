#ifndef __USB_CDC_CORE_H_
#define __USB_CDC_CORE_H_

#include  "usbd_ioreq.h"

#define TRANS_EP_NUM                        3  /*应用数据传输端点个数,用于申请及管理中转buffer*/

#define CDC_IN_EP                           VIRTUAL_IN_EP1  /* EP1 for data IN */
#define CDC_OUT_EP                          VIRTUAL_OUT_EP1  /* EP1 for data OUT */
#define CDC_CMD_EP                          VIRTUAL_IN_EP2  /* EP2 for CDC commands */

#define VIRTUAL_IN_EP1                      0x84
#define VIRTUAL_OUT_EP1                     0x04

#define VIRTUAL_IN_EP2                      0x86
#define VIRTUAL_OUT_EP2                     0x06

#define VIRTUAL_IN_EP3                      0x88
#define VIRTUAL_OUT_EP3                     0x08

#define CDC_DATA_MAX_PACKET_SIZE            64   /* Endpoint IN & OUT Packet size */
#define CDC_CMD_PACKET_SZE                  8    /* Control Endpoint Packet size */

#define CDC_DATA_IN_PACKET_SIZE             CDC_DATA_MAX_PACKET_SIZE

#define CDC_DATA_OUT_PACKET_SIZE            CDC_DATA_MAX_PACKET_SIZE

#define CDC_IN_BUF_SIZE                     (CDC_DATA_IN_PACKET_SIZE*10)
#define CDC_OUT_BUF_SIZE                    (CDC_DATA_OUT_PACKET_SIZE*10) /* Total size of IN buffer:CDC_OUT_BUF_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */


#define USB_CDC_CONFIG_DESC_SIZ             (67)
#define USB_CDC_DESC_SIZ                    (67-9)


#define DEVICE_CLASS_CDC                    0x02
#define DEVICE_SUBCLASS_CDC                 0x00


#define USB_DEVICE_DESCRIPTOR_TYPE          0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE   0x02
#define USB_STRING_DESCRIPTOR_TYPE          0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE       0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE        0x05

#define STANDARD_ENDPOINT_DESC_SIZE         0x09

#define SEND_ENCAPSULATED_COMMAND           0x00
#define GET_ENCAPSULATED_RESPONSE           0x01
#define SET_COMM_FEATURE                    0x02
#define GET_COMM_FEATURE                    0x03
#define CLEAR_COMM_FEATURE                  0x04
#define SET_LINE_CODING                     0x20
#define GET_LINE_CODING                     0x21
#define SET_CONTROL_LINE_STATE              0x22
#define SEND_BREAK                          0x23
#define NO_CMD                              0xFF

#define APP_FOPS                            VCP_fops

typedef struct _CDC_IF_PROP
{
  uint16_t (*pIf_Init)     (void);
  uint16_t (*pIf_DeInit)   (void);
  uint16_t (*pIf_Ctrl)     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
  uint16_t (*pIf_DataTx)   (uint8_t* Buf, uint32_t Len);
  uint16_t (*pIf_DataRx)   (uint8_t* Buf, uint32_t Len);
}
CDC_IF_Prop_TypeDef;

struct APP_DATA_STRUCT_DEF
{
    /* 通过PC接收数据(将会通过串口发出的数据)信息 */
    struct {
        uint8_t app_in_buf[TRANS_EP_NUM][CDC_IN_BUF_SIZE];     /* 接收数据缓存 */
        volatile uint32_t app_in_buf_wptr[TRANS_EP_NUM];            /* 缓存接收数据起始偏移地址 */
        volatile uint32_t app_in_buf_rptr[TRANS_EP_NUM];           /* 缓存读取数据起始偏移地址 */
        volatile uint8_t app_in_ep_busy[TRANS_EP_NUM];
    } in_struct;
    /* 通过串口接收数据(将会通过USB发送给PC的数据)信息 */
    struct{
        uint8_t app_out_buf[TRANS_EP_NUM][CDC_OUT_BUF_SIZE];    /* 接收数据缓存 */
        volatile uint32_t app_out_buf_wptr[TRANS_EP_NUM];            /* 缓存接收数据起始偏移地址*/
        volatile uint32_t app_out_buf_rptr[TRANS_EP_NUM];           /* 缓存读取数据起始偏移地址 */
    } out_struct;
    uint8_t COM_config_cmp;  /* 串口状态(1:串口处于连接状态；0：串口处于断开状态) */
} ;

extern USBD_Class_cb_TypeDef  USBD_CDC_cb;


uint32_t CDC_DataIn (uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t CDC_DataOut(uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t CDC_GetInDataLen(uint8_t ep_num);
uint32_t CDC_GetOutDataLen(uint8_t ep_num);

#endif  // __USB_CDC_CORE_H_
