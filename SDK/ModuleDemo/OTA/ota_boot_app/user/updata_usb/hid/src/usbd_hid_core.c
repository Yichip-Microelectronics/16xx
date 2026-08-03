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


USBD_Class_cb_TypeDef USBD_HID_cb =
    {
        USBD_HID_Init,
        USBD_HID_DeInit,
        USBD_HID_Setup,
        NULL,             /*EP0_TxSent*/
        NULL,             /*EP0_RxReady*/
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
const uint8_t hid_ep_len[TRANS_EP_NUM] = {HID_EP_MAX_PACKET};
const uint8_t hid_ep_map[TRANS_EP_NUM] = {HID_OUT_EP};

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
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,                              // Length = 9
        USB_CONFIGURATION_DESCRIPTOR_TYPE, // DescriptorType = Configuration
        USB_HID_CONFIG_DESC_SIZ, 0x00,     // TotalLength
        0x01,                              // NumInterfaces = 1
        0x01,                              // ConfigurationValue = 1
        0x00,                              // iConfiguration string index(Non)
        0xc0,                              // SelfPower = 0; RemoteWakeup = 0
        0x32,                              // MaxPower = 100mA

        // interface desc
        0x09,                          // Length = 9
        USB_INTERFACE_DESCRIPTOR_TYPE, // DescriptorType = Interface
        0x00,                          // InerfaceNumber = 0
        0x00,                          // AlternateSetting = 0
        0x02,                          // NumEndpoint = 2(bulk-IN, bulk-OUT)
        0x03,                          // Class = Human Interface Device
        0x00,                          // InterfaceSubClass = 0x00(No subclass)
        0x00,                          // InterfaceProtocol = 0x00(None)
        0x00,                          // iInterface string index(Non)

        // HID descriptor
        0x09, 0x21,                       // Length, Type
        0x01, 0x01,                       // HID Class Specification compliance ?0x10 0x01
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
};
__ALIGN_BEGIN static uint8_t HID_INPUT_ReportDesc[HID_INPUT_REPORT_DESC_SIZE] __ALIGN_END =
{
    0x05U, 0x81U, /* Usage Page (Vendor defined)*/
    0x09U, 0x82U, /* Usage (Vendor defined) */
    0xA1U, 0x01U, /* Collection (Application) */
    0x09U, 0x83U, /* Usage (Vendor defined) */

    0x09U, 0x84U, /* Usage (Vendor defined) */
    0x15U, 0x80U, /* logical Minimum (-128) */
    0x25U, 0x7FU, /* logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 64, /* Report Count (8U) */
    0x81U, 0x02U, /* Input(Data, Variable, Absolute) */

    0x09U, 0x84U, /* Usage (Vendor defined) */
    0x15U, 0x80U, /* logical Minimum (-128) */
    0x25U, 0x7FU, /* logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 64, /* Report Count (8U) */
    0x91U, 0x02U, /* Input(Data, Variable, Absolute) */
    0xC0U,        /* end collection */
};


/**
  * @}
  */

/** @defgroup USBD_HID_Private_Functions
  * @{
  */

/**
 * @brief  USBD_HID_Init
 *         Initialize the HID interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_HID_Init(void *pdev,uint8_t cfgidx)
{
    memset((uint8_t *)&hid_app_data, 0, sizeof(hid_app_data));
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
static uint8_t USBD_HID_DeInit(void *pdev,uint8_t cfgidx)
{
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
static uint8_t USBD_HID_Setup(void *pdev,USB_SETUP_REQ *req)
{

    uint16_t len = 0;
    uint8_t *pbuf = NULL;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest)
        {

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
                len = MIN(HID_INPUT_REPORT_DESC_SIZE, req->wLength);
                pbuf = HID_INPUT_ReportDesc;
            }
            else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
            {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
                pbuf = USBD_HID_Desc;
#else
                pbuf = USBD_HID_CfgDesc + 0x12;
#endif
                len = MIN(USB_HID_DESC_SIZ, req->wLength);
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
static uint8_t USBD_HID_DataIn(void *pdev,uint8_t epnum)
{
    uint8_t hid_ep_index = epnum-1;
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
    if((ep_packet_len+index)>(APP_OUT_BUF_LEN-1))
    {
        memcpy(hid_app_data.out_struct.app_out_buf[ep_index]+index,hid_out_data_buf,APP_OUT_BUF_LEN-index);
        memcpy(hid_app_data.out_struct.app_out_buf[ep_index],hid_out_data_buf+APP_OUT_BUF_LEN-index,ep_packet_len+index-APP_IN_BUF_LEN);
        index=ep_packet_len+index-APP_OUT_BUF_LEN;
    }
    else
    {
        memcpy(hid_app_data.out_struct.app_out_buf[ep_index]+index,hid_out_data_buf,ep_packet_len);
        index=index+ep_packet_len;
    }
    hid_app_data.out_struct.app_out_buf_wptr[ep_index] = index;
    if (( APP_OUT_BUF_LEN- HID_GetOutDataLen(epnum)  ) <= (HID_EP_MAX_PACKET*2))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy |= (1<<ep_index);
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
    while ((APP_IN_BUF_LEN - HID_GetInDataLen(ep)) <= len); // 不能正好放满
    uint32_t index = hid_app_data.in_struct.app_in_buf_wptr[ep_index];
    if((len+index)>(APP_IN_BUF_LEN-1))
    {
        memcpy(hid_app_data.in_struct.app_in_buf[ep_index]+index,buf,APP_IN_BUF_LEN-index);
        memcpy(hid_app_data.in_struct.app_in_buf[ep_index],buf+APP_IN_BUF_LEN-index,len+index-APP_IN_BUF_LEN);
        index=len+index-APP_IN_BUF_LEN;
    }
    else
    {
        memcpy(hid_app_data.in_struct.app_in_buf[ep_index]+index,buf,len);
        index=index+len;
    }
    hid_app_data.in_struct.app_in_buf_wptr[ep_index] = index;
    if (hid_app_data.in_struct.app_in_ep_busy[ep_index] == 0)
    {
        USBD_DCD_INT_fops->DataInStage(&USB_OTG_dev, ep_index+1);
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
    if((hid_app_data.out_struct.app_out_buf_rptr[ep_index]+len)<APP_OUT_BUF_LEN)
    {
        memcpy(buf,hid_app_data.out_struct.app_out_buf[ep_index]+hid_app_data.out_struct.app_out_buf_rptr[ep_index],len);
        hid_app_data.out_struct.app_out_buf_rptr[ep_index]=len+hid_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        memcpy(buf,hid_app_data.out_struct.app_out_buf[ep_index]+hid_app_data.out_struct.app_out_buf_rptr[ep_index],APP_OUT_BUF_LEN-hid_app_data.out_struct.app_out_buf_rptr[ep_index]);
        memcpy(buf+APP_OUT_BUF_LEN-hid_app_data.out_struct.app_out_buf_rptr[ep_index],hid_app_data.out_struct.app_out_buf[ep_index],len+hid_app_data.out_struct.app_out_buf_rptr[ep_index]-APP_OUT_BUF_LEN);
        hid_app_data.out_struct.app_out_buf_rptr[ep_index]=len+hid_app_data.out_struct.app_out_buf_rptr[ep_index]-APP_OUT_BUF_LEN;
    }
    if(((CORE_USB->usb_ep_busy.bit.usb_ep_busy&(1<<ep_index))!=0)&&(( APP_OUT_BUF_LEN- HID_GetOutDataLen(ep)  ) > (HID_EP_MAX_PACKET*2)))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy &= ~(1<<ep_index);
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
