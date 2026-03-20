#include "yc_ipc.h"
#include "le.h"
#include "yc_bt_evt.h"
#include "hw.h"
#include "yc_user.h"
#include "yc_bt.h"
#include "config.h"
#include "yc_types.h"
#include "yc_log.h"
/**
  *@brief Accept packages from the handle.
  *@param None.
  *@return None.
  */
void IPC_HandleRxPacket(void)
{
    int cmd;
    while((cmd = get_queue(evt_queue, sizeof(evt_queue), &s.cwptr, &s.crptr))) {
            IPC_ParseEvtCallback(cmd);
    }
}

/**
 * @brief  parse ble data callback function
 *
 * @param  len : data length
 *
 * @param  *dataPtr :  data buffer to be parse
 *
 * @retval none
 */
void IPC_ParseBleCallback(uint8_t *dataPtr)
{
    switch(dataPtr[6])
    {
    case ATTOP_WRITE_COMMAND:
    case ATTOP_WRITE_REQUEST:
        U_BleParseWriteData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);
        break;
    case ATTOP_HANDLE_VALUE_NOTIFICATION:
    case ATTOP_HANDLE_VALUE_INDICATION:
        U_BleParseNotifyData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);
        break;
    case ATTOP_READ_REQUEST:
        U_BleParseReadData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);

    case ATTOP_READ_BY_TYPE_REQUEST:
        U_BleParseReadByTypeData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);
        break;
    case ATTOP_PREPARE_WRITE_REQUEST:
        U_BleParsePrepareWriteData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);
        break;
    case ATTOP_EXECUTE_WRITE_REQUEST:
        U_BleParseExecuteWriteData((dataPtr[2] | (dataPtr[3]<<8))-1,&dataPtr[7]);
        break;
    default:
        break;
    }
    free(dataPtr);
}


/**
 * @brief    app event callback function
 *			 etc:	100ms timer ; Bt event
 */
void IPC_ParseEvtCallback(uint32_t dataPtr)
{
    byte *p;
    p = (byte *) (dataPtr >> 8);
    switch(dataPtr & 0xff)
    {
    case HCI_EVENT_LE_LL_RX_PARSE:
        le_parse_llid(p);
        free(p);
        break;
    case HCI_EVENT_LE_CONN_REP:
        yc_bt_log("HCI_EVENT_LE_CONN_REP");
        BT_BleConnectParmUpdateReq((uint8_t*)ble_conn_param,sizeof(ble_conn_param));
        BT_BleUpdateMtu(BLE_LOCAL_MTU);
        break;

    case HCI_EVENT_LE_DIS_REP:
        yc_bt_log("HCI_EVENT_LE_DIS_REP");
        memset(&s, 0, sizeof(s));
        memset(&ct, 0, sizeof(ct));
        BT_SetVisibility(ENABLE);
        break;
    case HCI_EVENT_SLEEP_INTERVAL:
        //yc_bt_log("HCI_EVENT_SLEEP_INTERVAL");
        // if(queue_empty(s.cwptr, s.crptr)) {
        //     ENTER_LPM
        //     switch(s.state)
        //     {
        //         case STATE_ADV:
        //             le_lpm_enter(((tmrtask*)s.advtimer)->timer-20);
        //             break;
        //         case STATE_SLAVE_RX:
        //             le_lpm_enter(ct != NULL ? ct->anchor - ANCHOR_AHEAD(ct->rxwin) :0);
        //             break;
        //         default:
        //             break;
        //     }
        // }
        break;
    case HCI_EVENT_LE_DATA_REP:
        yc_bt_log("HCI_EVENT_LE_DATA_REP");
        IPC_ParseBleCallback(p);
        break;
    case HCI_EVENT_MASTER_SCAN_ADV_DATA:
        yc_bt_log_buf("HCI_EVENT_MASTER_SCAN_ADV_DATA",p,p[1]+2);
        U_BleParseScanAdvData(p);
        free(p);
        break;
    case HCI_EVENT_MASTER_SERVICE_DISCOVER_DONE:
        yc_bt_log("HCI_EVENT_MASTER_SERVICE_DISCOVER_SUCCESS");
        ct->att_ptr = (att_record *)p;
        //TODO: 发起配对
        le_master_start_pairing();
        break;
    case HCI_EVENT_ENC_DONE:
        yc_bt_log("HCI_EVENT_ENC_DONE");
        if(ct->status & CONN_MASTER)
        {
            for(int i=1; ; i++)
            {
                if(ct->att_ptr[i].uuid ==0) break;
                le_send_att_read_req(i);
            }
        }
        break;
    case HCI_EVENT_LE_INPUT_GKEY:
        yc_bt_log("HCI_EVENT_LE_INPUT_GKEY_WAIT");
        if(smp_passkey_wait_input()<=0)
        {
            add_timeout_timer(1000, putcmd,HCI_EVENT_LE_INPUT_GKEY);
        }
        break;
    default:
        break;
    }
    return;
}