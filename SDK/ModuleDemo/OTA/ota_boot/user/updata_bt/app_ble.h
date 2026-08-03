#ifndef _APP_BLE_H
#define _APP_BLE_H
#include "le.h"
#include "yc_bt.h"
#include "yc_user.h"
#include "yc_ipc.h"
#include "yc_log.h"
#include "yc_systick.h"
#include "yc_lpm.h"
#include "yc_rtc.h"
#include "BLE_L2capATT_List.h"
#include "yc_qspi.h"

#define ENABLE_UART_LOG             0
#define MTU_MAX                     150

#define BLE_CMD_NOTIFY_CHAR_HANDLE          0x11

void Ble_ParamInit(void);

#endif

