#ifndef __USB_DCD_INT_H__
#define __USB_DCD_INT_H__

#include "usb_dcd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_INTRANS_AUTO 1
#define USB_INTRANS_MANUAL 0

typedef struct _USBD_DCD_INT
{
    uint8_t (* DataOutStage)(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
    uint8_t (* DataInStage)(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
    uint8_t (* SetupStage)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* SOF)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Reset)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Suspend)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Resume)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* IsoINIncomplete)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* IsoOUTIncomplete)(USB_OTG_CORE_HANDLE *pdev);

    uint8_t (* DevConnected)(USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* DevDisconnected)(USB_OTG_CORE_HANDLE *pdev);

} USBD_DCD_INT_cb_TypeDef;

extern USBD_DCD_INT_cb_TypeDef *USBD_DCD_INT_fops;

uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE *pdev);

//uint32_t USBD_OTG_DMA_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
uint32_t USBD_OTG_DMA_ISR_Handler(USB_OTG_CORE_HANDLE *pdev, uint8_t dma_intr_value);

uint8_t usb_getbyte();
uint16_t usb_getword();
void usb_clkcal_done();

void usb0_control_data(USB_OTG_CORE_HANDLE *pdev, uint16_t len);
void usb_reset();
void _delay_(uint32_t t);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_DCD_INT_H__ */
