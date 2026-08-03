#ifndef __YC_USER_H__
#define __YC_USER_H__
#include <stdint.h>
#include "yc_bt.h"
#include "BLE_L2capATTS.h"
extern uint8_t gen_ble_adv_normal[ADV_MAX_LEN];
extern uint8_t gen_ble_scan_rsp[ADV_MAX_LEN];
extern uint16_t yc_rx_handle,yc_tx_handle;
void U_BleSetMac(uint8_t * bleAddr);
void U_BleSetDeviceName(char *name_in,uint8_t *bleAddr);
void U_BleGenAdvScanData(uint8_t *adv_data,uint8_t *scan_rsp,uint8_t *name,uint8_t name_len,uint8_t *mac);
void U_BleParseWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseReadData(uint16_t len,uint8_t *dataPtr);
void U_BleParseReadByTypeData(uint16_t len, uint8_t *dataPtr);
void U_BleParsePrepareWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseExecuteWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseScanAdvData(uint8_t *p);
void U_BleParseNotifyData(uint16_t len,uint8_t *dataPtr);
int U_smp_passkey_wait_input(dev_handle_t dhandle);
void U_smp_passkey_wait_input_timeout(dev_handle_t dhandle);
int U_ATT_GenericAccess_CbRead(dev_handle_t dhandle, uint16_t att_handle,
    const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
int U_ATT_User_ServiceCbWrite(dev_handle_t dhandle, uint16_t att_handle,
    const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
void U_BleServiceParse(dev_handle_t dhandle);
void U_BleMasterParseNotifyData(dev_handle_t dhandle,uint8_t *rxp);
#endif // !__YC_USER_H__