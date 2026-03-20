#include "board_config.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_dcd_init.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"
#include "yc_plic.h"

volatile uint8_t usb_rxbuf[Usb_Rx_Bufsize] = {0};
volatile uint8_t usb_txbuf0[Usb_Endpoint0tx_Bufsize] = {0};
volatile uint8_t usb_txbuf1[Usb_Endpoint1tx_Bufsize] = {0};
volatile uint8_t usb_txbuf2[Usb_Endpoint2tx_Bufsize] = {0};
volatile uint8_t usb_txbuf3[Usb_Endpoint3tx_Bufsize] = {0};
USB_OTG_CORE_HANDLE USB_OTG_dev;

void USB_Init()
{
    SYSCTRL_ClkEn(CLK_USB, ENABLE);
    CORE_USB->usb_trig.bit.rst_udc=0x1;
    CORE_USB->host_md.bit.dp_pu1p5 = 1;/*D+ pullup 1.5K*/
    CORE_USB->ctrl.reg = 0;
    CORE_USB->ctrl.reg = 0x3c;/*DISABLE ISO for endpoint 2 OUT and IN,ENABLE USB,full speed*/
    CORE_USB->usb_rx_saddr.reg = (uint32_t)(usb_rxbuf);
    CORE_USB->usb_rx_buflen.reg = sizeof(usb_rxbuf);
    CORE_USB->usb_tx_saddr0.reg = (uint32_t)(usb_txbuf0);
    CORE_USB->usb_tx_saddr1.reg = (uint32_t)(usb_txbuf1);
    CORE_USB->usb_tx_saddr2.reg = (uint32_t)(usb_txbuf2);
    CORE_USB->usb_tx_saddr3.reg = (uint32_t)(usb_txbuf3);
    CORE_USB->usb_rxptr.reg = (uint32_t)(usb_rxbuf);
    CORE_USB->host_md.bit.dma_en = 1;
    CORE_USB->usb_irq_mask.bit.mask = 0xfd7;/*ENABLE rst rx_ready ep0_txdone ep1_txdone ep2_txdone ep3_txdone tx_done interrupt*/
    CORE_USB->usb_epmap.bit.ep1 = VIRTUAL_OUT_EP1;/*Endpoint1 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep2 = VIRTUAL_OUT_EP2;/*Endpoint2 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep3 = VIRTUAL_OUT_EP3;/*Endpoint3 virtual mapping*/
    memset(&USB_OTG_dev, 0, sizeof(USB_OTG_dev));
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USRD_cb);
}

int main(void)
{
    uint8_t Rxbuf[CDC_DATA_MAX_PACKET_SIZE] = {0};
    uint32_t TotalRcvLen = 0;
    uint32_t packetLen = 0;

    SYSCTRL_ChipInit();

    printport_init();
    yc_log("USB cdc demo");
    USB_Init();
    PLIC_SetPriority(USB_IRQn, 3);
    PLIC_EnableIRQ(USB_IRQn);
    while (1)
    {
        packetLen = CDC_GetOutDataLen(CDC_OUT_EP);
        if (packetLen > 0)
        {
            packetLen = packetLen>CDC_DATA_MAX_PACKET_SIZE? CDC_DATA_MAX_PACKET_SIZE:packetLen;
            CDC_DataOut(CDC_OUT_EP, Rxbuf, packetLen);
            TotalRcvLen += packetLen;
            yc_log("packetLen=%d TotalRcvLen=%d", packetLen, TotalRcvLen);
            CDC_DataIn(CDC_IN_EP,Rxbuf,packetLen);
        }
    }
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
