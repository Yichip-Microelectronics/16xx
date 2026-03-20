#ifndef __USB_PRINTER_CORE_H_
#define __USB_PRINTER_CORE_H_

#include  "usbd_ioreq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANS_EP_NUM                                1  /*应用数据传输端点个数,用于申请及管理中转buffer*/

#define PRINTER_IN_EP                               0x86  /* EP1 for data IN */
#define PRINTER_OUT_EP                              0x06  /* EP1 for data OUT */

#define USB_PRINTER_CONFIG_DESC_SIZ                 (32)
#define USB_PRINTER_DESC_SIZ                        (USB_PRINTER_CONFIG_DESC_SIZ-9)

#define PRINTER_DESCRIPTOR_TYPE                     0x21

#define DEVICE_CLASS_PRINTER                        0x02
#define DEVICE_SUBCLASS_PRINTER                     0x00


#define USB_DEVICE_DESCRIPTOR_TYPE                  0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE           0x02
#define USB_STRING_DESCRIPTOR_TYPE                  0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE               0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE                0x05

#define STANDARD_ENDPOINT_DESC_SIZE                 0x09

#define PRINTER_DATA_MAX_PACKET_SIZE                64   /* Endpoint IN & OUT Packet size */
#define PRINTER_DATA_IN_PACKET_SIZE                 PRINTER_DATA_MAX_PACKET_SIZE
#define PRINTER_DATA_OUT_PACKET_SIZE                PRINTER_DATA_MAX_PACKET_SIZE

#define PRINTER_APP_OUT_BUF_SIZE                     (PRINTER_DATA_MAX_PACKET_SIZE*10)

#define PRINTER_APP_IN_BUF_SIZE                     (PRINTER_DATA_MAX_PACKET_SIZE*5)

#define SEND_ENCAPSULATED_COMMAND                  0x00
#define GET_ENCAPSULATED_RESPONSE                  0x01
#define SET_COMM_FEATURE                           0x02
#define GET_COMM_FEATURE                           0x03
#define CLEAR_COMM_FEATURE                         0x04
#define SET_LINE_CODING                            0x20
#define GET_LINE_CODING                            0x21
#define SET_CONTROL_LINE_STATE                     0x22
#define SEND_BREAK                                 0x23
#define NO_CMD                                     0xFF

struct APP_DATA_STRUCT_DEF
{
    /* 通过PC接收数据(将会通过串口发出的数据)信息 */
    struct
    {
        uint8_t app_in_buf[TRANS_EP_NUM][PRINTER_APP_IN_BUF_SIZE];     // 接收数据缓存
        volatile uint32_t app_in_buf_wptr[TRANS_EP_NUM];            // 缓存接收数据起始偏移地址
        volatile uint32_t app_in_buf_rptr[TRANS_EP_NUM];           // 缓存读取数据起始偏移地址
        volatile uint8_t app_in_ep_busy[TRANS_EP_NUM];
    } in_struct;
    /* 通过串口接收数据(将会通过USB发送给PC的数据)信息 */
    struct
    {
        uint8_t app_out_buf[TRANS_EP_NUM][PRINTER_APP_OUT_BUF_SIZE];    // 接收数据缓存
        volatile uint32_t app_out_buf_wptr[TRANS_EP_NUM];            // 缓存接收数据起始偏移地址
        volatile uint32_t app_out_buf_rptr[TRANS_EP_NUM];           // 缓存读取数据起始偏移地址
    } out_struct;
} ;


extern USBD_Class_cb_TypeDef  USBD_PRINTER_cb;


uint32_t PRINTER_DataIn (uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t PRINTER_DataOut(uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t PRINTER_GetInDataLen(uint8_t ep_num);
uint32_t PRINTER_GetOutDataLen(uint8_t ep_num);


#ifdef __cplusplus
}
#endif

#endif   /*__USBD_PRINTER_CORE_H_*/
