#ifndef __USB_OTA_H__
#define __USB_OTA_H__


#ifdef __cplusplus
extern "C" {
#endif

#define NO_DATA         0
#define HAVE_DATA       1
#define HAL_OK          0
#define HAL_ERROR       -1


void USB_Init(void);
void Cmd_check(void);


#ifdef __cplusplus
}
#endif

#endif /* __USB_OTA_H__ */

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/


