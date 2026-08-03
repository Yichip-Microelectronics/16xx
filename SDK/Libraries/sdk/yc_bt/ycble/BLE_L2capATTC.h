#ifndef __ATTC_H__
#define __ATTC_H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "le.h"
#include "BLE_L2capATTS.h"
#ifdef __cplusplus
extern "C" {
#endif

void le_master_service_discovery(dev_handle_t dhandle);
void le_master_service_free(leconn *ct);
__LEATTFN_WEAK_ attc_error_response(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_exchange_mtu_response(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_find_info_response(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_read_by_type_response(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_read_group_type_response(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_notify_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ attc_indicate_req(dev_handle_t dhandle,uint8_t *rxp);

void le_send_att_mtu_req(dev_handle_t dhandle);
void le_send_att_mtu_rsp(dev_handle_t dhandle);
void le_send_att_write_cmd(dev_handle_t dhandle,uint16_t attr_handle,uint8_t *attr_val, uint16_t attr_len);
void le_send_att_write_req(dev_handle_t dhandle, uint16_t attr_handle, uint8_t *attr_val, uint16_t attr_len);
void le_send_att_prepwrite_req(dev_handle_t dhandle, uint16_t attr_handle, uint16_t offset, uint8_t *attr_val, uint16_t attr_len);
void le_send_att_execwrite_req(dev_handle_t dhandle, uint8_t flags);
void le_send_att_indicate_rsp(dev_handle_t dhandle);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_GATT_H__ */