/**
  ******************************************************************************
  * @file    usb_core.c
  * @author  WJJ
  * @version V1.1.0
  * @date    2021-9-8
  * @brief   This file provides the USBD descriptors and string formating method.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_dcd_init.h"
#include "usbd_usr.h"
#include "register.h"


extern uint8_t   usb_rxbuf [Usb_Rx_Bufsize] ;
extern uint8_t   usb_txbuf0[Usb_Endpoint0tx_Bufsize] ;
extern uint8_t   usb_txbuf1[Usb_Endpoint1tx_Bufsize] ;
extern uint8_t   usb_txbuf2[Usb_Endpoint2tx_Bufsize] ;
extern uint8_t   usb_txbuf3[Usb_Endpoint3tx_Bufsize] ;


volatile uint8_t  *usb_rxptr;
volatile uint16_t rx_DataLength;

void USB_INtransmission_Choose(USB_OTG_CORE_HANDLE *pdev, uint8_t channel_set)
{
    for (uint8_t i = 0 ; i < NUM_EP_FIFO; i++)
    {
        if (channel_set & (1 << i))
            pdev->dev.device_outchannel[i] = USB_INTRANS_AUTO;
    }

}

/**
 * @method usb_getbyte
 * @brief  None
 * @param  None
 * @retval None
 */
uint8_t usb_getbyte()
{
    uint8_t t;

    if (usb_rxptr == usb_rxbuf + sizeof(usb_rxbuf))
    {
        usb_rxptr = usb_rxbuf;
    }
    t = *usb_rxptr++;

    return t;
}



/**
 * @method usb_getword
 * @brief  None
 * @param  None
 * @retval None
 */
uint16_t usb_getword()
{
    return usb_getbyte() | (int) usb_getbyte() << 8;
}


void putbuf(uint8_t *buf, uint8_t *src, int len)
{
    int len0 = len - 1;
    buf[0] = len0, buf[1] = len0 >> 8;
    memcpy(buf + 2, src, len);
}

USB_OTG_STS USB_OTG_WritePacket(USB_OTG_CORE_HANDLE *pdev,
                                uint8_t *src,
                                uint8_t ch_ep_num, uint16_t len)
{

    USB_OTG_STS status = USB_OTG_OK;

    switch (ch_ep_num | 0x80)
    {
        case 0x80:
            while(CORE_USB->ep0_txbusy.bit.ep0_txbusy);
            putbuf(usb_txbuf0, src, len);
            CORE_USB->usb_tx_saddr0.reg, (uint32_t)(usb_txbuf0);
            CORE_USB->usb_trig.bit.ep_tx = 1;
            break;

        case 0x81:
            while(CORE_USB->ep0_txbusy.bit.ep1_txbusy);
            putbuf(usb_txbuf1, src, len);
            CORE_USB->usb_tx_saddr1.reg, (uint32_t)(usb_txbuf1);
            CORE_USB->usb_trig.bit.ep_tx = 2;
            break;

        case 0x82:
            while(CORE_USB->ep0_txbusy.bit.ep2_txbusy);
            putbuf(usb_txbuf2, src, len);
            CORE_USB->usb_tx_saddr2.reg, (uint32_t)(usb_txbuf2);
            CORE_USB->usb_trig.bit.ep_tx = 4;
            break;

        case 0x83:
            while(CORE_USB->ep0_txbusy.bit.ep3_txbusy);
            putbuf(usb_txbuf3, src, len);
            CORE_USB->usb_tx_saddr3.reg, (uint32_t)(usb_txbuf3);
            CORE_USB->usb_trig.bit.ep_tx = 8;
            break;
    }

    return status;
}


/**
* @brief  USB_OTG_ReadPacket : Reads a packet from the Rx FIFO
* @param  pdev : Selected device
* @param  dest : Destination Pointer
* @param  bytes : No. of bytes
* @retval None
*/
void USB_OTG_ReadPacket(USB_OTG_CORE_HANDLE *pdev,
                        uint8_t *dest, uint8_t ch_ep_num, uint16_t len)
{
    uint16_t i = 0;
    uint16_t count8b = len;
    uint8_t *data_buff = (uint8_t *) dest;
    for (i = 0; i < count8b; i++, data_buff++)
    {
        *data_buff = usb_getbyte();

    }
    rx_DataLength = 0;
    CORE_USB->usb_rxptr.reg = (uint32_t)(usb_rxptr);
}

extern uint32_t DCD_HandleOutEP_ISR(USB_OTG_CORE_HANDLE *pdev);


/**
* @brief  USB_OTG_SelectCore
*         Initialize core registers address.
* @param  pdev : Selected device
* @param  coreID : USB OTG Core ID
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_SelectCore(USB_OTG_CORE_HANDLE *pdev,
                               USB_OTG_CORE_ID_TypeDef coreID)
{

    USB_OTG_STS status = USB_OTG_OK;

#ifdef  USB_OTG_DMA_ENABLE
    pdev->cfg.dma_enable = 1;
#else
    pdev->cfg.dma_enable = 0;
#endif

    /* at startup the core is in FS mode */
    pdev->cfg.speed = USB_OTG_SPEED_FULL;
    pdev->cfg.mps = USB_OTG_FS_MAX_PACKET_SIZE;


    /* initialize device cfg following its address */
    if (coreID == USB_OTG_FS_CORE_ID)
    {
        pdev->cfg.coreID = USB_OTG_FS_CORE_ID;
        pdev->cfg.dev_endpoints = 4;
        pdev->cfg.TotalFifoSize = 64;   /* in 8-bits */
        pdev->cfg.phy_itface = USB_OTG_EMBEDDED_PHY;

#ifdef USB_OTG_FS_SOF_OUTPUT_ENABLED
        pdev->cfg.Sof_output = 1;
#endif

#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
        pdev->cfg.low_power = 1;
#endif
    }
    else if (coreID == USB_OTG_HS_CORE_ID)
    {
        pdev->cfg.coreID = USB_OTG_HS_CORE_ID;
        pdev->cfg.host_channels = 8;
        pdev->cfg.dev_endpoints = 4;
        pdev->cfg.TotalFifoSize = 512;  /* in 8-bits */

#ifdef USB_OTG_ULPI_PHY_ENABLED
        pdev->cfg.phy_itface = USB_OTG_ULPI_PHY;
#else
# ifdef USB_OTG_EMBEDDED_PHY_ENABLED
        pdev->cfg.phy_itface = USB_OTG_EMBEDDED_PHY;
# endif
#endif

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pdev->cfg.dma_enable = 1;
#endif

#ifdef USB_OTG_HS_SOF_OUTPUT_ENABLED
        pdev->cfg.Sof_output = 1;
#endif

#ifdef USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
        pdev->cfg.low_power = 1;
#endif
    }

    return status;
}


/**
* @brief  USB_OTG_CoreInit
*         Initializes the USB_OTG controller registers and prepares the core
*         device mode or host mode operation.
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_CoreInit(USB_OTG_CORE_HANDLE *pdev)
{
    USB_OTG_STS status = USB_OTG_OK;
    return status;
}



/**
* @brief  USB_OTG_SetCurrentMode : Set ID line
* @param  pdev : Selected device
* @param  mode :  (Host/device)only device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_SetCurrentMode(USB_OTG_CORE_HANDLE *pdev, uint8_t mode)
{
    USB_OTG_STS status = USB_OTG_OK;
    if (mode == HOST_MODE)
    {
    }
    else if (mode == DEVICE_MODE)
    {
#ifdef USE_DEVICE_MODE
        pdev->dev.out_ep[0].xfer_buff = pdev->dev.setup_packet;
        pdev->dev.out_ep[0].xfer_len = 8;
#endif
    }
    return status;
}

/**
* @brief  USB_OTG_EPActivate : Activates an EP
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPActivate(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    // USB_OTG_IRQ_MASK1_TypeDef intr_rxtxe;

    /* Read DEPCTLn register */
    if (ep->is_in == 1)
    {
        // intr_rxtxe.d8 = 1 << ep->num;
    }
    else
    {
        // intr_rxtxe.d8 = 1 << ep->num;
    }
    return status;
}


/**
* @brief  USB_OTG_EPDeactivate : Deactivates an EP
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPDeactivate(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    // USB_OTG_IRQ_MASK1_TypeDef intr_rxtxe;

    /* Read DEPCTLn register */
    if (ep->is_in == 1)
    {
        // intr_rxtxe.d8 = 1 << ep->num;
    }
    else
    {
        // intr_rxtxe.d8 = 1 << ep->num;
    }
    return status;
}



/**
* @brief  USB_OTG_GetMode : Get current mode
* @param  pdev : Selected device
* @retval current mode
*/
uint8_t USB_OTG_GetMode(USB_OTG_CORE_HANDLE *pdev)
{
    return DEVICE_MODE;
}


/**
* @brief  USB_OTG_IsDeviceMode : Check if it is device mode
* @param  pdev : Selected device
* @retval num_in_ep
*/
uint8_t USB_OTG_IsDeviceMode(USB_OTG_CORE_HANDLE *pdev)
{
    return (USB_OTG_GetMode(pdev) != HOST_MODE);
}

/**
* @brief  USB_OTG_EPStartXfer : Handle the setup for data xfer for an EP and
*         starts the xfer
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
uint8_t data_len;
USB_OTG_STS USB_OTG_EPStartXfer(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    uint16_t rx_count;

    /* IN endpoint */
    if (ep->is_in == 1)
    {
        if ((pdev->cfg.dma_enable == 0)
                || ((USB_OTG_DEV_DMA_EP_NUM & 0x07) != ep->num))
        {
            ep->rem_data_len = ep->xfer_len - ep->xfer_count;

            if (ep->rem_data_len == 0)
            {
                USB_OTG_WritePacket(pdev,
                                    ep->xfer_buff + ep->xfer_count, ep->num, 0);
                ep->xfer_count = ep->xfer_len;
                ep->rem_data_len = 0;
            }
            else if (ep->rem_data_len == ep->maxpacket)
            {
                USB_OTG_WritePacket(pdev,
                                    ep->xfer_buff + ep->xfer_count,
                                    ep->num, ep->maxpacket);
                ep->xfer_count += ep->maxpacket;
                ep->xfer_count = ep->xfer_len;
                ep->rem_data_len = 0;
            }
            else
            {

                if (ep->rem_data_len > ep->maxpacket)
                {
                    USB_OTG_WritePacket(pdev,
                                        ep->xfer_buff + ep->xfer_count,
                                        ep->num, ep->maxpacket);
                    ep->xfer_count += ep->maxpacket;
                    if (ep->xfer_len >= ep->xfer_count)
                    {
                        ep->rem_data_len = ep->xfer_len - ep->xfer_count;
                    }
                    else
                    {
                        ep->rem_data_len = 0;
                        ep->xfer_count = ep->xfer_len;
                    }
                }
                else
                {
                    USB_OTG_WritePacket(pdev,
                                        ep->xfer_buff + ep->xfer_count,
                                        ep->num, ep->rem_data_len);
                    ep->xfer_count = ep->xfer_len;
                    ep->rem_data_len = 0;
                }
            }
        }
    }
    else
    {
        rx_count = rx_DataLength;
        data_len = rx_count;
        USB_OTG_ReadPacket(pdev, ep->xfer_buff + ep->xfer_count, ep->num, rx_count);
        ep->xfer_count += rx_count;

        if (ep->xfer_len <= ep->xfer_count)
        {
            ep->rem_data_len = ep->xfer_count - ep->xfer_len;
        }
        else
        {
            ep->rem_data_len = 0;
            ep->xfer_len = ep->xfer_count;
        }
    }
    return status;
}


/**
* @brief  USB_OTG_EP0StartXfer : Handle the setup for a data xfer for EP0 and
*         starts the xfer
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EP0StartXfer(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    /* IN endpoint */
    if (ep->is_in == 1)
    {
        ep->rem_data_len = ep->xfer_len - ep->xfer_count;
        /* Zero Length Packet? */
        if (ep->rem_data_len == 0)
        {
            //  send zero packet
            USB_OTG_EPReply_Zerolen(pdev, ep);
            ep->xfer_count = ep->xfer_len;
            ep->rem_data_len = 0;
        }
        else
        {
            if (ep->rem_data_len > ep->maxpacket)
            {
                USB_OTG_WritePacket(pdev,
                                    ep->xfer_buff + ep->xfer_count,
                                    0, ep->maxpacket);

                ep->xfer_count += ep->maxpacket;
                ep->rem_data_len = ep->xfer_len - ep->xfer_count;
            }
            else if (ep->rem_data_len == ep->maxpacket)
            {
                USB_OTG_WritePacket(pdev,
                                    ep->xfer_buff + ep->xfer_count,
                                    0, ep->maxpacket);
                ep->xfer_count = ep->xfer_len;
                ep->rem_data_len = 0;
                USB_OTG_EPReply_Zerolen(pdev, ep);
                /****prepare receive outpacket *****/
            }
            else
            {
                USB_OTG_WritePacket(pdev,
                                    ep->xfer_buff + ep->xfer_count,
                                    0, ep->rem_data_len);
                ep->xfer_count = ep->xfer_len;
                ep->rem_data_len = 0;
                /****prepare receive outpacket *****/
                usb0_clear_out_nak();

            }
        }
        if (pdev->cfg.dma_enable == 0)
        {
            /* Enable the Tx FIFO Empty Interrupt for this EP */
            if (ep->xfer_len > 0)
            {
            }
        }
    }
    else
    {
        /* OUT endpoint */
    }
    return status;
}

/**
* @brief  USB_OTG_TRIG : Handle start xfer and set tx trig
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/


void usb_clear_nak(uint8_t ep, uint8_t out)
{
    uint8_t value = 0;
    if(out)
    {
        value |= (1 << 1);
    }
    else
    {
        value |= (1 << 0);
    }
    CORE_USB->reply0.reg = 1 << (ep*2 + out);
}


void usb0_clear_out_nak()
{
    usb_clear_nak(0, 1);
}

void usb0_clear_in_nak()
{
    usb_clear_nak(0, 0);
}

USB_OTG_STS USB_OTG_TRIG(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    return status;
}

/**
* @brief  USB_OTG_EPSetStall : Set the EP STALL
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPSetStall(USB_OTG_CORE_HANDLE *pdev, USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;

    if (ep->num == 0)
    {
        CORE_USB->set_stall_ep0.bit.set_stall_ep0 = 1;
        //USB_OTG_EPReply_Zerolen(pdev, ep);
    }

    return status;
}

/**
* @brief  USB_OTG_EPSetStall : ack zero  length packet
* @param  pdev : Selected device
* @retval USB_OTG_STS : status
*/

USB_OTG_STS USB_OTG_EPReply_Zerolen(USB_OTG_CORE_HANDLE *pdev,
                                    USB_OTG_EP *ep)
{
    USB_OTG_STS status = USB_OTG_OK;
    CORE_USB->reply0.reg = 1 << (ep->num * 2);
    return status;
}

/**
* @brief  USB_OTG_RemoteWakeup : active remote wakeup signalling
* @param  None
* @retval : None
*/
void USB_OTG_ActiveRemoteWakeup(USB_OTG_CORE_HANDLE *pdev)
{
    // USB_OTG_CTL_TypeDef power;

    /* Note: If CLK has been stopped,it will need be restarted before
     * this write can occur.
     */
    // power.d8 = HREAD(core_usb_config);
    // power.b.resume = 1;
    // power.b.wakeup_enable = 1;
    // HWRITE(core_usb_config, power.d8);
    //CORE_USB->usb_ctrl0.bit.usb_resume = 1;
    //CORE_USB->usb_ctrl0.bit.ep_wkup_en = 1;
    //  USB_CONFIG = power.d8;
    /* The software should leave then this bit set for approximately 10ms
     * (minimum of 2ms, a maximum of 15ms) before resetting it to 0.
     */
    // power.b.resume = 0;
    // power.b.wakeup_enable = 0;
    // USB_CONFIG = power.d8;
    // HWRITE(core_usb_config, power.d8);
    //CORE_USB->usb_ctrl0.bit.usb_resume = 0;
    //CORE_USB->usb_ctrl0.bit.ep_wkup_en = 0;
}

/**
  * @brief  USB_StopRcv
  * @param  None.
  * @retval None.
  */
void USB_StopRcv(void)
{
    //CORE_USB->mask.bit.mask = 0xfff;
    CORE_USB->usb_ep_busy.bit.usb_ep_busy=0X7;
}

/**
  * @brief  USB_RestartRcv
  * @param  None.
  * @retval None.
  */
void USB_RestartRcv(void)
{
    //CORE_USB->mask.bit.mask = 0xd7;
    CORE_USB->usb_ep_busy.bit.usb_ep_busy=0X0;
}
