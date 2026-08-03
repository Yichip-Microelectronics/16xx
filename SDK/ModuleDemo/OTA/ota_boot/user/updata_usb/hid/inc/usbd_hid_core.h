#ifndef __USB_HID_CORE_H_
#define __USB_HID_CORE_H_

#include  "usbd_ioreq.h"

#define TRANS_EP_NUM                1  /*应用数据传输端点个数,用于申请及管理中转buffer*/
#define HID_IN_EP                   0x85
#define HID_OUT_EP                  0x05
#define HID_REPORT_ID               0xf2
#define HID_REPORT_COUNT            0x3f
#define HID_REPORT_SIZE             8

#define HID_EP_MAX_PACKET           64

#define HID_EPIN_SIZE               HID_EP_MAX_PACKET
#define HID_EPOUT_SIZE              HID_EP_MAX_PACKET

#define USB_HID_CONFIG_DESC_SIZ     41
#define USB_HID_DESC_SIZ            9
#define HID_INPUT_REPORT_DESC_SIZE  33

#define HID_DESCRIPTOR_TYPE         0x21
#define HID_REPORT_DESC             0x22


#define HID_REQ_SET_PROTOCOL        0x0B
#define HID_REQ_GET_PROTOCOL        0x03

#define HID_REQ_SET_IDLE            0x0A
#define HID_REQ_GET_IDLE            0x02

#define HID_REQ_SET_REPORT          0x09
#define HID_REQ_GET_REPORT          0x01

#define APP_IN_BUF_LEN              (HID_EP_MAX_PACKET * 10)
#define APP_OUT_BUF_LEN             (HID_EP_MAX_PACKET * 10)

struct APP_DATA_STRUCT_DEF
{
    struct {
        uint8_t app_in_buf[TRANS_EP_NUM][APP_IN_BUF_LEN];
        volatile uint32_t app_in_buf_wptr[TRANS_EP_NUM];
        volatile uint32_t app_in_buf_rptr[TRANS_EP_NUM];
        volatile uint8_t app_in_ep_busy[TRANS_EP_NUM];
    } in_struct;

    struct{
        uint8_t app_out_buf[TRANS_EP_NUM][APP_OUT_BUF_LEN];
        volatile uint32_t app_out_buf_wptr[TRANS_EP_NUM];
        volatile uint32_t app_out_buf_rptr[TRANS_EP_NUM];
    } out_struct;
} ;

extern struct APP_DATA_STRUCT_DEF hid_app_data;

extern USBD_Class_cb_TypeDef  USBD_HID_cb;

uint32_t HID_DataIn (uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t HID_DataOut(uint8_t ep, uint8_t* buf, uint32_t len);
uint32_t HID_GetInDataLen(uint8_t ep_num);
uint32_t HID_GetOutDataLen(uint8_t ep_num);
#endif  // __USB_HID_CORE_H_
