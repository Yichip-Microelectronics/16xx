#ifndef __LL_H_
#define __LL_H_

typedef struct
{
	uint8_t features[8];
	uint8_t version[5];
}LL_ParamStru ;

void le_send_ll(byte cmd);
#endif //__LL_H_