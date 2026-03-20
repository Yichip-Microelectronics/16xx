#include "board_config.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_dcd_init.h"
#include "usbd_hid_core.h"
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
    CORE_USB->usb_epmap.bit.ep1 = HID_OUT_EP;/*Endpoint1 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep2 = 2;/*Endpoint2 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep3 = 3;/*Endpoint3 virtual mapping*/
    memset(&USB_OTG_dev, 0, sizeof(USB_OTG_dev));
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_cb, &USRD_cb);
}

uint8_t hid_data[HID_EP_MAX_PACKET];
int main(void)
{
    SYSCTRL_ChipInit();

    printport_init();
    yc_log("usb hid demo");
    USB_Init();
    PLIC_SetPriority(USB_IRQn, 3);
    PLIC_EnableIRQ(USB_IRQn);
    while (1)
    {
        if (HID_GetOutDataLen(HID_OUT_EP) >= HID_EPOUT_SIZE)
        {
            HID_DataOut(HID_OUT_EP, hid_data, sizeof(hid_data));
            hid_data[0] = HID_REPORT_ID;
            HID_DataIn(HID_IN_EP, hid_data, HID_EPIN_SIZE);
        }
    }
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
