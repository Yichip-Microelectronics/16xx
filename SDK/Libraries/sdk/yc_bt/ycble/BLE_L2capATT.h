#ifndef __ATT_H__
#define __ATT_H__
#include "yc_types.h"
#include "le.h"

void le_parse_att(dev_handle_t dhandle, byte *rxp);
void le_send_att_read_req(dev_handle_t dhandle, uint16_t attr_handle);
#endif // __ATT_H__
