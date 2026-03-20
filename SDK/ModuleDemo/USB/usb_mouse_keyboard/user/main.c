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

uint8_t hid_receive_data[HID_EPOUT_SIZE] = {0};
uint8_t keyboard_receive_data[KEYBOARD_EPOUT_SIZE] = {0};
uint8_t mouse_receive_data[MOUSE_EPOUT_SIZE] = {0};
uint8_t CapsLock[64] = {0}, CapsLockLen;
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
    CORE_USB->usb_epmap.bit.ep1 = 1;/*Endpoint1 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep2 = 2;/*Endpoint2 virtual mapping*/
    CORE_USB->usb_epmap.bit.ep3 = 3;/*Endpoint3 virtual mapping*/
    memset(&USB_OTG_dev, 0, sizeof(USB_OTG_dev));
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_cb, &USRD_cb);
}

uint8_t keyboard_capslock[8] = {0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t keyboard_uplift[8] = {0};

uint8_t mouse_leftshift[8] = {0x01, 0x00, 0xfa, 0xff, 0x00, 0x00, 0x00, 0x00};/*id botton x y Wheel ACPan undefined*/
uint8_t mouse_rightshift[8] = {0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t mouse_upshift[8] = {0x01, 0x00, 0x00, 0x00, 0xfa, 0xff, 0x00, 0x00};
uint8_t mouse_downshift[8] = {0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00};

int main(void)
{
    SYSCTRL_ChipInit();

    printport_init();
    yc_log("usb mouse&keyboard demo");
    USB_Init();

    PLIC_SetPriority(USB_IRQn, 3);
    PLIC_EnableIRQ(USB_IRQn);
    while (1)
    {
        if (UART_GetRxCount(PRINT_PORT) > 0)
        {
            switch (UART_ReceiveData(PRINT_PORT))
            {
            case 'k':
            case 'K':
                HID_DataIn(KEYBOARD_IN_EP, keyboard_capslock, KEYBOARD_EPIN_SIZE);
                HID_DataIn(KEYBOARD_IN_EP, keyboard_uplift, KEYBOARD_EPIN_SIZE);
                yc_log("casplock");
                break;

            case 'm':
            case 'M':
                for (int i = 0; i < 50; i++)
                {
                    HID_DataIn(MOUSE_IN_EP, mouse_upshift, MOUSE_EPIN_SIZE);
                }
                for (int i = 0; i < 50; i++)
                {
                    HID_DataIn(MOUSE_IN_EP, mouse_rightshift, MOUSE_EPIN_SIZE);
                }
                for (int i = 0; i < 50; i++)
                {
                    HID_DataIn(MOUSE_IN_EP, mouse_downshift, MOUSE_EPIN_SIZE);
                }
                for (int i = 0; i < 50; i++)
                {
                    HID_DataIn(MOUSE_IN_EP, mouse_leftshift, MOUSE_EPIN_SIZE);
                }
                yc_log("mouse test");
                break;
            default:
                break;
            }
        }

        if (HID_GetOutDataLen(HID_OUT_EP) >= HID_EPOUT_SIZE)
        {
            yc_usb_log("hid receive %d bytes data", HID_EPOUT_SIZE);
            HID_DataOut(HID_OUT_EP, hid_receive_data, HID_EPOUT_SIZE);
            HID_DataIn(HID_IN_EP, hid_receive_data, HID_EPIN_SIZE);
        }

        int hid_len = HID_GetOutDataLen(KEYBOARD_OUT_EP);
        if (hid_len > 0)
        {
            yc_log("keyboard receive %d bytes data", hid_len);
            HID_DataOut(KEYBOARD_OUT_EP, keyboard_receive_data, hid_len);
            yc_log_buf("keyboard_receive_data", keyboard_receive_data, hid_len);
        }

        hid_len = HID_GetOutDataLen(MOUSE_OUT_EP);
        if (hid_len > 0)
        {
            yc_log("mouse receive %d bytes data", hid_len);
            HID_DataOut(MOUSE_OUT_EP, mouse_receive_data, hid_len);
            yc_log_buf("mouse_receive_data", mouse_receive_data, hid_len);
        }
    }
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
