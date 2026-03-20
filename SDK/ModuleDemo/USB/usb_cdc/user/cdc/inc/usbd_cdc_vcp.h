#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

#include "usbd_cdc_core.h"
#include "register.h"

typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;

#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1


#endif /* __USBD_CDC_VCP_H */
