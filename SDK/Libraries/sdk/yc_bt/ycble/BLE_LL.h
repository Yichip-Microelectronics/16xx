#ifndef __LL_H_
#define __LL_H_

typedef enum
{
	LL_ERR_PIN_OR_KEY_MISSING = 0x06,
}LL_ErrCode;


void le_send_ll(dev_handle_t dhandle, uint8_t cmd, uint8_t *p);
#endif //__LL_H_