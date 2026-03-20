#include "usb_dcd_init.h"
#include "usbd_usr.h"
#include "usbd_ioreq.h"
#include "register.h"

static uint32_t DCD_HandleInEP_ISR(USB_OTG_CORE_HANDLE *pdev, uint16_t ep_intr);
uint32_t DCD_HandleOutEP_ISR(USB_OTG_CORE_HANDLE *pdev);
uint32_t DCD_HandleUsbReset_ISR(USB_OTG_CORE_HANDLE *pdev);

void _delay_(uint32_t t)
{
    ((void (*)())(0xc6 + 1))(t);
}

/**
* @brief  USBD_OTG_ISR_Handler
*         handles all USB Interrupts
* @param  pdev: device instance
* @retval status
*/


extern volatile uint8_t tx_sendflag;

extern USBD_DCD_INT_cb_TypeDef USBD_DCD_INT_cb;

uint8_t setup_cnt = 0;
uint8_t SetAddress_Flag = 0;
uint8_t Address_Value = 0;


USB_OTG_IRQ_TypeDef gintr_status;

extern uint8_t AudioCtl[64];
extern uint8_t AudioCtlCmd;
extern uint8_t epmap[];

uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE *pdev)
{
    uint32_t retval = 0;
    if (USB_OTG_IsDeviceMode(pdev))     /* ensure that we are in device mode */
    {
        gintr_status.d16 = CORE_USB->usb_irq_status.reg; /* 先将中断位保存好,之后再使用 */
        if (!gintr_status.d16)
        {
            return 0;
        }
        CORE_USB->usb_irq_status.reg = gintr_status.d16;
        if (gintr_status.b.reset)
        {
            pdev->dev.device_status = USB_OTG_DEFAULT;
            retval |= DCD_HandleUsbReset_ISR(pdev);
        }
        if (gintr_status.b.suspend)
        {
            pdev->dev.device_old_status = pdev->dev.device_status;
            pdev->dev.device_status  = USB_OTG_SUSPENDED;
            retval = 1;
        }
        if (gintr_status.b.rx_ready)                    /* 当USB设备接收数据完成后，进入这个if，主要作用为进行数据的处理 */
        {
            retval |= DCD_HandleOutEP_ISR(pdev);
            pdev->dev.device_status = USB_OTG_CONFIGURED;
        }
        if (gintr_status.b.tx_donex)                   /* 当USB设备发送数据完成后，主要作用为数据是否已经发送完成(因为我们可以发送最大的一包数据位64字节，如果大于64的话，就要分次发) */
        {
            retval |= DCD_HandleInEP_ISR(pdev, gintr_status.b.ep_txdoen);
        }
    }
    return retval;
}

static uint32_t DCD_HandleInEP_ISR(USB_OTG_CORE_HANDLE *pdev, uint16_t ep_intr)
{
    USB_OTG_EP *ep;
    uint16_t  epnum= 0;

    while (ep_intr)
    {
        ep = &pdev->dev.in_ep[epnum];
        /* Setup and start the Transfer */
        ep->is_in = 1;
        ep->num = epnum;
        if((CORE_USB->usb_in_miss.reg&(1<<epnum))!=0)
        {
            CORE_USB->usb_in_miss.reg=1<<epnum;
            CORE_USB->usb_trig.bit.ep_tx=1 << (epnum);
        }
        else
        {
            if (ep_intr & 0x01) /* In ITR */
            {
                if(epnum == 0)
                {
                    if(ep->rem_data_len != 0)
                    {
                        USB_OTG_EP0StartXfer(pdev, ep); /* 回调 USB_OTG_EP0StartXfer 发送上次未发完的数据 */
                    }
                }
                else
                {
                    USBD_DCD_INT_fops->DataInStage(pdev, epnum);
                }
            }
            epnum++;
            ep_intr >>= 1;
        }
    }
    return 1;
}



void usb0_control_data(USB_OTG_CORE_HANDLE *pdev, uint16_t len)
{
}
#ifdef USB_SOFT_DMA_WORK
uint8_t gIsFirstBufferCache = 10;
extern volatile uint8_t   usb_rxbuf[Usb_Rx_Bufsize] ;
extern volatile uint8_t   usb_txbuf0[100] ;
extern volatile uint8_t   usb_txbuf1[70] ;
extern volatile uint8_t   usb_txbuf2[70] ;
extern volatile uint8_t   usb_txbuf3[70];

void usbcpy_wrap(void *dst, int len, void *dst_start, int dst_len)
{
    dmacpy_wrap(dst, usb_rxptr, len, dst_start, dst_len, usb_rxbuf, sizeof(usb_rxbuf));
}

void usbcpy(void *dst, int len)
{
    dmacpy_wrap(dst, usb_rxptr, len, 0, -1, usb_rxbuf, sizeof(usb_rxbuf));
}
#define INCANTIOF(p, inc, buf)     if((p += inc) >= buf + sizeof(buf)) p -= sizeof(buf)
#define INC_RPTR(x)             INCANTIOF(usb_rxptr, x, usb_rxbuf)
#endif
/**
* @brief  DCD_HandleOutEP_ISR
*         Indicates that an OUT EP has a pending Interrupt
* @param  pdev: device instance
* @retval status
*/
uint32_t DCD_HandleOutEP_ISR(USB_OTG_CORE_HANDLE *pdev)
{
    uint16_t rx_ep;
    uint8_t ep_num, i, rxcnt;

    rxcnt = CORE_USB->usb_rxcnt.reg;
    for (i = 0, rxcnt = rxcnt; gintr_status.b.rx_ready; i++)
    {
        if (rxcnt == ((i + pdev->dev.last_rxcnt) & 0xff))
        {
            pdev->dev.rxcnt = rxcnt;
            pdev->dev.last_rxcnt += i;
            break;

        }
        usb_rxptr = (uint8_t *)((CORE_USB->usb_rxptr.reg));
        rx_ep = usb_getword();
        rx_DataLength = rx_ep & 0xfff;
        ep_num = (uint8_t)(rx_ep >> 12);
        if (ep_num == 0)
        {
            if (SetAddress_Flag)
            {
                CORE_USB->addr.bit.dis_broadcast = 1;
                SetAddress_Flag = 0;
            }
            if (rx_DataLength == USB_SETUP_PACKET_LEN)
            {
                /* Copy the setup packet received in FIFO into the setup buffer in RAM */
                USB_OTG_ReadPacket(pdev, pdev->dev.setup_packet, 0, USB_SETUP_PACKET_LEN);

                /* deal with receive setup packet */
                USBD_DCD_INT_fops->SetupStage(pdev);
            }
            else
            {
                /* deal with ep0 out packet */
                USBD_DCD_INT_fops->DataOutStage(pdev, ep_num);
            }
        }
        else
        {



            /* deal with receive ep1,ep2,ep3 out packet */
            USBD_DCD_INT_fops->DataOutStage(pdev, ep_num);
        }
    }
    return 1;
}

/**
* @brief  DCD_HandleUsbReset_ISR
*         This interrupt occurs when a USB Reset is detected
* @param  pdev: device instance
* @retval status
*/
/**
 * @method usb_reset
 * @brief  None
 * @param  None
 * @retval None
 */
void usb_reset()
{
    CORE_USB->addr.reg = 0;
}
extern USB_OTG_CORE_HANDLE USB_OTG_dev;
uint32_t DCD_HandleUsbReset_ISR(USB_OTG_CORE_HANDLE *pdev)
{
    usb_reset();
    //USBD_DCD_INT_fops->Reset(pdev);
    return 1;
}
void usb_clkcal_done()
{
    // dsmdiv_12m_adjust_by_usb_sof(HREADW(core_usb_clkcnt));

}

