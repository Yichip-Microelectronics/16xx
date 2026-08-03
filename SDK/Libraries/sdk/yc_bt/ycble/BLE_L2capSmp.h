#ifndef __BLE_L2CAP_SMP_H_
#define __BLE_L2CAP_SMP_H_
#include "le.h"
typedef struct {
	uint8_t io_cap;
	uint8_t oob_flag;
	uint8_t auth_req;
	uint8_t keysize;
	uint8_t init_kdist;
	uint8_t res_kdist;
} smp_cap;

// auth_req 中的 SC 标志位 (bit 3)
#define AUTH_REQ_SC_FLAG    0x08
// Secure Connection 状态定义
#define SC_STATE_IDLE              0
#define SC_STATE_WAIT_PK           1   // 等待对端公钥
#define SC_STATE_WAIT_CONFIRM      2   // 等待确认值(NC/JW)
#define SC_STATE_WAIT_RANDOM       3   // 等待随机数
#define SC_STATE_WAIT_DHKEY_CHECK  4   // 等待DHKey Check
#define SC_STATE_COMPLETE          5
#define SC_STATE_WAIT_PASSKEY_INPUT 6  // 等待passkey输入
#define SC_STATE_WAIT_SEND_CONFIRM  7  // 等待发送确认值

// Keypress Notification类型 (Core Spec Vol. 3 Part H Section 3.6.7)
#define KEYPRESS_ENTRY_STARTED      0x00
#define KEYPRESS_DIGIT_ENTERED      0x01
#define KEYPRESS_DIGIT_ERASED       0x02
#define KEYPRESS_CLEARED            0x03
#define KEYPRESS_ENTRY_COMPLETED    0x04

// Secure Connection 配对方法
#define SC_METHOD_JW               0   // Just Works
#define SC_METHOD_NC               1   // Numeric Comparison
#define SC_METHOD_PK_INIT_INPUT    2   // Passkey Entry, Initiator输入
#define SC_METHOD_PK_RESP_INPUT    3   // Passkey Entry, Responder输入
#define SC_METHOD_PK_BOTH_INPUT    4   // Passkey Entry, 双方输入

// Numeric Comparison 确认回调类型
// 返回: 1=确认配对, 0=拒绝配对
typedef int (*sc_nc_confirm_cb)(uint32_t passkey);

// Secure Connection 上下文结构体（配对时动态分配）
typedef struct {
    uint8_t local_pk[64];      // 本地公钥 (X||Y, 各32字节)
    uint8_t local_sk[32];      // 本地私钥
    uint8_t remote_pk[64];     // 对端公钥
    uint8_t dhkey[32];         // ECDH共享密钥
    uint8_t local_rand[16];    // 本地随机数(Na/Nb)
    uint8_t remote_rand[16];   // 对端随机数
    uint8_t peer_confirm[16];  // 对端确认值
    uint8_t sc_state;          // SC状态机状态
    uint8_t sc_method;         // SC配对方法
    uint32_t nc_value;      // Numeric Comparison显示值
    uint8_t passkey_round;     // Passkey Entry当前轮次(0-19)
    uint32_t passkey;       // Passkey值（SC模式需要20轮）
} smp_sc_ctx;


// Passkey 上下文结构体（配对时动态分配）
typedef struct {
    uint32_t passkey;      // 6位 passkey (0-999999)
    uint8_t passkey_input[7]; // 用户输入的 passkey (6位数字 + '\0')
    uint8_t input_idx;        // 当前输入位置
    uint8_t passkey_state;    // 0: 未开始, 1: 等待输入, 2: 输入完成
    uint8_t is_display;       // 1: Display 角色, 0: Keyboard 角色
} smp_passkey_ctx;




void le_parse_smp(dev_handle_t dhandle, byte *rxp);
void le_master_start_pairing(dev_handle_t dhandle);
void le_send_smp_pairing_failed(dev_handle_t dhandle, byte reason);
void aes_result(uint8_t *ciphertext);
void function_c1(uint8_t *key, uint8_t *r, smp_cap *preq, smp_cap *pres, uint8_t iat, uint8_t rat, uint8_t *ia, uint8_t *ra, uint8_t *out);
void le_send_smp(dev_handle_t dhandle, uint8_t op, word len, byte *p);
void aes_encrypt(uint8_t aes_ctrl, uint8_t *key, uint8_t *plaintext, uint8_t len);
void smp_passkey_input_done(dev_handle_t dhandle);
#endif // __BLE_L2CAP_SMP_H_