#include "usb_ota.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_dcd_init.h"
#include "yc_plic.h"
#include "yc_flash.h"
#include "flash.h"

volatile uint8_t   usb_rxbuf [Usb_Rx_Bufsize] = {0} ;
volatile uint8_t   usb_txbuf0[Usb_Endpoint0tx_Bufsize]  = {0} ;
volatile uint8_t   usb_txbuf1[Usb_Endpoint1tx_Bufsize]  = {0} ;
volatile uint8_t   usb_txbuf2[Usb_Endpoint2tx_Bufsize]  = {0} ;
volatile uint8_t   usb_txbuf3[Usb_Endpoint3tx_Bufsize]  = {0} ;

USB_OTG_CORE_HANDLE  USB_OTG_dev;

uint8_t rev_data[HID_EP_MAX_PACKET] = {0};
uint8_t tx_buffer[HID_EP_MAX_PACKET] = {0};
uint32_t total_packet_num, now_packet_num, check_sum, last_packet_len, pc_check_sum;
extern uint32_t crc32_write_res;
extern uint32_t crc32_read_res;
extern OTA_CONFIG ota_config;
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
    ota_config.updateaddr=OTA_APP_ADDR;
    PLIC_SetPriority(USB_IRQn,3);
    PLIC_EnableIRQ(USB_IRQn);
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}

uint8_t halUsbDataAck(uint8_t *buf)
{
    if (HID_GetOutDataLen(HID_OUT_EP) >= HID_EPOUT_SIZE)
    {
        HID_DataOut(HID_OUT_EP,buf,HID_EPOUT_SIZE);
        return HAVE_DATA;
    }
    else
    {
        return NO_DATA;
    }
}

/**
 * @method USB_OTA_END
 * @brief  OTA end. Running sum check.
 * @param  none
 * @retval none.
 */
void USB_OTA_END(void)
{
    /******************CMD_OTA_END***************/
    pc_check_sum =  rev_data[4] + (rev_data[5] << 8) + (rev_data[6] << 16);
    tx_buffer[0] = 0xAB;
    tx_buffer[1] = 0xC2;
    tx_buffer[2] = rev_data[2];
    tx_buffer[3] = rev_data[3];
    yc_log("OTA end!\r\n");
    if (pc_check_sum == (check_sum & (0xffffff)))
    {
        tx_buffer[4] = 0x55;
        ota_config.SetAppFlag = 1;
        yc_log("pc_check_sum sucess\n\n");
    }
    else
    {
        tx_buffer[4] = 0xAA;
        /* soft reset */
        SYSCTRL_SoftReset();
    }
    HID_DataIn(HID_IN_EP,tx_buffer,sizeof(tx_buffer));
}

/**
 * @method USB_OTA_DATA
 * @brief  OTA data transmission. Write flash.
 * @param  none
 * @retval none.
 */
void USB_OTA_DATA(void)
{
    /******************CMD_OTA_DATA***************/
    now_packet_num =  rev_data[2] + (rev_data[3] << 8);
    if (now_packet_num < total_packet_num)
    {
        for (int i = 6; i < 64 ; i++)
        {
            ota_config.buffer[ota_config.offset++] = rev_data[i];
            if (ota_config.offset == OTA_BUFFER_LEN)
            {
                FLASH_SectorErase(ota_config.updateaddr);
                crc32_write_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                FLASH_Write(ota_config.updateaddr,OTA_BUFFER_LEN, ota_config.buffer);
                memset(ota_config.buffer, 0, sizeof(ota_config.buffer));
                FLASH_Read(ota_config.updateaddr,OTA_BUFFER_LEN, ota_config.buffer);
                crc32_read_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                if (crc32_write_res!=crc32_read_res)
                {
                    yc_log("flash write error");
                }
                ota_config.updateaddr += 0x1000;
                ota_config.offset = 0;
                memset(ota_config.buffer, 0, sizeof(ota_config.buffer));
            }
        }
        tx_buffer[0] = 0xAB;
        tx_buffer[1] = 0xC1;
        tx_buffer[2] = rev_data[2];
        tx_buffer[3] = rev_data[3];
        for (int i = 6; i < 64; i++)
        {
            check_sum = check_sum + rev_data[i];
        }
        memset(rev_data, 0, 64);
        HID_DataIn(HID_IN_EP,tx_buffer,sizeof(tx_buffer));
    }
    else
    {
        last_packet_len = rev_data[4] + (rev_data[5] << 8);
        for (int i = 6; i < (last_packet_len + 6); i++)
        {
            ota_config.buffer[ota_config.offset++] = rev_data[i];
            if (ota_config.offset == OTA_BUFFER_LEN)
            {
                FLASH_SectorErase(ota_config.updateaddr);
                crc32_write_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                FLASH_Write(ota_config.updateaddr, OTA_BUFFER_LEN,ota_config.buffer);
                memset(ota_config.buffer, 0, sizeof(ota_config.buffer));
                FLASH_Read(ota_config.updateaddr,OTA_BUFFER_LEN, ota_config.buffer);
                crc32_read_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                if (crc32_write_res!=crc32_read_res)
                {
                    yc_log("flash write error");
                }
                ota_config.updateaddr += 0x1000;
                ota_config.offset = 0;
                memset(ota_config.buffer, 0, sizeof(ota_config.buffer));
            }
        }

        if(ota_config.offset != 0)
        {
            FLASH_SectorErase(ota_config.updateaddr);
            crc32_write_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
            FLASH_Write(ota_config.updateaddr, OTA_BUFFER_LEN,ota_config.buffer);
            memset(ota_config.buffer, 0, sizeof(ota_config.buffer));
            FLASH_Read(ota_config.updateaddr,OTA_BUFFER_LEN, ota_config.buffer);
            crc32_read_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
            if (crc32_write_res!=crc32_read_res)
            {
                yc_log("flash write error");
            }
        }

        tx_buffer[0] = 0xAB;
        tx_buffer[1] = 0xC1;
        tx_buffer[2] = rev_data[2];
        tx_buffer[3] = rev_data[3];
        for (int i = 6; i < last_packet_len + 6; i++)
        {
            check_sum = check_sum + rev_data[i];
        }
        HID_DataIn(HID_IN_EP,tx_buffer,sizeof(tx_buffer));
    }
}

/**
 * @method USB_OTA_START
 * @brief  OTA check CMD. running OTA.
 * @param  none
 * @retval none.
 */
void USB_OTA_START(void)
{
    /******************CMD_OTA_START***************/
    total_packet_num = rev_data[2] + (rev_data[3] << 8);

    tx_buffer[0] = 0xAB;
    tx_buffer[1] = 0xC0;
    tx_buffer[2] = 0x3A;
    tx_buffer[3] = 0x00;
    HID_DataIn(HID_IN_EP,tx_buffer,sizeof(tx_buffer));

    memset(tx_buffer, 0, sizeof(tx_buffer));
}

/**
 * @method Cmd_check
 * @brief  check OTA.
 * @param  none
 * @retval none
 */
void Cmd_check(void)
{
    if (halUsbDataAck(rev_data) == HAVE_DATA)
    {
        switch ((rev_data[0] | rev_data[1] << 8))
        {
            case 0xC0BA:
                USB_OTA_START();
                break;

            case 0xC1BA:
                USB_OTA_DATA();
                break;

            case 0xC2BA:
                USB_OTA_END();
                break;
        }
    }
}

