#ifndef _BT_REG_H_
#define _BT_REG_H_

#define	BT_EVT_START                     	        0x00
#define HCI_EVENT_LE_L2CAP_RX_PARSE                 0x01
#define	HCI_EVENT_LE_CONN_REP                     	0x02
#define HCI_EVENT_SLEEP_INTERVAL                    0x03
#define	HCI_EVENT_LE_DIS_REP                  	    0x05
#define HCI_EVENT_LE_DATA_REP	                    0x08
#define HCI_EVENT_STANDBY_REP	                    0x09
#define HCI_EVENT_STATUS_RES	                    0x0a
#define HCI_EVENT_GKEY	                            0x0e
#define HCI_EVENT_INVALID_PACKET	                0x0F
#define HCI_EVENT_GET_PASSKEY	                    0x10
#define HCI_EVENT_LE_TK	                            0x11
#define HCI_EVENT_REMOTE_MTU	                    0x12
#define HCI_EVENT_LE_PAIRING_STATE	                0x14
#define HCI_EVENT_LE_ENCRYPTION_START	            0X15
#define HCI_EVENT_LE_CONNECTION_UPDATE	            0x17
#define HCI_EVENT_LE_HANDLE_VALUE_CONFIRMATION      0x18
#define HCI_EVENT_LE_GKEY	                        0x1d
#define HCI_EVENT_UUID_HANDLE	                    0x29
#define HCI_BLE_UPDATE_PHY_EVENT                    0x40

#define HCI_EVENT_MASTER_SCAN_ADV                   0x50
#define HCI_EVENT_MASTER_SCAN_RSP                   0x51
#define HCI_EVENT_ENC_DONE                        	0x52
#define HCI_EVENT_MASTER_SERVICE_DISCOVER_DONE      0x55
#endif // _BT_REG_H_
