#ifndef __YC_IPC_H__
#define __YC_IPC_H__
#include <stdint.h>

void IPC_HandleRxPacket(void);
void IPC_ExceptionCallback(uint8_t len,uint8_t *pdata);
void IPC_ParseBleCallback(uint8_t *dataPtr);
void IPC_ParseEvtCallback(uint32_t dataPtr);
#endif //__YC_IPC_H__