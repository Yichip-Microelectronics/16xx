#ifndef __BLE_L2CAP_SIGN_H__
#define __BLE_L2CAP_SIGN_H__

void le_parse_signaling(dev_handle_t dhandle, byte *p_l2cap);
void le_l2cap_tx_updata_req(dev_handle_t dhandle, uint8_t *data);
#endif // __BLE_L2CAP_SIGN_H__
