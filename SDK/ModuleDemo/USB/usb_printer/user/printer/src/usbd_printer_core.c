/* Includes ------------------------------------------------------------------*/
#include "usbd_printer_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_usr.h"
#include "usb_dcd_init.h"
/** @addtogroup YC-1606_USB_OTG_DEVICE_LIBRARY
 * @{
 */

/** @defgroup usbd_printer
 * @brief usbd core module
 * @{
 */

/** @defgroup usbd_printer_Private_TypesDefinitions
 * @{
 */
/**
 * @}
 */

/** @defgroup usbd_printer_Private_Defines
 * @{
 */
/**
 * @}
 */

/** @defgroup usbd_printer_Private_Macros
 * @{
 */
/**
 * @}
 */

/** @defgroup usbd_printer_Private_FunctionPrototypes
 * @{
 */

/*********************************************
   PRINTER Device library callbacks
 *********************************************/
static uint8_t usbd_printer_Init(void *pdev, uint8_t cfgidx);
static uint8_t usbd_printer_DeInit(void *pdev, uint8_t cfgidx);
static uint8_t usbd_printer_Setup(void *pdev, USB_SETUP_REQ *req);
static uint8_t usbd_printer_EP0_RxReady(void *pdev);
static uint8_t usbd_printer_DataIn(void *pdev, uint8_t epnum);
static uint8_t usbd_printer_DataOut(void *pdev, uint8_t epnum);
static uint8_t usbd_printer_SOF(void *pdev);

/*********************************************
   PRINTER specific management functions
 *********************************************/
static uint8_t *USBD_printer_GetCfgDesc(uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS
static uint8_t *USBD_printer_GetOtherCfgDesc(uint8_t speed, uint16_t *length);
#endif
/**
 * @}
 */

/** @defgroup usbd_printer_Private_Variables
 * @{
 */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_printer_CfgDesc[USB_PRINTER_CONFIG_DESC_SIZ] __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_printer_OtherCfgDesc[USB_PRINTER_CONFIG_DESC_SIZ] __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t usbd_printer_AltSet __ALIGN_END = 0;

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

struct APP_DATA_STRUCT_DEF printer_app_data;
const uint8_t printer_ep_len[TRANS_EP_NUM] = {PRINTER_DATA_MAX_PACKET_SIZE};
const uint8_t printer_ep_map[TRANS_EP_NUM] = {PRINTER_OUT_EP};
extern USB_OTG_CORE_HANDLE USB_OTG_dev;

/* PRINTER interface class callbacks structure */
USBD_Class_cb_TypeDef USBD_PRINTER_cb =
    {
        usbd_printer_Init,
        usbd_printer_DeInit,
        usbd_printer_Setup,
        NULL, /* EP0_TxSent, */
        usbd_printer_EP0_RxReady,
        usbd_printer_DataIn,
        usbd_printer_DataOut,
        usbd_printer_SOF,
        NULL,
        NULL,
        USBD_printer_GetCfgDesc,
#ifdef USE_USB_OTG_HS
        USBD_printer_GetOtherCfgDesc, /* use same cobfig as per FS */
#endif                                /* USE_USB_OTG_HS  */
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB PRINTER device Configuration Descriptor */
__ALIGN_BEGIN uint8_t usbd_printer_CfgDesc[USB_PRINTER_CONFIG_DESC_SIZ] __ALIGN_END =
    {
        /*Configuation Descriptor*/
        0x09,                              /* bLength: Configuation Descriptor size */
        USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
        USB_PRINTER_CONFIG_DESC_SIZ,       /* wTotalLength:no of returned bytes */
        0x00,

        0x01, /* bNumInterfaces: 1 interface */

        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor describing the configuration */
        0xc0, /* bmAttributes: self powered */
        0x32, /* MaxPower 0 mA */
        /*Interface Descriptor*/
        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x02, /* bNumEndpoints: One endpoints used */

        0x07, // basic class for printer
        0x01, // printer calll device

        0x02, // bi-directional interface.
        0x04, /* iInterface: */

        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        PRINTER_OUT_EP,               /* bEndpointAddress: (OUT3) */
        0x02,                         /* bmAttributes: Bulk */
        0x40,                         /* wMaxPacketSize: */
        0x00,
        0x00, /* bInterval: ignore for Bulk transfer */
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        PRINTER_IN_EP,                /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x40,                         /* wMaxPacketSize: */
        0x00,
        0x00 /* bInterval */
};

/* USB PRINTER device id Descriptor */
__ALIGN_BEGIN uint8_t print_id_descriptor[] __ALIGN_END =
    {
        0x55,
        0xAA,
        'M', 'A', 'N', 'U', 'F', 'A', 'C', 'T', 'U', 'R', 'E', 'R', ':', 'Y', 'I', 'C', 'H', 'I', 'P', ';',
        'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', 'S', 'E', 'T', ':', 'E', 'S', 'C', '/', 'P', 'O', 'S', ';',
        'M', 'O', 'D', 'E', 'L', ':', '-', 'P', 'r', 'i', 'n', 't', 'e', 'r', ' ', 'd', 'e', 'm', 'o', ';',
        'C', 'O', 'M', 'M', 'E', 'N', 'T', ':', 't', 'e', 's', 't', ' ', 'P', 'r', 'i', 'n', 't', 'e', 'r', ';',
        'A', 'C', 'T', 'I', 'V', 'E', ' ', 'C', 'O', 'M', 'M', 'A', 'N', 'D', ':', 'E', 'S', 'C', '/', 'P', 'O', 'S', ';'};

#ifdef USE_USB_OTG_HS
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t usbd_printer_OtherCfgDesc[USB_PRINTER_CONFIG_DESC_SIZ] __ALIGN_END =
    {};
#endif /* USE_USB_OTG_HS  */

/**
 * @}
 */

/** @defgroup usbd_printer_Private_Functions
 * @{
 */

/**
 * @brief  USBD_printer_GetCfgDesc
 *         Return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t *USBD_printer_GetCfgDesc(uint8_t speed, uint16_t *length)
{
    *length = sizeof(usbd_printer_CfgDesc);
    return usbd_printer_CfgDesc;
}

/**
 * @brief  USBD_printer_GetCfgDesc
 *         Return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
#ifdef USE_USB_OTG_HS
static uint8_t *USBD_printer_GetOtherCfgDesc(uint8_t speed, uint16_t *length)
{
    *length = sizeof(usbd_printer_OtherCfgDesc);
    return usbd_printer_OtherCfgDesc;
}
#endif

/**
 * @brief  usbd_printer_Init
 *         Initilaize the PRINTER interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t usbd_printer_Init(void *pdev,
                                 uint8_t cfgidx)
{
    uint8_t printer_packet_buf[PRINTER_DATA_MAX_PACKET_SIZE];

    memset((uint8_t *)&printer_app_data, 0, sizeof(printer_app_data));

    /* Open EP IN */
    DCD_EP_Open(pdev,
                PRINTER_IN_EP,
                PRINTER_DATA_IN_PACKET_SIZE,
                USB_OTG_EP_BULK);

    /* Open EP OUT */
    DCD_EP_Open(pdev,
                PRINTER_OUT_EP,
                PRINTER_DATA_OUT_PACKET_SIZE,
                USB_OTG_EP_BULK);

    /* Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev,
                     PRINTER_OUT_EP,
                     (uint8_t *)(printer_packet_buf),
                     PRINTER_DATA_OUT_PACKET_SIZE);

    return USBD_OK;
}

/**
 * @brief  usbd_printer_Init
 *         DeInitialize the PRINTER layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t usbd_printer_DeInit(void *pdev,
                                   uint8_t cfgidx)
{
    /* Open EP IN */
    DCD_EP_Close(pdev,
                 PRINTER_IN_EP);

    /* Open EP OUT */
    DCD_EP_Close(pdev,
                 PRINTER_OUT_EP);

    return USBD_OK;
}

/**
 * @brief  usbd_printer_Setup
 *         Handle the PRINTER specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */
static uint8_t usbd_printer_Setup(void *pdev,
                                  USB_SETUP_REQ *req)
{
    uint16_t len = USB_PRINTER_DESC_SIZ;
    uint8_t *pbuf = usbd_printer_CfgDesc + 9;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
    /* PRINTER Class Requests -------------------------------*/
    case USB_REQ_TYPE_CLASS:
        /* Check if the request is a data setup packet */
        if (req->wLength)
        {
            /* Check if the request is Device-to-Host */
            if (req->bmRequest & 0x80)
            {
                /* Get the data to be sent to Host from interface layer */

                /* Send the data to the host */
                print_id_descriptor[0] = sizeof(print_id_descriptor) >> 8;
                print_id_descriptor[1] = sizeof(print_id_descriptor);
                USBD_CtlSendData(pdev,
                                 print_id_descriptor,
                                 sizeof(print_id_descriptor));
                USB_OTG_dev.dev.device_status_new = USB_OTG_END;
            }
            else /* Host-to-Device requeset */
            {

                /* Prepare the reception of the buffer over EP0
                Next step: the received data will be managed in usbd_printer_EP0_TxSent()
                function. */
                USBD_CtlError(pdev, req);
            }
        }
        else /* No Data request */
        {
            /* Transfer the command to the interface layer */
            USBD_CtlError(pdev, req);
        }

        return USBD_OK;

    default:
        USBD_CtlError(pdev, req);
        return USBD_FAIL;

    /* Standard Requests -------------------------------*/
    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_DESCRIPTOR:
            if ((req->wValue >> 8) == PRINTER_DESCRIPTOR_TYPE)
            {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
                pbuf = usbd_printer_Desc;
#else
                pbuf = usbd_printer_CfgDesc + 9 + (9 * USBD_ITF_MAX_NUM);
#endif
                len = MIN(USB_PRINTER_DESC_SIZ, req->wLength);
            }

            USBD_CtlSendData(pdev,
                             pbuf,
                             len);
            break;

        case USB_REQ_GET_INTERFACE:
            USBD_CtlSendData(pdev,
                             (uint8_t *)&usbd_printer_AltSet,
                             1);
            break;

        case USB_REQ_SET_INTERFACE:
            if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)
            {
                usbd_printer_AltSet = (uint8_t)(req->wValue);
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
 * @brief  usbd_printer_EP0_RxReady
 *         Data received on control endpoint
 * @param  pdev: device device instance
 * @retval status
 */
static uint8_t usbd_printer_EP0_RxReady(void *pdev)
{
    return USBD_OK;
}

static uint8_t PRINTER_GetEpIndex(uint8_t epNum)
{
    for (uint8_t i = 0; i < TRANS_EP_NUM; i++)
    {
        if (printer_ep_map[i] == (epNum & 0x7f))
        {
            return i;
        }
    }
    return 0;
}

/**
 * @brief  usbd_printer_DataIn
 *         Data sent on non-control IN endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t usbd_printer_DataIn(void *pdev, uint8_t epnum)
{
    uint8_t printer_in_buf[PRINTER_DATA_IN_PACKET_SIZE];
    uint8_t printer_ep_index = epnum-1;
    uint32_t printer_pack_size = printer_ep_map[epnum-1];

    printer_pack_size = (printer_pack_size > printer_ep_len[printer_ep_index]) ? printer_ep_len[printer_ep_index] : printer_pack_size;
    if (printer_pack_size > 0)
    {
        for (uint8_t i = 0; i < printer_pack_size; i++)
        {
            printer_in_buf[i] = printer_app_data.in_struct.app_in_buf[printer_ep_index][printer_app_data.in_struct.app_in_buf_rptr[printer_ep_index]++];
            if (printer_app_data.in_struct.app_in_buf_rptr[printer_ep_index] >= PRINTER_APP_IN_BUF_SIZE)
            {
                printer_app_data.in_struct.app_in_buf_rptr[printer_ep_index] = 0;
            }
        }
        uint32_t sys_irq_status = __disable_irq();
        printer_app_data.in_struct.app_in_ep_busy[printer_ep_index] = 1;
        __enable_irq(sys_irq_status);
        DCD_EP_Tx(pdev, epnum, printer_in_buf, printer_pack_size);
    }
    else
    {
        uint32_t sys_irq_status = __disable_irq();
        printer_app_data.in_struct.app_in_ep_busy[printer_ep_index] = 0;
        __enable_irq(sys_irq_status);
    }
    return USBD_OK;
}

/**
 * @brief  usbd_printer_DataOut
 *         Data received on non-control Out endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t usbd_printer_DataOut(void *pdev, uint8_t epnum)
{
    uint8_t printer_out_buf[PRINTER_DATA_OUT_PACKET_SIZE];
    uint8_t ep_index = PRINTER_GetEpIndex(epnum);
    uint8_t ep_packet_len = rx_DataLength;

    DCD_EP_PrepareRx(pdev, epnum, printer_out_buf, ep_packet_len);
    uint32_t index = printer_app_data.out_struct.app_out_buf_wptr[ep_index];
    if((ep_packet_len+index)>(PRINTER_APP_OUT_BUF_SIZE-1))
    {
        memcpy(printer_app_data.out_struct.app_out_buf[ep_index]+index,printer_out_buf,PRINTER_APP_OUT_BUF_SIZE-index);
        memcpy(printer_app_data.out_struct.app_out_buf[ep_index],printer_out_buf+PRINTER_APP_OUT_BUF_SIZE-index,ep_packet_len+index-PRINTER_APP_OUT_BUF_SIZE);
        index=ep_packet_len+index-PRINTER_APP_OUT_BUF_SIZE;
    }
    else
    {
        memcpy(printer_app_data.out_struct.app_out_buf[ep_index]+index,printer_out_buf,ep_packet_len);
        index=index+ep_packet_len;
    }
    printer_app_data.out_struct.app_out_buf_wptr[ep_index] = index;
    if (( PRINTER_APP_OUT_BUF_SIZE- PRINTER_GetOutDataLen(epnum)  ) <= (PRINTER_DATA_OUT_PACKET_SIZE*2))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy =1<<(ep_index);
    }

    return USBD_OK;
}

/**
 * @brief  send printer data(put data into send buffer)
 * @param  ep: the endpoint of send data
 * @param  buf: Buffer of data to be send
 * @param  len: Number of data to be send (in bytes)
 * @retval send(put) len
 */
uint32_t PRINTER_DataIn(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint8_t ep_index = PRINTER_GetEpIndex(ep);

    if (USB_OTG_dev.dev.device_status_new != USB_OTG_END || len == 0)
    {
        return 0;
    }

    while ((PRINTER_APP_IN_BUF_SIZE - PRINTER_GetInDataLen(ep)) <= len)
        ; // 不能正好放满

    uint32_t index = printer_app_data.in_struct.app_in_buf_wptr[ep_index];

    for (int i = 0; i < len; i++)
    {
        /* Detect whether the buffer overflow */
        printer_app_data.in_struct.app_in_buf[ep_index][index++] = buf[i];
        /* To avoid buffer overflow */
        if (index >= PRINTER_APP_IN_BUF_SIZE)
        {
            index = 0;
        }
    }
    printer_app_data.in_struct.app_in_buf_wptr[ep_index] = index;
    if (printer_app_data.in_struct.app_in_ep_busy[ep_index] == 0)
    {
        USBD_DCD_INT_fops->DataInStage(&USB_OTG_dev, ep_index+1);
    }

    // while (HREAD(CORE_USB_TX_BUSY) & (1 << (ep_num & 0x7f)));
    return len;
}

/**
 * @brief  read printer out data
 * @param  ep: the endpoint of read data
 * @param  buf: Buffer addr
 * @param  len: read len
 * @retval readed len
 */
uint32_t PRINTER_DataOut(uint8_t ep, uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    uint8_t ep_index = PRINTER_GetEpIndex(ep);
    if((printer_app_data.out_struct.app_out_buf_rptr[ep_index]+len)<PRINTER_APP_OUT_BUF_SIZE)
    {
        memcpy(buf,printer_app_data.out_struct.app_out_buf[ep_index]+printer_app_data.out_struct.app_out_buf_rptr[ep_index],len);
        printer_app_data.out_struct.app_out_buf_rptr[ep_index]=len+printer_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        memcpy(buf,printer_app_data.out_struct.app_out_buf[ep_index]+printer_app_data.out_struct.app_out_buf_rptr[ep_index],PRINTER_APP_OUT_BUF_SIZE-printer_app_data.out_struct.app_out_buf_rptr[ep_index]);
        memcpy(buf+PRINTER_APP_OUT_BUF_SIZE-printer_app_data.out_struct.app_out_buf_rptr[ep_index],printer_app_data.out_struct.app_out_buf[ep_index],len+printer_app_data.out_struct.app_out_buf_rptr[ep_index]-PRINTER_APP_OUT_BUF_SIZE);
        printer_app_data.out_struct.app_out_buf_rptr[ep_index]=len+printer_app_data.out_struct.app_out_buf_rptr[ep_index]-PRINTER_APP_OUT_BUF_SIZE;
    }
    if(((CORE_USB->usb_ep_busy.bit.usb_ep_busy&(1<<(ep_index)))!=0)&&(PRINTER_APP_OUT_BUF_SIZE- PRINTER_GetOutDataLen(ep)) > (PRINTER_DATA_OUT_PACKET_SIZE*2))
    {
        CORE_USB->usb_ep_busy.bit.usb_ep_busy &= ~(1<<(ep_index));
    }
    return i;
}

/**
 * @brief  Get_TxBuf_length
 *         Get the length of the remaining data to be transmitted
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t PRINTER_GetInDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = PRINTER_GetEpIndex(ep_num);
    if (printer_app_data.in_struct.app_in_buf_rptr[ep_index] <= printer_app_data.in_struct.app_in_buf_wptr[ep_index])
    {
        ret = printer_app_data.in_struct.app_in_buf_wptr[ep_index] - printer_app_data.in_struct.app_in_buf_rptr[ep_index];
    }
    else
    {
        ret = PRINTER_APP_IN_BUF_SIZE - (printer_app_data.in_struct.app_in_buf_rptr[ep_index] - printer_app_data.in_struct.app_in_buf_wptr[ep_index]);
    }
    return ret;
}

/**
 * @brief  Get receive data length
 * @param  NONE
 * @retval Result receive data length
 */
uint32_t PRINTER_GetOutDataLen(uint8_t ep_num)
{
    uint32_t ret = 0x00;
    uint8_t ep_index = PRINTER_GetEpIndex(ep_num);
    if (printer_app_data.out_struct.app_out_buf_rptr[ep_index] <= printer_app_data.out_struct.app_out_buf_wptr[ep_index])
    {
        ret = printer_app_data.out_struct.app_out_buf_wptr[ep_index] - printer_app_data.out_struct.app_out_buf_rptr[ep_index];
    }
    else
    {
        ret = PRINTER_APP_OUT_BUF_SIZE + printer_app_data.out_struct.app_out_buf_wptr[ep_index] - printer_app_data.out_struct.app_out_buf_rptr[ep_index];
    }

    return ret;
}

/**
 * @brief  usbd_printer_SOF
 *         Start Of Frame event management
 * @param  pdev: instance
 * @param  epnum: endpoint number
 * @retval status
 */
static uint8_t usbd_printer_SOF(void *pdev)
{
    return USBD_OK;
}
