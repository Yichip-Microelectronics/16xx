#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
#include "usbd_cdc_vcp.h"
#include "usb_conf.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_dcd_init.h"
LINE_CODING linecoding =
  {
    921600, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };


//UART_InitTypeDef USART_InitStructure;

extern struct APP_DATA_STRUCT_DEF cdc_app_data;
                            /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops =
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  NULL,
  NULL
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
    cdc_app_data.COM_config_cmp = 0;
    memset(&cdc_app_data, 0, sizeof(cdc_app_data));
    return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media 
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{

  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl(uint32_t Cmd, uint8_t *Buf, uint32_t Len)
{
    switch (Cmd)
    {

    case SEND_ENCAPSULATED_COMMAND:
        /* Not  needed for this driver */
        break;

    case GET_ENCAPSULATED_RESPONSE:
        /* Not  needed for this driver */
        break;

    case SET_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case GET_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case CLEAR_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case SET_LINE_CODING:

        linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
        linecoding.format = Buf[4];
        linecoding.paritytype = Buf[5];
        linecoding.datatype = Buf[6];
        cdc_app_data.COM_config_cmp = 1;
        /* Set the new configuration */
        VCP_COMConfig(OTHER_CONFIG);
        break;

    case GET_LINE_CODING:
        Buf[0] = (uint8_t)(linecoding.bitrate);
        Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
        Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
        Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
        Buf[4] = linecoding.format;
        Buf[5] = linecoding.paritytype;
        Buf[6] = linecoding.datatype;
        break;

    case SET_CONTROL_LINE_STATE:
        break;

    case SEND_BREAK:
        /* Not  needed for this driver */
        break;

    default:
        break;
    }

    return USBD_OK;
}


/**
  * @brief  VCP_COMConfig
  *         Configure the COM Port with default values or values received from host.
  * @param  Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG
  *         to set a configuration received from the host.
  * @retval None.
  */
static uint16_t VCP_COMConfig(uint8_t Conf)
{
  return USBD_OK;
}
