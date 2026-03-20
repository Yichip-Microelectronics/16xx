#ifndef __RF_H__
#define __RF_H__
#include "hw.h"
#include "types.h"


#ifdef RF_MODE_FPGA
    #define PLL_SETUP		120
    #define TX_SETUP		3
    #define RX_SETUP		70					// 80=39us ahead of packet
    #define SIFS_TX			120					//  SIFS before TX
    #define SIFS_RX			146					// SIFS before RX
    #define RX_WINDOW		159
    #define RX_SYNC			50
    #define RX_SYNC_LR		422
    #define TX_END			50
#else
    #define PLL_SETUP		50
    #define TX_SETUP		3                   // delay after TXON
    #define RX_SETUP		70					// 80=39us ahead of packet
    #define SIFS_TX			121					//  SIFS before TX
    #define SIFS_RX			120					//  SIFS before RX
    #define RX_WINDOW		159
    #define RX_PREP         (24+19)             // set rx_freq + rxon
    #define TX_PREP         (24+12+TX_SETUP)    // set tx_freq + txon + TX_SETUP
    #define RX_SYNC			30
    #define RX_SYNC_LR		422
    #define TX_END			50
    #define XTAL_CNT        0x35
    #define EXEN_CNT        0x08
    #define XTAL_ISEL       0x0f
 // #define SLEEP_AHEAD     (1138)       //96M  680us(dvdd2wake) +  92us(idle) + 182+60us(rx_slave)
    #define SLEEP_AHEAD     (2180)       //48M  680us(dvdd2wake) +  80us(idle) + 216+60us(rx_slave)
 // #define SLEEP_AHEAD     (1200)       //24M  676us(dvdd2wake) + 140us(idle) + 268+60us(rx_slave)
#endif

// #define ANCHOR_AHEAD	(PLL_SETUP + RX_WINDOW + 228 + RX_PREP) //96M
#define ANCHOR_AHEAD(m,x)	((m)?(PLL_SETUP + 150 + TX_PREP):(PLL_SETUP + ((x)>>1) + 150 + RX_PREP)) //48M
// #define ANCHOR_AHEAD	(PLL_SETUP + RX_WINDOW + 300 + RX_PREP) //24M

#define BLEMODE_1M		1
#define BLEMODE_2M		2
#define BLEMODE_S2		4
#define BLEMODE_S8		12
#define BLEMODE_500K    16
#define BLEMODE_FEC1    32
#define BLEMODE_CONN	128

#define TYPE_RX             0
#define TYPE_TX             1
#define TYPE_NORX           2
#define TYPE_SIFS           4
#define TYPE_SIFS_MODE      8
#define TYPE_ANCHOR         16
#define TYPE_TX_RX          32
#define TYPE_NOTX           64


// #define RF_TEST_MODE

#define RF_DEBUG_IO         (7)
#define RF_DEBUG_LINE_H     GPIO_CONFIG(RF_DEBUG_IO)  = 0x3f
#define RF_DEBUG_LINE_L     GPIO_CONFIG(RF_DEBUG_IO)  = 0x3e
#define RF_DEBUG_LINE(x)    RF_DEBUG_LINE_##x

#if !defined(RF_TEST_MODE)
    #define RF_DBGIO_RX         (0x8306)//(0x8000) // (0x8306)
    #define RF_DBGIO_TX         (0x8307)//(0x8000) // (0x8307)

    #define RF_DBG_IRQ          GPIO_CONFIG(36) = GPIO_CONFIG(36) == 0x3f ? 0x3e:0x3F;
    #define TIM_DBG_IRQ         GPIO_CONFIG(7) = GPIO_CONFIG(7) == 0x3f ? 0x3e:0x3F;
    #define ENTER_LPM           GPIO_CONFIG(37) = GPIO_CONFIG(37) == 0x3f ? 0x3e:0x3F;
    #define MISS_RXPKT          GPIO_CONFIG(37) = GPIO_CONFIG(37) == 0x3f ? 0x3e:0x3F;
#else
    #define RF_DBGIO_RX         (0x8000)
    #define RF_DBGIO_TX         (0x8000)

    #define RF_DBG_IRQ
    #define TIM_DBG_IRQ
    #define ENTER_LPM
	#define MISS_RXPKT
#endif



#define bdfunc  __attribute__((section (".__bdfunc__"))) int
#define bdpara  __attribute__((section (".__bdpara__")))

typedef struct _rf_para
{
    /*para*/
    int8_t txpower;                //0
    uint8_t last_ch;
    uint8_t freq_index;
    uint8_t curr_mode;
    uint8_t fcomp_div; // 24M/16M   //4
    uint8_t enc;
    uint8_t mic_err;
    bool dtm_mode:1;
    bool ft_mode:1;
    bool g24_mode:1;
    bool fec_mode:1;
    uint8_t mode_res0:4;

    /*lpm save para*/
    uint32_t bt_timer;              //8
    uint8_t  bt_timer_en;           //c
    uint8_t  bt_seq;
    uint8_t res[2];
    uint32_t bt_sleep_rtccnt;       //10
    uint32_t bt_sleep_syscnt;       //14
    float bt_rtccali;               //18
    uint32_t last_anchor;           //1c

    /*btdma*/
    uint8_t *txptr;                 //20
    uint8_t *rxptr;                 //24
    uint8_t *txpkt_head;            //28
    uint8_t *rxpkt_head;            //2c
    uint8_t *rx_rptr;               //30

    uint8_t txaccess[6];            //34
    uint8_t lrpreamble[10];         //3a
    uint8_t lrterm;                 //44
    uint32_t crc_init;              //48
    uint32_t rx_starttime;          //4c
    uint32_t tx_starttime;          //50
    uint32_t tmp; //btdma temp val  //54

    int btdma_stack[16];            //58
    int *btdesc_ptr, *txdesc, *rxdesc;//98
    /*calc*/
    uint16_t ghpc_table[12];        //a4
    uint8_t  aac_res_table[12];     //bc
    bool aac_ghpc_generated;        //c8
    uint8_t  rx_tiai_dcoc_val;      //c9
    uint8_t  rx_tiaq_dcoc_val;      //ca
    uint8_t res1[9];                //cb

    //connect
    uint32_t anchor;                //d8

    //debug
    uint32_t rssi;                  //dc
}rf_para_cb;



extern int *btdesc_ptr;
extern int *btdesc_start;
extern int btdesc_len;
extern int scan_rsp[9];
extern int tx_reply[10];
extern int rx_prep[10];
extern int rx_cmp[16];
extern int txsifs[16];
extern int rxsifs[22];
extern int set_txfreq[8];
extern int set_rxfreq[8];
extern int txon[19];
extern int rxon[23];
extern int txenc[35];
extern int check_mic[10];

void rftx_set_txptr(byte *pkt);
byte* rftx_get_txptr(void);
void rftx_setup(byte ch,byte mode, uint access, uint crcinit);
void rftx_packet(byte ch,byte mode, uint access, uint crcinit, int flag, byte len, byte *pkt);
void rfrx_prepare(byte ch, byte mode, int access, int crcinit, int wait_time,byte *pkt);
void start_btdma(int flag);
void wait_dma_done(void);
void rf_nowhite(bool md);
void rf_callbtdma(int *bt);
void rf_txpower(char db);
byte *rxok();
void rf_set_anchor(uint32_t t);
char rf_lpm_enter(uint32_t anchor);
void rf_lpm_exit(void);
void phy_init(void);
void rf_set_enc(bool mode);
uint8_t *rf_get_para(void);


#define rf_para ((rf_para_cb*)rf_get_para())
#endif
