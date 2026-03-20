#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usb_dcd_init.h"

static uint8_t USBD_HID_Init(void *pdev,
                             uint8_t cfgidx);

static uint8_t USBD_HID_DeInit(void *pdev,
                               uint8_t cfgidx);

static uint8_t USBD_HID_Setup(void *pdev,
                              USB_SETUP_REQ *req);

static uint8_t *USBD_HID_GetCfgDesc(uint8_t speed, uint16_t *length);

static uint8_t USBD_HID_DataIn(void *pdev, uint8_t epnum);
static uint8_t USBD_HID_DataOut(void *pdev, uint8_t epnum);
static uint8_t USBD_EP0_RxReady(void *pdev);


USBD_Class_cb_TypeDef USBD_HID_cb =
    {
        USBD_HID_Init,
        USBD_HID_DeInit,
        USBD_HID_Setup,
        NULL,             /*EP0_TxSent*/
        USBD_EP0_RxReady, /*EP0_RxReady*/
        USBD_HID_DataIn,  /*DataIn*/
        USBD_HID_DataOut, /*DataOut*/
        NULL,             /*SOF */
        NULL,
        NULL,
        USBD_HID_GetCfgDesc,
#ifdef USB_OTG_HS_CORE
        USBD_HID_GetCfgDesc, /* use same config as per FS */
#endif
};

struct APP_DATA_STRUCT_DEF hid_app_data = {0};
uint8_t hid_out_data_buf[HID_EPOUT_SIZE];
const uint8_t hid_ep_len[TRANS_EP_NUM] = {HID_EP1_MAX_PACKET, HID_EP2_MAX_PACKET, HID_EP3_MAX_PACKET};
const uint8_t hid_ep_map[TRANS_EP_NUM] = {KEYBOARD_OUT_EP, MOUSE_OUT_EP, HID_OUT_EP};
extern uint32_t out_num;
extern uint8_t CapsLock_buff[64];

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t USBD_HID_AltSet __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t USBD_HID_Protocol __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t USBD_HID_IdleState __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB HID device Configuration Descriptor */

__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZE] __ALIGN_END =
    {
        0x09,                              // Length = 9
        USB_CONFIGURATION_DESCRIPTOR_TYPE, // DescriptorType = Configuration
        USB_HID_CONFIG_DESC_SIZE, 0x00,    // TotalLength
        B_NUM_INTERFACES,                  // NumInterfaces
        0x01,                              // ConfigurationValue = 1
        0x00,                              // iConfiguration string index(Non)
        0xc0,                              // SelfPower = 0; RemoteWakeup = 0
        0x32,                              // MaxPower = 100mA

        /*     HID    */
        // interface desc
        0x09,                          // Length = 9
        USB_INTERFACE_DESCRIPTOR_TYPE, // DescriptorType = Interface
        0x00,                          // InerfaceNumber = 0
        0x00,                          // AlternateSetting = 0
        0x02,                          // NumEndpoint = 2(bulk-IN, bulk-OUT)
        0x03,                          // Class = Human Interface Device
        0x00,                          // bInterfaceSubClass : 1=BOOT, 0=no boot
        0x00,                          // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
        0x00,                          // iInterface string index(Non)

        // HID descriptor
        0x09,                             // Length
        HID_DESCRIPTOR_TYPE,              // DescriptorType=HID
        0x10, 0x01,                       // HID Class Specification compliance ?0x10 0x01
        0x00,                             // Country localization (=none)
        0x01,                             // number of descriptors to follow
        0x22,                             // And it's a Report descriptor
        HID_INPUT_REPORT_DESC_SIZE, 0x00, // Report descriptor length

        // Endpoint desc
        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        HID_IN_EP,                    // In; Ep1
        0x03,                         // Endpoint type = interrupt
        HID_EPIN_SIZE, 0x00,          // MaxPacketSize = 64->16
        0x01,                         // 0x0a,                   //Poll

        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        HID_OUT_EP,                   // Out; Ep2
        0x03,                         // Endpoint type = interrupt
        HID_EPOUT_SIZE, 0x00,         // MaxPacketSize = 64->16
        0x01,                         // 0x0a,    DT??USB�̨���y����??��?������yusb?????��?�� @ 2018.08.28

#ifdef HID_MOUSE_KAYPAD_ENABLE
        /*      MOUSE    */
        // interface desc
        0x09,                          // Length = 9
        USB_INTERFACE_DESCRIPTOR_TYPE, // DescriptorType = Interface
        0x01,                          // InerfaceNumber = 1
        0x00,                          // AlternateSetting = 0
        0x02,                          // NumEndpoint = 2(bulk-IN, bulk-OUT)
        0x03,                          // Class = Human Interface Device
        0x01,                          // bInterfaceSubClass : 1=BOOT, 0=no boot
        0x02,                          // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
        0x00,                          // iInterface string index(Non)

        // HID descriptor
        0x09,                                // Length
        HID_DESCRIPTOR_TYPE,                 // DescriptorType=HID
        0x10, 0x01,                          // HID Class Specification compliance ?0x10 0x01
        0x00,                                // Country localization (=none)
        0x01,                                // number of descriptors to follow
        0x22,                                // And it's a Report descriptor
        USB_HID_MOUSE_REPORTDESC_SIZE, 0x00, // Report descriptor length

        // Endpoint desc
        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        MOUSE_IN_EP,                  // In; Ep1
        0x03,                         // Endpoint type = interrupt
        MOUSE_EPIN_SIZE, 0x00,        // MaxPacketSize = 64->16
        0x08,                         // 0x0a,                   //Poll

        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        MOUSE_OUT_EP,                 // Out; Ep2
        0x03,                         // Endpoint type = interrupt
        MOUSE_EPOUT_SIZE, 0x00,       // MaxPacketSize = 64->16
        0x08,                         // 0x0a,    DT??USB�̨���y����??��?������yusb?????��?�� @ 2018.08.28

        /*      KEYBOARD    */
        // interface desc
        0x09,                          // Length = 9
        USB_INTERFACE_DESCRIPTOR_TYPE, // DescriptorType = Interface
        0x02,                          // InerfaceNumber = 1
        0x00,                          // AlternateSetting = 0
        0x02,                          // NumEndpoint = 2(bulk-IN, bulk-OUT)
        0x03,                          // Class = Human Interface Device
        0x01,                          // bInterfaceSubClass : 1=BOOT, 0=no boot
        0x01,                          // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
        0x00,                          // iInterface string index(Non)

        // HID descriptor
        0x09,                                 // Length
        HID_DESCRIPTOR_TYPE,                  // DescriptorType=HID
        0x10, 0x01,                           // HID Class Specification compliance ?0x10 0x01
        0x00,                                 // Country localization (=none)
        0x01,                                 // number of descriptors to follow
        0x22,                                 // And it's a Report descriptor
        USB_HID_KEYPAD_REPORTDESC_SIZE, 0x00, // Report descriptor length

        // Endpoint desc
        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        KEYBOARD_IN_EP,               // In; Ep1
        0x03,                         // Endpoint type = interrupt
        KEYBOARD_EPIN_SIZE, 0x00,     // MaxPacketSize = 64->16
        0x08,                         // 0x0a,                   //Poll

        0x07,                         // Length = 7
        USB_ENDPOINT_DESCRIPTOR_TYPE, // DescriptorType = Endpoint
        KEYBOARD_OUT_EP,              // Out; Ep2
        0x03,                         // Endpoint type = interrupt
        KEYBOARD_EPOUT_SIZE, 0x00,    // MaxPacketSize = 64->16
        0x08,                         // 0x0a,    DT??USB�̨���y����??��?������yusb?????��?�� @ 2018.08.28
#endif
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END =
    {
#if 0
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x11,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
    0x00,
#endif
#if 1
        0x09, 0x21,                       // Length, Type
        0x11, 0x01,                       // HID Class Specification compliance ?0x10 0x01
        0x00,                             // Country localization (=none)
        0x01,                             // number of descriptors to follow
        0x22,                             // And it's a Report descriptor
        HID_MOUSE_REPORT_DESC_SIZE, 0x00, // Report descriptor length
#endif
};
#endif

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN static uint8_t HID_INPUT_ReportDesc[HID_INPUT_REPORT_DESC_SIZE] __ALIGN_END =
    {
        0x06U, 0x00U, 0xffU, /* Usage Page (Vendor defined)*/

        0x09U, 0x01U,                                   /* Usage (Vendor defined) */
        0xa1U, 0x01U,                                   /* Collection (Application) */
        0x85U, HID_REPORT_ID,                           /* Report Id(242) */
        0x95U, HID_REPORT_COUNT, /* Report Count(63) */ /*有多少个这样的字段*/
        0x75U, HID_REPORT_SIZE, /* Report Size(8) */    /*每个字段的长度，单位bit*/
        0x25U, 0xffU, /* logical Maximum (255) */       /*每个字段的逻辑大小*/
        0x15U, 0x00U,                                   /* logical Minimum (0) */

        0x09U, 0x01U, /* Usage (Vendor-defined) */
        0x81U, 0x02U, /* Input(Data, Variable, Absolute, Bit Field) */

        0x09U, 0x01U, /* Usage (Vendor-defined) */
        0x91U, 0x02U, /* Output(Data,Value,Absolute,Non-volatile,Bit Field) */

        0xC0U, /* end collection */
};

#ifdef HID_MOUSE_KAYPAD_ENABLE

#if 0
__ALIGN_BEGIN static uint8_t HID_mouse_kaypad_DESCRIPTOR[USB_HID_DESCRIPTOR_SIZE] __ALIGN_END =
{
    0x09, 0x02, 0x3B, 0x00, 0x02, 0x01, 0x00, 0xA0, 0x32, 0x09, 0x04, 0x00, 0x00
    , 0x01, 0x03, 0x01, 0x01, 0x00, 0x09, 0x21, 0x00, 0x02, 0x00, 0x01, 0x22, 0x3F
    , 0x00, 0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x02, 0x09, 0x04, 0x01, 0x00, 0x01
    , 0x03, 0x01, 0x02, 0x00, 0x09, 0x21, 0x00, 0x02, 0x00, 0x01, 0x22, 0xC3, 0x00
    , 0x07, 0x05, 0x82, 0x03, 0x40, 0x00, 0x02

} ;
#endif

#if 1
__ALIGN_BEGIN static uint8_t HID_kaypad_ReportDesc[USB_HID_KEYPAD_REPORTDESC_SIZE] __ALIGN_END =
    {
        0x05,0x01,// USAGE_PAGE (Generic Desktop)
        0x09,0x06,// USAGE (Keyboard)
        0xA1,0x01,// COLLECTION (Application)

        0x05,0x07,//   USAGE_PAGE (Keyboard Key Codes)
        0x19,0xE0,//   USAGE_MINIMUM (Keyboard LeftControl)
        0x29,0xE7,//   USAGE_MAXIMUM (Keyboard Right GUI)
        0x15,0x00,//   LOGICAL_MINIMUM (0)
        0x25,0x01,//   LOGICAL_MAXIMUM (1)
        0x75,0x01,//   REPORT_SIZE (1)
        0x95,0x08,//   REPORT_COUNT (8)
        0x81,0x02,//   INPUT (Data, Var, Abs)
        0x95,0x01,//   REPORT_COUNT (1)
        0x75,0x08,//   REPORT_SIZE (8)
        0x81,0x01,//   INPUT (Constant, Array, Abs)
        
        0x95,0x05,//   REPORT_COUNT (5)
        0x75,0x01,//   REPORT_SIZE (1)
        0x05,0x08,//   USAGE_PAGE (LEDs)
        0x19,0x01,//   USAGE_MINIMUM (Num Lock)
        0x29,0x05,//   USAGE_MAXIMUM (Kana)
        0x91,0x02,//   OUTPUT (Data, Var, Abs)
        0x95,0x01,//   REPORT_COUNT (1)
        0x75,0x03,//   REPORT_SIZE (3)
        0x91,0x01,//   OUTPUT (Constant, Array, Abs)

        0x95,0x06,//   REPORT_COUNT (6)
        0x75,0x08,//   REPORT_SIZE (8)
        0x15,0x00,//   LOGICAL_MINIMUM (0)
        0x25,0x68,//   LOGICAL_MAXIMUM (104)
        0x05,0x07,//   USAGE_PAGE (Keyboard Key Codes)
        0x19,0x00,//   USAGE_MINIMUM (0)
        0x29,0x68,//   USAGE_MAXIMUM (104)
        0x81,0x00,//   INPUT (Data, Array, Abs)
        0xC0,

};

#endif

#if 1
__ALIGN_BEGIN static uint8_t HID_mouse_ReportDesc[USB_HID_MOUSE_REPORTDESC_SIZE] __ALIGN_END =
    {
        /*Mouse pointer*/
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x02, // USAGE (Mouse)
        0xA1, 0x01, // COLLECTION (Application)
        0x85, 0x01, //   REPORT_ID (1)
        0x09, 0x01, //   USAGE (Pointer)
        0xA1, 0x00, //   COLLECTION (Physical)

        /*The five buttons of the mouse*/
        0x05, 0x09, //     USAGE_PAGE (Button)
        0x19, 0x01, //     USAGE_MINIMUM (Button 1)
        0x29, 0x05, //     USAGE_MAXIMUM (Button 5)
        0x15, 0x00, //     LOGICAL_MINIMUM (0)
        0x25, 0x01, //     LOGICAL_MAXIMUM (1)
        0x95, 0x05, //     REPORT_COUNT (5)
        0x75, 0x01, //     REPORT_SIZE (1)
        0x81, 0x02, //     INPUT (Data,Var,Abs)
        0x95, 0x01, //     REPORT_COUNT (1)
        0x75, 0x03, //     REPORT_SIZE (3)
        0x81, 0x01, //     INPUT (Constant,Array,Abs)

        /*The X and Y axes of the mouse*/
        0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
        0x09, 0x30, //     USAGE (X)
        0x09, 0x31, //     USAGE (Y)
        0x16, 0x01,0xF8, //     LOGICAL_MINIMUM (-2047)
        0x26, 0xFF, 0x07, //     LOGICAL_MAXIMUM (2047)
        0x75, 0x10, //     REPORT_SIZE (16)
        0x95, 0x02, //     REPORT_COUNT (2)
        0x81, 0x06, //     INPUT (Data,Var,Rel)

        /*Mouse wheel movement amount*/
        0x09, 0x38, //     USAGE (Wheel)
        0x15, 0x81, //     LOGICAL_MINIMUM (-127)
        0x25, 0x7F, //     LOGICAL_MAXIMUM (127)
        0x75, 0x08, //     REPORT_SIZE (8)
        0x95, 0x01, //     REPORT_COUNT (1)
        0x81, 0x06, //     INPUT (Data,Var,Rel)

        /*User-defined purpose*/
        0x05, 0x0C, //     USAGE_PAGE (Consumer)
        0x0A, 0x38, 0x02, //     USAGE (AC Pan)
        0x95, 0x01, //     REPORT_COUNT (1)
        0x81, 0x06, //     REPORT_SIZE (8)
        0xC0, //   END_COLLECTION
        0xC0 // END_COLLECTION 
    };
#endif
#endif /* HID_MOUSE_KAYPAD_ENABLE */

/**
 * @brief  USBD_HID_Init
 *         Initialize the HID interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_HID_Init(void *pdev,
                             uint8_t cfgidx)
{

    memset((uint8_t *)&hid_app_data, 0, sizeof(hid_app_data));
#ifdef HID_MOUSE_KAYPAD_ENABLE
    /* Open KEYBOARD EP IN */
    DCD_EP_Open(pdev,
                KEYBOARD_IN_EP,
                KEYBOARD_EPIN_SIZE,
                USB_OTG_EP_INT);

    /* Open KEYBOARD EP OUT */
    DCD_EP_Open(pdev,
                KEYBOARD_OUT_EP,
                KEYBOARD_EPOUT_SIZE,
                USB_OTG_EP_INT);
    /* Open MOUSE EP IN */
    DCD_EP_Open(pdev,
                MOUSE_IN_EP,
                MOUSE_EPIN_SIZE,
                USB_OTG_EP_INT);

    /* Open MOUSE EP OUT */
    DCD_EP_Open(pdev,
                MOUSE_OUT_EP,
                MOUSE_EPOUT_SIZE,
                USB_OTG_EP_INT);
#endif
    /* Open EP IN */
    DCD_EP_Open(pdev,
                HID_IN_EP,
                HID_EPIN_SIZE,
                USB_OTG_EP_INT);

    /* Open EP OUT */
    DCD_EP_Open(pdev,
                HID_OUT_EP,
                HID_EPOUT_SIZE,
                USB_OTG_EP_INT);

    return USBD_OK;
}

/**
 * @brief  USBD_HID_Init
 *         DeInitialize the HID layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_HID_DeInit(void *pdev,
                               uint8_t cfgidx)
{
#ifdef HID_MOUSE_KAYPAD_ENABLE
    /* Close keyboard EPs */
    DCD_EP_Close(pdev, KEYBOARD_IN_EP);
    DCD_EP_Close(pdev, KEYBOARD_OUT_EP);

    /* Close mouse EPs */
    DCD_EP_Close(pdev, MOUSE_IN_EP);
    DCD_EP_Close(pdev, MOUSE_OUT_EP);

#endif
    /* Close HID EPs */
    DCD_EP_Close(pdev, HID_IN_EP);
    DCD_EP_Close(pdev, HID_OUT_EP);

    return USBD_OK;
}

/**
 * @brief  USBD_HID_Setup
 *         Handle the HID specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
static uint8_t USBD_HID_Setup(void *pdev,
                              USB_SETUP_REQ *req)
{

    uint16_t len = 0;
    uint8_t *pbuf = NULL;
    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest)
        {
        case HID_REQ_SET_REPORT:
            CapsLockLen = req->wLength;
            USBD_CtlPrepareRx(pdev, CapsLock, CapsLockLen);
            usb0_clear_out_nak();
            break;
        case HID_REQ_SET_PROTOCOL:
            USBD_HID_Protocol = (uint8_t)(req->wValue);
            break;
        case HID_REQ_GET_PROTOCOL:
            pbuf = (uint8_t *)&USBD_HID_Protocol;
            len = 1;
            break;

        case HID_REQ_SET_IDLE:
            USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
            USB_OTG_dev.dev.device_status_new = USB_OTG_END;
            break;
        case HID_REQ_GET_IDLE:
            pbuf = (uint8_t *)&USBD_HID_IdleState;
            len = 1;
            break;

        default:
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_DESCRIPTOR:
            if (req->wValue >> 8 == HID_REPORT_DESC)
            {
#ifdef HID_MOUSE_KAYPAD_ENABLE
                if (LOBYTE(req->wIndex) == 2)
                {
                    len = MIN(USB_HID_KEYPAD_REPORTDESC_SIZE, req->wLength);
                    pbuf = HID_kaypad_ReportDesc;
                }
                if (LOBYTE(req->wIndex) == 1)
                {
                    len = MIN(USB_HID_MOUSE_REPORTDESC_SIZE, req->wLength);
                    pbuf = HID_mouse_ReportDesc;
                }
#endif
                if (LOBYTE(req->wIndex) == 0)
                {
                    len = MIN(HID_INPUT_REPORT_DESC_SIZE, req->wLength);
                    pbuf = HID_INPUT_ReportDesc;
                }
            }
            else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
            {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
                pbuf = USBD_HID_Desc;
                len = MIN(USB_HID_DESC_SIZ, req->wLength);
#else
#ifdef HID_MOUSE_KAYPAD_ENABLE

                if (LOBYTE(req->wIndex) == 2)
                {
                    len = MIN(HID_KEYBARD_DESC_SIZE, req->wLength);
                    pbuf = USBD_HID_CfgDesc + 75;
                }
                else if (LOBYTE(req->wIndex) == 1)
                {
                    len = MIN(HID_MOUSE_DESC_SIZE, req->wLength);
                    pbuf = USBD_HID_CfgDesc + 50;
                }
#endif
                if (LOBYTE(req->wIndex) == 0)
                {
                    pbuf = USBD_HID_CfgDesc + 18;
                    len = MIN(USB_HID_DESC_SIZ, req->wLength);
                }
#endif
            }
            break;

        case USB_REQ_GET_INTERFACE:
            pbuf = (uint8_t *)USBD_HID_AltSet;
            len = 1;
            break;

        case USB_REQ_SET_INTERFACE:
            USBD_HID_AltSet = (uint8_t)(req->wValue);
            break;
        }
    }
    if ((len != 0) && (req->wLength != 0))
    {
        len = MIN(len, req->wLength);
        USBD_CtlSendData(pdev,
                         pbuf,
                         len);
    }
    return USBD_OK;
}

/**
 * @brief  USBD_HID_GetCfgDesc
 *         return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t *USBD_HID_GetCfgDesc(uint8_t speed, uint16_t *length)
{
    *length = sizeof(USBD_HID_CfgDesc);
    return USBD_HID_CfgDesc;
    return 0;
}

static uint8_t HID_GetEpIndex(uint8_t epNum)
{
    for (uint8_t i = 0; i < TRANS_EP_NUM; i++)
    {
        if (hid_ep_map[i] == (epNum & 0x7f))
        {
            return i;
        }
    }
    return 0;
}

/**
 * @brief  USBD_HID_DataIn
 *         handle data IN Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_HID_DataIn(void *pdev,
                               uint8_t epnum)
{
    uint8_t hid_ep_index = HID_GetEpIndex(epnum);
    uint8_t hid_pack_size = hid_ep_len[hid_ep_index];
    if (HID_GetInDataLen(epnum) >= hid_pack_size)
    {
        int pack_rptr = hid_app_data.in_struct.app_in_buf_rptr[hid_ep_index];
        hid_app_data.in_struct.app_in_buf_rptr[hid_ep_index] += hid_pack_size;
        if (hid_app_data.in_struct.app_in_buf_rptr[hid_ep_index] >= APP_IN_BUF_LEN)
        {
            hid_app_data.in_struct.app_in_buf_rptr[hid_ep_index] -= APP_IN_BUF_LEN;
        }
        uint32_t sys_irq_status = __disable_irq();
        hid_app_data.in_struct.app_in_ep_busy[hid_ep_index] = 1;
        __enable_irq(sys_irq_status);
        DCD_EP_Tx(pdev, epnum, &hid_app_data.in_struct.app_in_buf[hid_ep_index][pack_rptr], hid_pack_size);
    }
    else
    {
        uint32_t sys_irq_status = __disable_irq();
        hid_app_data.in_struct.app_in_ep_busy[hid_ep_index] = 0;
        __enable_irq(sys_irq_status);
    }
    return USBD_OK;
}

/**
 * @brief  USBD_HID_DataOut
 *         Data received on non-control Out endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t USBD_HID_DataOut(void *pdev, uint8_t epnum)
{
    uint8_t ep_index = HID_GetEpIndex(epnum);
    uint8_t ep_packet_len = rx_DataLength;

    DCD_EP_PrepareRx(pdev, epnum, hid_out_data_buf, ep_packet_len);

    uint32_t index = hid_app_data.out_struct.app_out_buf_wptr[ep_index];
    for (int i = 0; i < ((USB_OTG_CORE_HANDLE *)pdev)->dev.out_ep[epnum & 0x7f].xfer_len; i++)
    {
        hid_app_data.out_struct.app_out_buf[ep_index][index++] = hid_out_data_buf[i];
        if (index >= APP_OUT_BUF_LEN)
        {
            index = 0;
        }
    }
    hid_app_data.out_struct.app_out_buf_wptr[ep_index] = index;

    if ((HID_GetOutDataLen(epnum) + HID_EP3_MAX_PACKET + rx_DataLength) < (APP_OUT_BUF_LEN))
    {
        CORE_USB->reply0.bit.reply0 = 1 << (HID_OUT_EP * 2 + 1);
    }
    return USBD_OK;
}

/**
 * @brief  send hid data(put data into send buffer)
 * @param  ep: the endpoint of send data
 * @param  buf: Buffer of data to be send
 * @param  len: Number of data to be send (in bytes)
 * @retval send(put) len
 */
uint32_t HID_DataIn(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint8_t ep_index = HID_GetEpIndex(ep);

    if (USB_OTG_dev.dev.device_status_new != USB_OTG_END || len == 0)
    {
        return 0;
    }

    while ((APP_IN_BUF_LEN - HID_GetInDataLen(ep)) <= len)
        ; // 不能正好放满

    uint32_t index = hid_app_data.in_struct.app_in_buf_wptr[ep_index];

    for (int i = 0; i < len; i++)
    {
        /* Detect whether the buffer overflow */
        hid_app_data.in_struct.app_in_buf[ep_index][index++] = buf[i];
        /* To avoid buffer overflow */
        if (index >= APP_IN_BUF_LEN)
        {
            index = 0;
        }
    }
    hid_app_data.in_struct.app_in_buf_wptr[ep_index] = index;
    if (hid_app_data.in_struct.app_in_ep_busy[ep_index] == 0)
    {
        USBD_DCD_INT_fops->DataInStage(&USB_OTG_dev, ep);
    }

    // while (HREAD(CORE_USB_TX_BUSY) & (1 << (ep_num & 0x7f)));
    return len;
}

/**
 * @brief  read hid out data
 * @param  ep: the endpoint of read data
 * @param  buf: Buffer addr
 * @param  len: read len
 * @retval readed len
 */
uint32_t HID_DataOut(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    uint8_t ep_index = HID_GetEpIndex(ep);

    while ((hid_app_data.out_struct.app_out_buf_rptr[ep_index] != hid_app_data.out_struct.app_out_buf_wptr[ep_index]) && (i < len))
    {
        buf[i++] = hid_app_data.out_struct.app_out_buf[ep_index][hid_app_data.out_struct.app_out_buf_rptr[ep_index]++];
        if (hid_app_data.out_struct.app_out_buf_rptr[ep_index] >= APP_OUT_BUF_LEN)
        {
            hid_app_data.out_struct.app_out_buf_rptr[ep_index] = 0;
        }
    }
    return i;
}

/**
 * @brief  Get_TxBuf_length
 *         Get the length of the remaining data to be transmitted
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t HID_GetInDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = HID_GetEpIndex(ep_num);
    if (hid_app_data.in_struct.app_in_buf_rptr[ep_index] <= hid_app_data.in_struct.app_in_buf_wptr[ep_index])
    {
        ret = hid_app_data.in_struct.app_in_buf_wptr[ep_index] - hid_app_data.in_struct.app_in_buf_rptr[ep_index];
    }
    else
    {
        ret = APP_IN_BUF_LEN - (hid_app_data.in_struct.app_in_buf_rptr[ep_index] - hid_app_data.in_struct.app_in_buf_wptr[ep_index]);
    }
    return ret;
}

/**
 * @brief  Get receive data length
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t HID_GetOutDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = HID_GetEpIndex(ep_num);
    if (hid_app_data.out_struct.app_out_buf_rptr[ep_index] <= hid_app_data.out_struct.app_out_buf_wptr[ep_index])
    {
        ret = hid_app_data.out_struct.app_out_buf_wptr[ep_index] - hid_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        ret = APP_OUT_BUF_LEN + hid_app_data.out_struct.app_out_buf_wptr[ep_index] - hid_app_data.out_struct.app_out_buf_rptr[ep_index];
    }

    return ret;
}

static uint8_t USBD_EP0_RxReady(void *pdev)
{
    USB_OTG_ReadPacket(pdev, CapsLock, 0, CapsLockLen);
    return USBD_OK;
}
