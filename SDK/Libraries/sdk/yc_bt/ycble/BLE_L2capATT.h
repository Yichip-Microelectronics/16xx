#ifndef __ATT_H__
#define __ATT_H__
#include "types.h"

typedef struct
{
    word local_mtu;
    word remote_mtu;
} ATT_MTU;

void le_send_att_notify(word handle, word len, byte *p);
void le_send_att_write_command(word handle, word len, byte *p);
void le_send_att_read_rsp(word handle, word len, byte *p);
void le_send_att_read_bytype_rsp(word handle, word len, byte *p);
void le_master_service_discovery(void);
void le_send_att_ex_mtu();
void le_parse_att(byte *rxp);
byte *le_find_att_value_ptr(word handle);
byte le_find_att_valuelen(word handle);
word le_search_uuid(word uuid, word start, word end);
void ble_find_ATTHandleByUUID(byte *att_list, uint16_t uuid, uint16_t *handle);
void le_send_att_read_req(uint16_t handle);
#endif // __ATT_H__
