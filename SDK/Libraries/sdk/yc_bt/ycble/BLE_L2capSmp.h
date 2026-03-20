#ifndef __BLE_L2CAP_SMP_H_
#define __BLE_L2CAP_SMP_H_
typedef struct {
	uint8_t io_cap;
	uint8_t oob_flag;
	uint8_t auth_req;
	uint8_t keysize;
	uint8_t init_kdist;
	uint8_t res_kdist;
} smp_cap;


// Passkey 上下文结构体（配对时动态分配）
typedef struct {
    uint32_t passkey;      // 6位 passkey (0-999999)
    byte passkey_input[7]; // 用户输入的 passkey (6位数字 + '\0')
    byte input_idx;        // 当前输入位置
    byte passkey_state;    // 0: 未开始, 1: 等待输入, 2: 输入完成
    byte is_display;       // 1: Display 角色, 0: Keyboard 角色
} smp_passkey_ctx;



void le_parse_smp(byte *rxp);
void le_master_start_pairing(void);

void function_c1(byte *key, byte *r, smp_cap *preq, smp_cap *pres, byte iat, byte rat, byte *ia, byte *ra, byte *out);
void le_send_smp(byte op, word len, byte *p);
#endif // __BLE_L2CAP_SMP_H_