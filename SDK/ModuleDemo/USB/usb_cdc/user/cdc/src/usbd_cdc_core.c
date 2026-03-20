#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usb_dcd_init.h"
#include "usbd_cdc_vcp.h"

static uint8_t usbd_cdc_Init(void *pdev, uint8_t cfgidx);
static uint8_t usbd_cdc_DeInit(void *pdev, uint8_t cfgidx);
static uint8_t usbd_cdc_Setup(void *pdev, USB_SETUP_REQ *req);
static uint8_t usbd_cdc_EP0_RxReady(void *pdev);
static uint8_t usbd_cdc_DataIn(void *pdev, uint8_t epnum);
static uint8_t usbd_cdc_DataOut(void *pdev, uint8_t epnum);
static uint8_t usbd_cdc_SOF(void *pdev);
static uint8_t *USBD_cdc_GetCfgDesc(uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS
static uint8_t *USBD_cdc_GetOtherCfgDesc(uint8_t speed, uint16_t *length);
#endif

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_cdc_CfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_cdc_OtherCfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t usbd_cdc_AltSet __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t CmdBuff[CDC_CMD_PACKET_SZE] __ALIGN_END;

struct APP_DATA_STRUCT_DEF cdc_app_data = {0};
const uint8_t cdc_ep_len[TRANS_EP_NUM] = {CDC_DATA_MAX_PACKET_SIZE,CDC_DATA_MAX_PACKET_SIZE,CDC_DATA_MAX_PACKET_SIZE};
const uint8_t cdc_ep_map[TRANS_EP_NUM] = {VIRTUAL_OUT_EP1,VIRTUAL_OUT_EP2,VIRTUAL_OUT_EP3};
extern LINE_CODING linecoding;
extern CDC_IF_Prop_TypeDef APP_FOPS;
extern USB_OTG_CORE_HANDLE USB_OTG_dev;

static uint32_t cdcCmd = 0xFF;
static uint32_t cdcLen = 0;

/* CDC interface class callbacks structure */
USBD_Class_cb_TypeDef USBD_CDC_cb =
    {
        usbd_cdc_Init,
        usbd_cdc_DeInit,
        usbd_cdc_Setup,
        NULL, /* EP0_TxSent, */
        usbd_cdc_EP0_RxReady,
        usbd_cdc_DataIn,
        usbd_cdc_DataOut,
        usbd_cdc_SOF,
        NULL,
        NULL,
        USBD_cdc_GetCfgDesc,
#ifdef USE_USB_OTG_HS
        USBD_cdc_GetOtherCfgDesc, /* use same cobfig as per FS */
#endif                            /* USE_USB_OTG_HS  */
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t usbd_cdc_CfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09,/* bLength: Configuation Descriptor size */
        0x02, /*bDescriptor Type: Configuration descriptor type*/
        0x43, 0x00, /*wTotalLength: Total length of data returned by the configuration*/
        0x02, /*bNumInterfaces: Configure the number of supported interfaces*/
        0x01, /*bConfigurationValue: Configuration values for Get Configuration and Set Configuration requests*/
        0x00, /*iConfiguration: String Descriptor Index*/
        0xC0, /*bmAttributes: Configuration features*/
        0x32, /*bMaxPower: The maximum power consumption obtained from the bus by the device.*/
        
        /*Interface Descriptor */
        0x09, /* bLength: Configuation Descriptor size */
        0x04, /*bDescriptor Type: Interface descriptor type*/
        0x00, /*bInterfaceNumber: Interface number*/
        0x00, /*bAlternateSetting: The number used to confirm the alternative settings for bInterfaceNumber.*/
        0x01, /*bNumEndpoints: The number of endpoints used by the interface*/
        0x02, /*bInterfaceClass: Class code*/
        0x02, /*bInterfaceSubClass: Subclass code*/
        0x01, /*bInterfaceProtocol: protocol code*/
        0x00, /*iInterface: Index of the interface string descriptor*/
        
        /*Header Functional Descriptor*/
        0x05, /*bLength: Configuation Descriptor size*/
        0x24, /*bDescriptor Type: CS_INTERFACE*/
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10,0x01, /* bcdCDC: spec release number */
        
        /*Call Management Functional Descriptor*/
        0x05, /*bLength: Configuation Descriptor size*/
        0x24, /*bDescriptor Type: CS_INTERFACE*/
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: This configuration supports capabilities D0 D1*/
        0x00, /*bDataInterface: Optional interface number for data class interfaces used in call management.*/

        /*ACM Functional Descriptor*/
        0x04, /*bLength: Configuation Descriptor size*/
        0x24, /*bDescriptor Type: CS_INTERFACE*/
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities: This configuration supports capabilities D0 D1 D2 D3*/
        
        /*Union Functional Descriptor*/
        0x05, /*bLength: Configuation Descriptor size*/
        0x24, /*bDescriptor Type:*/
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication or data interface number as the main control or control interface. */
        0x01, /* bSlaveInterface0: The interface number of the first subordinate or associated interface. */
        
        /*Endpoint 2 Descriptor*/
        0x07, /*bLength: Configuation Descriptor size*/
        0x05, /*bDescriptor Type: Endpoint*/
        CDC_CMD_EP, /* bEndpointAddress: Endpoint address of the device*/
        0x03, /* bmAttributes: Attributes of the endpoint */
        0x08,0x00,/* wMaxPacketSize: The maximum packet size supported by this endpoint*/
        0xFF, /* bInterval: Interval for data transmission at endpoints*/
        
        /*Data class interface descriptor*/
        0x09, /*bLength: Configuation Descriptor size*/
        0x04, /*bDescriptor Type: Interface descriptor type*/
        0x01, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: The backup settings number for this interface */
        0x02, /* bNumEndpoints: The number of endpoints used by this interface */
        0x0A, /* bInterfaceClass: Class code */
        0x00, /* bInterfaceSubClass: Subclass code*/
        0x00, /* bInterfaceProtocol: protocol code*/
        0x00, /* iInterface: Index of the interface string descriptor*/
        
        /*Endpoint OUT Descriptor*/
        0x07, /*bLength: Configuation Descriptor size*/
        0x05, /*bDescriptor Type: Endpoint*/
        CDC_OUT_EP, /* bEndpointAddress: Endpoint address of the device*/
        0x02, /* bmAttributes: Bulk */
        0x40,0x00,/* wMaxPacketSize: The maximum packet size supported by this endpoint*/
        0x00, /* bInterval: ignore for Bulk transfer */
        
        /*Endpoint IN Descriptor*/
        0x07,/*bLength: Configuation Descriptor size*/
        0x05,/*bDescriptor Type: Endpoint*/
        CDC_IN_EP,/* bEndpointAddress: Endpoint address of the device*/
        0x02,/* bmAttributes: Bulk */
        0x40,0x00,/* wMaxPacketSize: The maximum packet size supported by this endpoint*/
        0x00,/* bInterval: ignore for Bulk transfer */
    };

#ifdef USE_USB_OTG_HS
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_cdc_OtherCfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        0x09, /* bLength: Configuation Descriptor size */
        USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
        USB_CDC_CONFIG_DESC_SIZ,
        0x00,
        0x02, /* bNumInterfaces: 2 interfaces */
        0x01, /* bConfigurationValue: */
        0x04, /* iConfiguration: */
        0xC0, /* bmAttributes: */
        0x32, /* MaxPower 100 mA */

        /*Interface Descriptor */
        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x01, /* bNumEndpoints: One endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */

        /*Header Functional Descriptor*/
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,

        /*Call Management Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */

        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */

        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x00, /* bMasterInterface: Communication class interface */
        0x01, /* bSlaveInterface0: Data Class Interface */

        /*Endpoint 2 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        CDC_CMD_EP,                   /* bEndpointAddress */
        0x03,                         /* bmAttributes: Interrupt */
        LOBYTE(CDC_CMD_PACKET_SZE),   /* wMaxPacketSize: */
        HIBYTE(CDC_CMD_PACKET_SZE),
        0xFF, /* bInterval: */

        /*---------------------------------------------------------------------------*/

        /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        0x01,                          /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */

        /*Endpoint OUT Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        CDC_OUT_EP,                   /* bEndpointAddress */
        0x02,                         /* bmAttributes: Bulk */
        0x40,                         /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */

        /*Endpoint IN Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        CDC_IN_EP,                    /* bEndpointAddress */
        0x02,                         /* bmAttributes: Bulk */
        0x40,                         /* wMaxPacketSize: */
        0x00,
        0x00 /* bInterval */
};
#endif /* USE_USB_OTG_HS  */

/**
 * @}
 */

/** @defgroup usbd_cdc_Private_Functions
 * @{
 */

/**
 * @brief  USBD_cdc_GetCfgDesc
 *         Return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t *USBD_cdc_GetCfgDesc(uint8_t speed, uint16_t *length)
{
    *length = sizeof(usbd_cdc_CfgDesc);
    return usbd_cdc_CfgDesc;
}

/**
 * @brief  USBD_cdc_GetCfgDesc
 *         Return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
#ifdef USE_USB_OTG_HS
static uint8_t *USBD_cdc_GetOtherCfgDesc(uint8_t speed, uint16_t *length)
{
    *length = sizeof(usbd_cdc_OtherCfgDesc);
    return usbd_cdc_OtherCfgDesc;
}
#endif

/**
 * @brief  usbd_cdc_Init
 *         Initilaize the CDC interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t usbd_cdc_Init(void *pdev,
                             uint8_t cfgidx)
{
    uint8_t cdc_packet_buf[CDC_DATA_OUT_PACKET_SIZE];
    /* Open EP IN */
    DCD_EP_Open(pdev,
                CDC_IN_EP,
                CDC_DATA_IN_PACKET_SIZE,
                USB_OTG_EP_BULK);

    /* Open EP OUT */
    DCD_EP_Open(pdev,
                CDC_OUT_EP,
                CDC_DATA_OUT_PACKET_SIZE,
                USB_OTG_EP_BULK);

    /* Open Command IN EP */
    DCD_EP_Open(pdev,
                CDC_CMD_EP,
                CDC_CMD_PACKET_SZE,
                USB_OTG_EP_INT);

    /* Initialize the Interface physical components */
    APP_FOPS.pIf_Init();

    /* Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev,
                     CDC_OUT_EP,
                     (uint8_t *)(cdc_packet_buf),
                     CDC_DATA_OUT_PACKET_SIZE);

    return USBD_OK;
}

/**
 * @brief  usbd_cdc_Init
 *         DeInitialize the CDC layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t usbd_cdc_DeInit(void *pdev,
                               uint8_t cfgidx)
{
    /* Open EP IN */
    DCD_EP_Close(pdev,
                 CDC_IN_EP);

    /* Open EP OUT */
    DCD_EP_Close(pdev,
                 CDC_OUT_EP);

    /* Open Command IN EP */
    DCD_EP_Close(pdev,
                 CDC_CMD_EP);

    /* Restore default state of the Interface physical components */
    APP_FOPS.pIf_DeInit();

    return USBD_OK;
}

/**
 * @brief  usbd_cdc_Setup
 *         Handle the CDC specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */
static uint8_t usbd_cdc_Setup(void *pdev,
                              USB_SETUP_REQ *req)
{
    uint16_t len = USB_CDC_DESC_SIZ;
    uint8_t *pbuf = usbd_cdc_CfgDesc + 9;
    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
        /* CDC Class Requests -------------------------------*/
    case USB_REQ_TYPE_CLASS:
        /* Check if the request is a data setup packet */
        if (req->wLength)
        {
            /* Check if the request is Device-to-Host */
            if (req->bmRequest & 0x80)
            {
                /* Get the data to be sent to Host from interface layer */
                APP_FOPS.pIf_Ctrl(req->bRequest, CmdBuff, req->wLength);
                /* Send the data to the host */
                USBD_CtlSendData(pdev,
                                 CmdBuff,
                                 req->wLength);
            }
            else /* Host-to-Device requeset */
            {
                /* Set the value of the current command to be processed */
                cdcCmd = req->bRequest;
                cdcLen = req->wLength;
                /* Prepare the reception of the buffer over EP0
                Next step: the received data will be managed in usbd_cdc_EP0_TxSent()
                function. */
                /***clear outpacket nak****/
                usb0_clear_out_nak();

                USBD_CtlPrepareRx(pdev, CmdBuff, req->wLength);
            }
        }
        else /* No Data request */
        {
            APP_FOPS.pIf_Ctrl(req->bRequest, NULL, 0);
        }
        return USBD_OK;
    default:
        USBD_CtlError(pdev, req);
        return USBD_FAIL;
    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_DESCRIPTOR:
            USBD_CtlSendData(pdev,
                             pbuf,
                             len);
            break;

        case USB_REQ_GET_INTERFACE:
            USBD_CtlSendData(pdev,
                             (uint8_t *)&usbd_cdc_AltSet,
                             1);
            break;

        case USB_REQ_SET_INTERFACE:
            if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)
            {
                usbd_cdc_AltSet = (uint8_t)(req->wValue);
            }
            else
            {
                /* Call the error management function (command will be nacked */
                USBD_CtlError(pdev, req);
            }
            break;
        }
    }
    return USBD_OK;
}

/**
 * @brief  usbd_cdc_EP0_RxReady
 *         Data received on control endpoint
 * @param  pdev: device device instance
 * @retval status
 */
static uint8_t usbd_cdc_EP0_RxReady(void *pdev)
{
    if (cdcCmd != NO_CMD)
    {
        USB_OTG_ReadPacket(pdev, CmdBuff, 0, cdcLen);

        /* Process the data */
        APP_FOPS.pIf_Ctrl(cdcCmd, CmdBuff, cdcLen);

        /* Reset the command variable to default value */
        cdcCmd = NO_CMD;
    }

    return USBD_OK;
}

static uint8_t CDC_GetEpIndex(uint8_t epNum)
{
    for (uint8_t i = 0; i < TRANS_EP_NUM; i++)
    {
        if (cdc_ep_map[i] == (epNum & 0x7f))
        {
            return i;
        }
    }
    return 0;
}

/**
 * @brief  usbd_cdc_DataIn
 *         handle data IN Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t usbd_cdc_DataIn(void *pdev, uint8_t epnum)
{
    uint8_t cdc_in_buf[CDC_DATA_IN_PACKET_SIZE];
    uint8_t cdc_ep_index = epnum-1;
    uint32_t cdc_pack_size = CDC_GetInDataLen(cdc_ep_map[epnum-1]);
    cdc_pack_size = (cdc_pack_size > cdc_ep_len[cdc_ep_index]) ? cdc_ep_len[cdc_ep_index] : cdc_pack_size;
    if (cdc_pack_size > 0)
    {
        for (uint8_t i = 0; i < cdc_pack_size; i++)
        {
            cdc_in_buf[i] = cdc_app_data.in_struct.app_in_buf[cdc_ep_index][cdc_app_data.in_struct.app_in_buf_rptr[cdc_ep_index]++];
            if (linecoding.datatype == 7)
            {
                cdc_in_buf[i] &= 0x7f;
            }
            if (cdc_app_data.in_struct.app_in_buf_rptr[cdc_ep_index] >= CDC_IN_BUF_SIZE)
            {
                cdc_app_data.in_struct.app_in_buf_rptr[cdc_ep_index] = 0;
            }
        }
        uint32_t sys_irq_status = __disable_irq();
        cdc_app_data.in_struct.app_in_ep_busy[cdc_ep_index] = 1;
        __enable_irq(sys_irq_status);
        DCD_EP_Tx(pdev, epnum, cdc_in_buf, cdc_pack_size);
    }
    else
    {
        uint32_t sys_irq_status = __disable_irq();
        cdc_app_data.in_struct.app_in_ep_busy[cdc_ep_index] = 0;
        __enable_irq(sys_irq_status);
    }
    return USBD_OK;
}

/**
 * @brief  usbd_cdc_DataOut
 *         Data received on non-control Out endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t usbd_cdc_DataOut(void *pdev, uint8_t epnum)
{
    uint8_t cdc_out_buf[CDC_DATA_OUT_PACKET_SIZE];
    uint8_t ep_index = CDC_GetEpIndex(epnum);
    uint8_t ep_packet_len = rx_DataLength;
    DCD_EP_PrepareRx(pdev, epnum, cdc_out_buf, ep_packet_len);
    uint32_t index = cdc_app_data.out_struct.app_out_buf_wptr[ep_index];
    if((ep_packet_len+index)>(CDC_OUT_BUF_SIZE-1))
    {
        memcpy(cdc_app_data.out_struct.app_out_buf[ep_index]+index,cdc_out_buf,CDC_OUT_BUF_SIZE-index);
        memcpy(cdc_app_data.out_struct.app_out_buf[ep_index],cdc_out_buf+CDC_OUT_BUF_SIZE-index,ep_packet_len+index-CDC_OUT_BUF_SIZE);
        index=ep_packet_len+index-CDC_OUT_BUF_SIZE;
    }
    else
    {
        memcpy(cdc_app_data.out_struct.app_out_buf[ep_index]+index,cdc_out_buf,ep_packet_len);
        index=index+ep_packet_len;
    }
    cdc_app_data.out_struct.app_out_buf_wptr[ep_index] = index;
    if (( CDC_OUT_BUF_SIZE- CDC_GetOutDataLen(epnum)  ) <= (CDC_DATA_OUT_PACKET_SIZE*2))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy |= (1<<(ep_index));
    }
    return USBD_OK;
}

/**
 * @brief  send cdc data(put data into send buffer)
 * @param  ep: the endpoint of send data
 * @param  buf: Buffer of data to be send
 * @param  len: Number of data to be send (in bytes)
 * @retval send(put) len
 */
uint32_t CDC_DataIn(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint8_t ep_index = CDC_GetEpIndex(ep);
    if (cdc_app_data.COM_config_cmp != 1 || len == 0)
    {
        return 0;
    }

    while ((CDC_IN_BUF_SIZE - CDC_GetInDataLen(ep)) <= len); // 不能正好放满
    uint32_t index = cdc_app_data.in_struct.app_in_buf_wptr[ep_index];
    if((len+index)>(CDC_IN_BUF_SIZE-1))
    {
        memcpy(cdc_app_data.in_struct.app_in_buf[ep_index]+index,buf,CDC_IN_BUF_SIZE-index);
        memcpy(cdc_app_data.in_struct.app_in_buf[ep_index],buf+CDC_IN_BUF_SIZE-index,len+index-CDC_IN_BUF_SIZE);
        index=len+index-CDC_IN_BUF_SIZE;
    }
    else
    {
        memcpy(cdc_app_data.in_struct.app_in_buf[ep_index]+index,buf,len);
        index=index+len;
    }
    cdc_app_data.in_struct.app_in_buf_wptr[ep_index] = index;
    if (cdc_app_data.in_struct.app_in_ep_busy[ep_index] == 0)
    {
        USBD_DCD_INT_fops->DataInStage(&USB_OTG_dev, ep_index+1);
    }
    return len;
}

/**
 * @brief  read cdc out data
 * @param  ep: the endpoint of read data
 * @param  buf: Buffer addr
 * @param  len: read len
 * @retval readed len
 */
uint32_t CDC_DataOut(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    uint8_t ep_index = CDC_GetEpIndex(ep);
    if((cdc_app_data.out_struct.app_out_buf_rptr[ep_index]+len)<CDC_OUT_BUF_SIZE)
    {
        memcpy(buf,cdc_app_data.out_struct.app_out_buf[ep_index]+cdc_app_data.out_struct.app_out_buf_rptr[ep_index],len);
        cdc_app_data.out_struct.app_out_buf_rptr[ep_index]=len+cdc_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        memcpy(buf,cdc_app_data.out_struct.app_out_buf[ep_index]+cdc_app_data.out_struct.app_out_buf_rptr[ep_index],CDC_OUT_BUF_SIZE-cdc_app_data.out_struct.app_out_buf_rptr[ep_index]);
        memcpy(buf+CDC_OUT_BUF_SIZE-cdc_app_data.out_struct.app_out_buf_rptr[ep_index],cdc_app_data.out_struct.app_out_buf[ep_index],len+cdc_app_data.out_struct.app_out_buf_rptr[ep_index]-CDC_OUT_BUF_SIZE);
        cdc_app_data.out_struct.app_out_buf_rptr[ep_index]=len+cdc_app_data.out_struct.app_out_buf_rptr[ep_index]-CDC_OUT_BUF_SIZE;
    }
    if((CORE_USB->usb_ep_busy.bit.usb_ep_busy&((1<<(ep_index))))!=0&&( CDC_OUT_BUF_SIZE- CDC_GetOutDataLen(ep)  ) > (CDC_DATA_OUT_PACKET_SIZE*2))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy &= (~(1<<(ep_index)));
    }
    return i;
}

/**
 * @brief  Get_TxBuf_length
 *         Get the length of the remaining data to be transmitted
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t CDC_GetInDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = CDC_GetEpIndex(ep_num);
    if (cdc_app_data.in_struct.app_in_buf_rptr[ep_index] <= cdc_app_data.in_struct.app_in_buf_wptr[ep_index])
    {
        ret = cdc_app_data.in_struct.app_in_buf_wptr[ep_index] - cdc_app_data.in_struct.app_in_buf_rptr[ep_index];
    }
    else
    {
        ret = CDC_IN_BUF_SIZE - (cdc_app_data.in_struct.app_in_buf_rptr[ep_index] - cdc_app_data.in_struct.app_in_buf_wptr[ep_index]);
    }
    return ret;
}

/**
 * @brief  Get receive data length
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t CDC_GetOutDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = CDC_GetEpIndex(ep_num);
    if (cdc_app_data.out_struct.app_out_buf_rptr[ep_index] <= cdc_app_data.out_struct.app_out_buf_wptr[ep_index])
    {
        ret = cdc_app_data.out_struct.app_out_buf_wptr[ep_index] - cdc_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        ret = CDC_OUT_BUF_SIZE + cdc_app_data.out_struct.app_out_buf_wptr[ep_index] - cdc_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    return ret;
}

/**
 * @brief  usbd_cdc_SOF
 *         Start Of Frame event management
 * @param  pdev: instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t usbd_cdc_SOF(void *pdev)
{
    return USBD_OK;
}
