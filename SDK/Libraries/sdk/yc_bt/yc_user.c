
#include "yc_user.h"
#include "yc_bt.h"
#include "string.h"
#include "le.h"
#include "yc_log.h"
#include "ble_ll_scan.h"
#include "yc_flash.h"
#include "BLE_L2capATT_List.h"
#include "BLE_L2capATTS.h"
#include "yc_bt_evt.h"
#include "BLE_L2capATTC.h"
LPM_RETENTION uint8_t gen_ble_adv_normal[ADV_MAX_LEN];
LPM_RETENTION uint8_t gen_ble_scan_rsp[ADV_MAX_LEN];
LPM_RETENTION uint16_t  yc_rx_handle,yc_tx_handle,yc_name_handle;

const uint8_t hex_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static void array2hex(uint32_t in, uint8_t inlen, uint8_t *out)
{
    uint8_t i;
    for (i = 0; i < inlen; ++i)
    {
        out[2 * i] = hex_lookup[(in >> (4+i*8)) & 0x0f];
        out[2 * i + 1] = hex_lookup[(in>>(i*8)) & 0x0f];
    }
}
/*********************************************
 * FunctionName : U_BleSetMac
 * Description  : 设置蓝牙地址,通过flash id的crc16值生成唯一地址
 * Parameters   : bleAddr :蓝牙地址指针
 * Returns      : NA
 **********************************************/
__WEAK void U_BleSetMac(uint8_t * bleAddr)
{
    uint8_t flash_id[16];
    FLASH_GetUniqueID(flash_id);
    uint16_t crc = ((uint16_t(*)(uint16_t base_crc, uint8_t* data, unsigned int len))ROM_API_CRC16)(0xffff, flash_id, 16);
    bleAddr[4] = crc & 0xff;
    bleAddr[5] = crc >> 8;
    BT_BleSetMac(bleAddr);
}
/*********************************************
 * FunctionName : U_BleSetDeviceName
 * Description  : 设置蓝牙设备名称,通过将设备名称和蓝牙地址拼接起来,防止设备名称重复
 * Parameters   : name_in :设备名称指针
 *                bleAddr :蓝牙地址指针
 * Returns      : NA
 **********************************************/
__WEAK void U_BleSetDeviceName(char *name_in,uint8_t *bleAddr)
{
    uint8_t name_index,i;
    char ble_name_index[16];

    for (i = 0,name_index = 0; i < strlen(name_in); name_index++, i++) {
        if (name_in[i] == 0)
            break;
        ble_name_index[name_index] = name_in[i];
    }
    array2hex(bleAddr[4]<<8|bleAddr[5],2,(uint8_t *)ble_name_index+name_index);
    array2hex(bleAddr[4]<<8|bleAddr[5],2,(uint8_t *)name_in+name_index);
    name_index += 4;

    // yc_log_buf("ble_name_gen", (uint8_t *)ble_name_index, (name_index));
    BT_BleSetName((uint8_t *)ble_name_index, name_index);//蓝牙名称配置

}
/*********************************************
 * FunctionName : U_BleGenAdvScanData
 * Description  : 生成广播数据和扫描响应数据
 * Parameters   : adv_data :广播数据指针
 *                scan_rsp :扫描响应数据指针
 *                name :设备名称指针
 *                name_len :设备名称长度
 *                mac :蓝牙地址指针
 * Returns      : NA
 **********************************************/
__WEAK void U_BleGenAdvScanData(uint8_t *adv_data,uint8_t *scan_rsp,
                            uint8_t *name,uint8_t name_len,uint8_t *mac)
{
    uint8_t adv_index,scan_index;
    adv_index = 1;
    adv_data[adv_index++] = 0x02;
    adv_data[adv_index++] = 0x01;
    adv_data[adv_index++] = 0x06;
    //manufacturer info
    adv_data[adv_index++] = 0x09;
    adv_data[adv_index++] = 0xff;//manufacturer info flag
    adv_data[adv_index++] = 0xff;
    adv_data[adv_index++] = 0xff;
    for(int i=0; i<6; i++)      //mac
    {
        adv_data[adv_index++] = *(mac + i);
    }
    adv_data[0] = adv_index - 1;
    scan_index = 1;
    scan_rsp[scan_index++] = name_len+1;            //+ name flag len

    scan_rsp[scan_index++] = 0x09;
    for(int i=0; i<name_len; i++)      //name
    {
        scan_rsp[scan_index++] = *(name+i);
    }
    scan_rsp[0] = scan_index - 1;
}
/*********************************************
 * FunctionName : U_BleParseScanAdvData
 * Description  : 解析ble扫描到的对端广播数据，用于ble做主时连接使用
 * Parameters   : len :接收数据包长度
 *                dataPtr :接收数据包指针
 * Returns      : NA
 **********************************************/
__WEAK void U_BleParseScanAdvData(uint8_t *p)
{
    if(memcmp(dev->conn_mac, p + ADV_ADDR, 6) == 0)
    {
        le_start_conn();
    }
    else
        le_start_scan();
}
// ======================== Passkey 输入处理函数 ========================

// 处理 UART 接收的 passkey 字符
// 返回: 0=继续输入, 1=输入完成
static int U_smp_passkey_input_char(dev_handle_t dhandle, char c)
{
	leconn *ct = GET_CT(dhandle);
	if (c >= '0' && c <= '9') {
		ct->passkey_ctx->passkey_input[ct->passkey_ctx->input_idx++] = c;

		// SC模式: 发送DIGIT_ENTERED通知
		if (ct->sc_ctx && ct->sc_ctx->sc_method >= SC_METHOD_PK_INIT_INPUT) {
			sc_send_keypress_notification(dhandle, KEYPRESS_DIGIT_ENTERED);
		}

		if (ct->passkey_ctx->input_idx >= 6) {
			yc_bt_log_buf("   passkey_input", ct->passkey_ctx->passkey_input, 6);
			ct->passkey_ctx->passkey_input[6] = '\0';
			return 1; // 输入完成
		}
	} else if (c == '\b' || c == 0x7f) {
		// 退格处理
		if (ct->passkey_ctx->input_idx > 0) {
			ct->passkey_ctx->input_idx--;
			yc_bt_log("\b \b");
			if (ct->sc_ctx && ct->sc_ctx->sc_method >= SC_METHOD_PK_INIT_INPUT) {
				sc_send_keypress_notification(dhandle, KEYPRESS_DIGIT_ERASED);
			}
		}
	}
	return 0;
}

static int U_smp_passkey_transfer(dev_handle_t dhandle)
{
    int passkey = 0;
    // 将用户输入转换为 passkey
	for (int i = 0; i < 6; i++) {
		passkey = passkey * 10
		                           + (ct->passkey_ctx->passkey_input[i] - '0');
	}
    return passkey;
}

/*return <0: not ready,
         >=0 passkey */
int U_smp_passkey_ext_getkey(dev_handle_t dhandle)
{
    char c;
    int done = 0;
    int passkey = -1;
    // 处理所有可用的字符
    while (UART_GetRxCount(PRINT_PORT)) {
        c = UART_ReceiveData(PRINT_PORT);
        yc_bt_log("%c",c);
        done = U_smp_passkey_input_char(dhandle, c);
        if (done) {
            // 输入完成，继续配对流程
            passkey = U_smp_passkey_transfer(dhandle);
            yc_bt_log("\r\nPasskey: %06d\r\n", passkey);
            break;
        }
    }
    return passkey;

}

int U_smp_passkey_wait_input(dev_handle_t dhandle)
{
	leconn *ct = GET_CT(dhandle);
    // 检查是否在等待 passkey 输入
    if (!ct->passkey_ctx || ct->passkey_ctx->passkey_state != 1) {
        return -1;
    }
    ct->passkey_ctx->passkey = U_smp_passkey_ext_getkey(dhandle);
    if(ct->passkey_ctx->passkey != -1)
    {
        // Legacy模式: 调用smp_passkey_input_done()
        if (!(ct->sc_ctx)) {
            smp_passkey_input_done(dhandle);
        }
        // SC模式: 调用SC的完成函数
        if (ct->sc_ctx && ct->sc_ctx->sc_method >= SC_METHOD_PK_INIT_INPUT) {
            sc_passkey_input_complete(dhandle);
        }
        return 1;
    }
    return 0;
}

void U_smp_passkey_wait_input_timeout(dev_handle_t dhandle)
{
    putcmd(GET_DEV(dhandle), HCI_EVENT_GET_PASSKEY, 0);
}
__WEAK void U_BleServiceParse(dev_handle_t dhandle)
{
    leconn *ct = GET_CT(dhandle);
    for(int i = 1; i < 100; i++)
    {
        if(ct->att_ptr[i].uuid == 0) break;
        uint32_t uuid,uuid_type;
        if(ct->att_ptr[i].uuid_flag == 1)
        {
            uuid = ct->att_ptr[i].uuid;
            uuid_type = BLE_UUID_TYPE_16;
        }
        else if(ct->att_ptr[i].p_uuid)
        {
            if(ct->att_ptr[i].p_uuid->type == BLE_UUID_TYPE_16)
            {
                uuid = ((struct ble_uuid_16 *)ct->att_ptr[i].p_uuid)->val;
                uuid_type = BLE_UUID_TYPE_16;
            }
            else if(ct->att_ptr[i].p_uuid->type == BLE_UUID_TYPE_128)
            {
                uuid =(uint32_t)(&((struct ble_uuid_128 *)ct->att_ptr[i].p_uuid)->val);
                uuid_type = BLE_UUID_TYPE_128;
            }
            else
            {
                uuid = 0;
                uuid_type = 0; // Unknown type
            }
        }
        if(uuid == 0x2800)  yc_bt_log("\n");
        yc_bt_log("ATT[%04x] Perm:%04x UUID-%s:", i, ct->att_ptr[i].perm, uuid_type == BLE_UUID_TYPE_16 ? "16" : "128");
        if(uuid_type == BLE_UUID_TYPE_16){
            yc_bt_log("%04x\n",uuid);
        }
        else{
            yc_bt_log_buf("",(uint8_t *)uuid,16);
        }
        if(uuid == 0x2800) //service declaration
        {
            if(ct->att_ptr[i].value_flag == 1)
            {
                yc_bt_log("    Service UUID = %04x",ct->att_ptr[i].value);
            }
            else if(ct->att_ptr[i].p_value)
            {
                yc_bt_log_buf("    Service UUID",ct->att_ptr[i].p_value,ct->att_ptr[i].attr_len);
            }
        }

        if(uuid == 0x2902 && ((ct->att_ptr[i-1].perm & BLE_GATT_CHRC_NOTIFY) != 0))
        {
            if(ct->att_ptr[i-1].uuid_flag == 1)
            {
                uuid = ct->att_ptr[i-1].uuid;
                uuid_type = BLE_UUID_TYPE_16;
            }
            else if(ct->att_ptr[i-1].p_uuid)
            {
                if(ct->att_ptr[i-1].p_uuid->type == BLE_UUID_TYPE_16)
                {
                    uuid = ((struct ble_uuid_16 *)ct->att_ptr[i-1].p_uuid)->val;
                    uuid_type = BLE_UUID_TYPE_16;
                }
                else if(ct->att_ptr[i].p_uuid->type == BLE_UUID_TYPE_128)
                {
                    uuid = (uint32_t)(&((struct ble_uuid_128 *)ct->att_ptr[i].p_uuid)->val);
                    uuid_type = BLE_UUID_TYPE_128;
                }
                else
                {
                    uuid = 0;
                    uuid_type = 0; // Unknown type
                }
            }

            if(uuid == 0xfffa)
            {
                uint8_t ccc_value[2] = {BLE_GATT_CCC_NOTIFY,(BLE_GATT_CCC_NOTIFY>>8)};
                le_send_att_write_req(dev->dhandle,i,ccc_value,sizeof(ccc_value));
                yc_bt_log("le master tx_handle = %04x",i-1);
            }
        }
    }
}

void U_BleMasterParseNotifyData(dev_handle_t dhandle,uint8_t *rxp)
{
    leconn *ct = GET_CT(dhandle);
    uint16_t rlen = GETW(rxp + 2);
    uint16_t handle = GETW(rxp + 7);
    uint8_t *value = rxp + 9;
    uint16_t value_len = rlen - 3;
    if (rlen < 3)return;
    if (ct->att_ptr == NULL|| !(ct->att_ptr[handle].perm & BLE_GATT_CHRC_NOTIFY)) return;
    yc_bt_log("gatt_handle: %04x", handle);
    yc_bt_log_buf("buf:", value, value_len);

}


// ======================== ATT Callback ========================
__WEAK int U_ATT_GenericAccess_CbRead(dev_handle_t dhandle, uint16_t att_handle, const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(BLE_UUID_GAP_DEVICE_NAME_VAL == get_attr_uuid16((struct ble_gatt_attr *)attr))
    {
        memcpy(buf,dev->name,  dev->name_len);
        return dev->name_len;
    }
    return atts_read_value((struct ble_gatt_attr *)attr, (uint8_t *)buf, len);
}

__WEAK int U_ATT_User_ServiceCbWrite(dev_handle_t dhandle, uint16_t att_handle, const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    yc_bt_log("get_attr_uuid16: %x,att_handle: %x", get_attr_uuid16((struct ble_gatt_attr *)attr), att_handle);
    yc_bt_log_buf("buf:", buf, len);
    if (get_attr_uuid16((struct ble_gatt_attr *)attr) == BLE_UUID_GATT_CCC_VAL)
    {
        return atts_write_value((struct ble_gatt_attr *)attr, (uint8_t *)buf, len);
    }
    // 回环写入
    return BT_BleSlaveSendData(dhandle,att_handle, buf,len);
}