#include "yc_ipc.h"
#include "yc_bt.h"
#include "app_ble.h"
#include "ota_ble.h"
#include "yc_flash.h"
#include "yc_lpm.h"
extern uint16_t yc_ota_tx_handle;
uint8_t ota_last_pos = 0;
extern uint32_t crc32_write_res;
extern uint32_t crc32_read_res;
uint8_t cmd_para_check(BLE_OTA_CMD check_cmd)
{
    if ((check_cmd == PORROOL_VERSION_REQUEST)  ||
        (check_cmd == BUCK_SIZE_REQUEST)        ||
        (check_cmd == START_REQUEST)            ||
        (check_cmd == DATA_WRITE_CMD)           ||
        (check_cmd == DATA_WRITE_REQUEST)       ||
        (check_cmd == END_REQUEST))
        return 1;
    else
        return 0;
}


void Ble_Cmd_Check(dev_handle_t dhandle,uint8_t* bledata)
{
    uint8_t ota_head;
    uint8_t ota_cmd;
    uint16_t  datalen;
    uint16_t PackgeIndex;
	uint16_t packet_maxlen = PACKET_MAXLEN;
    uint16_t buck_len = BUCK_LEN;
    uint32_t Sumlength = 0;
    uint32_t CheckSum  = 0;
    BleDataPackStr bledatapack;
    BleEndDataPack endDataPack;

    ota_head    = bledata[0];
    ota_cmd     = bledata[1];

    /* OTA cmd check */
    if(!cmd_para_check((BLE_OTA_CMD)ota_cmd))
    {
        return;
    }

    bledatapack.Evt         =   EVTRESPOND;
    bledatapack.ResultCode  =   RESULTTRUE;
    bledatapack.RcvFlag     =   ota_cmd;

    if(ota_head == RECDATAPACKCMD)
    {
        switch (ota_cmd)
        {
            case PORROOL_VERSION_REQUEST:
                bledatapack.Payload[0] = BT_GetLibVersion() & 0xff;
                bledatapack.Payload[1] = ((BT_GetLibVersion() >> 8) & 0xff);
                BT_BleSlaveSendData(dhandle,yc_ota_tx_handle, (uint8_t*)&bledatapack, 5);
                break;

            case BUCK_SIZE_REQUEST:
                memcpy(bledatapack.Payload, &buck_len, 2);
                memcpy((bledatapack.Payload + 2), &packet_maxlen, 2);
                BT_BleSlaveSendData(dhandle,yc_ota_tx_handle, (uint8_t*)&bledatapack, 7);
                break;

            case START_REQUEST:
                bledatapack.Payload[0] = NORMALOTA;
                BT_BleSlaveSendData(dhandle,yc_ota_tx_handle, (uint8_t*)&bledatapack, 4);
                break;

            case DATA_WRITE_CMD:
                PackgeIndex = (uint16_t)(bledata[2]|(bledata[3] << 8));
                datalen  = bledata[4];

                if(PackgeIndex % (OTA_BUFFER_LEN / BLE_OTA_DATA_LEN) == 0)
                {
                    ota_config.pos = 0;
                    ota_last_pos = 1;
                }
                else
                {
                    ota_config.pos++;
                    ota_last_pos = 0;
                }

                memcpy((ota_config.buffer + (ota_config.pos * BLE_OTA_DATA_LEN)), (bledata + 5), datalen);

                /* 计算checksum 和 sumlength */
                ota_config.Sumlength += datalen;
                for(int i = 0; i < datalen; i++)
                {
                    ota_config.CheckSum += ota_config.buffer[(ota_config.pos * BLE_OTA_DATA_LEN) + i];
                }
                break;

            case DATA_WRITE_REQUEST:
                PackgeIndex = bledata[2]|(bledata[3] << 8);
                datalen  = bledata[4];

                if((PackgeIndex + 1) % (OTA_BUFFER_LEN / BLE_OTA_DATA_LEN) == 0)
                {
                    /* buf满4K了 */
                    ota_config.isBufferFull = true;
                }


                if (ota_config.pos!=0)
                {
                    ota_config.pos++;
                }
                if(ota_last_pos == 1)
                {
                    ota_config.pos++;
                    ota_last_pos = 0;
                }

                memcpy((ota_config.buffer + (ota_config.pos * BLE_OTA_DATA_LEN)), (bledata + 5), datalen);

                /* 计算checksum 和 sumlength */
                ota_config.Sumlength += datalen;
                for(uint8_t i = 0; i < datalen; i++)
                {
                    ota_config.CheckSum += ota_config.buffer[(ota_config.pos * BLE_OTA_DATA_LEN) + i];
                }

                /* buff满4K */
                if(ota_config.isBufferFull)
                {
                    ota_config.isBufferFull = false;
                    ota_config.pos = 0;

                    /* 满 4k 写flash */
                    ota_config.updateaddr = UpdateFirmwareAddr(Get_OffsetAddr());

                    FLASH_SectorErase(ota_config.updateaddr + ota_config.writedLen);
                    crc32_write_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                    /* 写qspi flash */
                    FLASH_Write(ota_config.updateaddr + ota_config.writedLen, OTA_BUFFER_LEN,ota_config.buffer);
                    memset(ota_config.buffer, 0xff, OTA_BUFFER_LEN);
                    FLASH_Read(ota_config.updateaddr + ota_config.writedLen, OTA_BUFFER_LEN,ota_config.buffer);
                    crc32_read_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                    if (crc32_write_res!=crc32_read_res)
                    {
                        yc_log("flash write error");
                    }
                    /* 写过一次flash之后，地址递增1次 */
                    ota_config.writedLen +=OTA_BUFFER_LEN;
                }
                else
                {
                    ota_config.pos = 0;

                    /* 最后一包buffer数据写入flash */
                    ota_config.updateaddr = UpdateFirmwareAddr(Get_OffsetAddr());

                    FLASH_SectorErase(ota_config.updateaddr + ota_config.writedLen);
                    crc32_write_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                    /* 写qspi flash */
                    FLASH_Write(ota_config.updateaddr + ota_config.writedLen, OTA_BUFFER_LEN,ota_config.buffer);
                    memset(ota_config.buffer, 0xff, OTA_BUFFER_LEN);
                    FLASH_Read(ota_config.updateaddr + ota_config.writedLen, OTA_BUFFER_LEN,ota_config.buffer);
                    crc32_read_res=crc32_byte(ota_config.buffer,OTA_BUFFER_LEN);
                    if (crc32_write_res!=crc32_read_res)
                    {
                        yc_log("flash write error");
                    }
                }

                memset(ota_config.buffer, 0xff, OTA_BUFFER_LEN);
                BT_BleSlaveSendData(dhandle,yc_ota_tx_handle, (uint8_t*)&bledatapack, 3);
                break;

            case END_REQUEST:
                /* 结束写 qspi flash */
                ota_config.updateaddr = UpdateFirmwareAddr(Get_OffsetAddr());

                Sumlength = (bledata[3] | (bledata[4] << 8) | (bledata[5] << 16) | (bledata[6] << 24));
                CheckSum  = (bledata[7] | (bledata[8] << 8) | (bledata[9] << 16) | (bledata[10] << 24));

                /* 比较checksum 和sumlength 是否一致 */
                if((Sumlength == ota_config.Sumlength) && (CheckSum == ota_config.CheckSum))
                {
                    /* set ota app flag */
                    ota_config.SetAppFlag = true;
                    yc_log("check sum ok \n");
                }
                else
                {
                    yc_log("check sum fail \n");
                }

                memcpy(&endDataPack.Evt, &bledatapack.Evt, 3);
                endDataPack.updateFlashMode     =   NORMALOTA;
                endDataPack.length              =   Sumlength;
                endDataPack.checkSum            =   CheckSum;
                BT_BleSlaveSendData(dhandle,yc_ota_tx_handle, (uint8_t*)&endDataPack, 12);
                while((core_bt_status&0x1));
                break;

            default:
                break;
        }
    }
}
