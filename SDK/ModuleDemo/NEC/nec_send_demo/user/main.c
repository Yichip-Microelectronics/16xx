#include "board_config.h"
#include "yc_nec.h"
#include "yc_timer.h"
#include "yc_plic.h"
#include "yc_systick.h"

#define NEC_MODE                        NEC_TX_CARRIER_HIGH
#define NEC_CLK_US                      (3)
#define NEC_USE_EXTERNAL_DRIVE          (0)                                                     /*0:IO直驱红外发射,1:IO+MOS管驱动红外发射*/
#define NEC_CARRIER_DUTY_FREQ           (38000)                                                 /*载波频率*/
#define NEC_SUB_FILL_DATA               (1)                                                     /*分包填充数据控制*/
#if (NEC_SUB_FILL_DATA)
#define NEC_DMA_BUF_SIZE                (36)
#else
#define NEC_DMA_BUF_SIZE                (72)
#endif

#define KEY_LONG_PRESS_TIME_MS          (300)                                                   /* 300ms */

#define NEC_DATA_BIT_NUM                (1+4*8)                                                 /* 引导码+数据*/
#define NEC_REPEAT_INTERVAL_TIME_MS     (110)                                                   /* 110ms */
#define NEC_BOOT_CODE_START_TIME        9000                                                    /* 9ms */
#define NEC_BOOT_CODE_STOP_TIME         4500                                                    /* 4.5ms */
#define NEC_DATA_START_TIME             560                                                     /* 560us */
#define NEC_DATA_BIT0_STOP_TIME         560                                                     /* 560us */
#define NEC_DATA_BIT1_STOP_TIME         1690                                                    /* 1690us */
#define NEC_REPEAT_START_TIME           9000                                                    /* 9ms */
#define NEC_REPEAT_STOP_TIME            2250                                                    /* 2250us */
#define NEC_START_ERROR_VALUE           500                                                     /* start time error value define */
#define NEC_STOP_ERROR_VALUE            300                                                     /* stop time error value define */
#define NEC_ERROR_VALUE                 100                                                     /* error value define */

typedef enum
{
    NEC_NONE_DATA,
    NEC_RECV_SUCC,
    NEC_RECV_REPEAT,
    NEC_ADDR_CHECK_ERR,
    NEC_DATA_CHECK_ERR
}NEC_StatusEnum;

/**
 * @brief nec data struct
 *
 */
typedef struct{
    uint8_t address;
    uint8_t addressInverse;
    uint8_t data;
    uint8_t dataInverse;
}NEC_DataStruct;

uint16_t nec_dma_buf[NEC_DMA_BUF_SIZE] = {0};
int nec_send_bit_index = 0;
extern uint32_t LPM_RETENTION SystickCount;
uint32_t start_tick = 0;
bool sended_key_value = false;/*每次按下是否发过键值包标识符,用于判断是否需要连发重复包*/
bool nec_long_press = false;/*是否长按标识符*/
bool nec_need_fill_data = false;
int nec_interval_time;

NEC_DataStruct nec_send_data =
{
    .address = 0x80,
    .addressInverse = 0x7f,
    .data = 0x01,
    .dataInverse = 0xfe,
};

void NEC_TxInit(void)
{
    NEC_InitStruct initStruct = {0};

    initStruct.bufSaddr =  (uint32_t)nec_dma_buf;
    initStruct.bufSize  = sizeof(nec_dma_buf);
    initStruct.clk      = NEC_CLK_US*1000000;
    initStruct.mode     = NEC_MODE;

    #if (NEC_USE_EXTERNAL_DRIVE)
    GPIO_Config(NEC_PIN, GPIO_MODE_HI_Z);/*非IO直驱时可配任意IO*/
    #else
    GPIO_DsConfig(GPIO_0,ENABLE);/*IO直驱时固定IO0~3,且许将IO驱动能力配为大驱动*/
    GPIO_DsConfig(GPIO_1,ENABLE);
    GPIO_DsConfig(GPIO_2,ENABLE);
    GPIO_DsConfig(GPIO_3,ENABLE);
    GPIO_Config(GPIO_0, GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_1, GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_2, GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_3, GPIO_MODE_HI_Z);
    #endif

    if((NEC_MODE==NEC_TX_CARRIER_HIGH) || (NEC_MODE==NEC_TX_CARRIER_LOW))
    {
        uint32_t carrier_high_cnt = (((NEC_CLK_US*1000000+(NEC_CARRIER_DUTY_FREQ-1))/NEC_CARRIER_DUTY_FREQ)/3);/*载波高定平cnt,按1/3占空比算*/
        uint32_t carrier_low_cnt = (((NEC_CLK_US*1000000+(NEC_CARRIER_DUTY_FREQ-1))/NEC_CARRIER_DUTY_FREQ)-carrier_high_cnt);/*载波低定平cnt*/
        _ASSERT(carrier_high_cnt<=0xffff && carrier_low_cnt<=0xffff);
        TIM_SetPwmPeriod(TIM7,carrier_low_cnt-1,carrier_high_cnt-1);/*载波固定使用TIM7进行配置*/
    }

    NEC_Init(&initStruct);
    #if (NEC_IRQ_ENABLE==1 || NEC_SUB_FILL_DATA==1)
    PLIC_SetPriority(NEC_IRQn,1);
    PLIC_EnableIRQ(NEC_IRQn);
    #endif
}

bool NEC_Send(NEC_DataStruct *txData,int* bitIndex)
{
    uint16_t nec_bit_data[2];
    int start_index = *bitIndex;

    if(start_index == 0)
    {
        NEC_Cmd(DISABLE);
        NEC_PtrInit();
        #if (NEC_USE_EXTERNAL_DRIVE)
        GPIO_Config(NEC_PIN, GPIO_MODE_NEC_SEND);/*非IO直驱时可配任意IO*/
        #else
        GPIO_Config(GPIO_0, GPIO_MODE_NEC_SEND);
        GPIO_Config(GPIO_1, GPIO_MODE_NEC_SEND);
        GPIO_Config(GPIO_2, GPIO_MODE_NEC_SEND);
        GPIO_Config(GPIO_3, GPIO_MODE_NEC_SEND);
        #endif
        _ASSERT((NEC_BOOT_CODE_START_TIME*NEC_CLK_US) < 0x7fff);/*确保最长的引导码编码后不会越界*/
        /*需要发送引导码*/
        nec_bit_data[0] = (NEC_BOOT_CODE_START_TIME*NEC_CLK_US)|(1<<15);
        nec_bit_data[1] = NEC_BOOT_CODE_STOP_TIME*NEC_CLK_US;
        (*bitIndex)++;
        NEC_WriteData(nec_bit_data,2);
    }
    for(;*bitIndex < NEC_DATA_BIT_NUM;)
    {
        if(NEC_DMA_BUF_SIZE-(NEC_GetDataCount()) <= (2+2))/*一个bit需要两个uint16_t,预留两uint16_t放截止符*/
        {
            #if (NEC_SUB_FILL_DATA == 0)
            yc_log("dma buf not enough");
            return false;
            #else
            if(start_index == 0)
            {
                NEC_Cmd(ENABLE);
                NEC_SetIrq(NEC_IRQ_DONE|NEC_IRQ_HALF_DONE,ENABLE);
            }
            nec_need_fill_data = true;
            return true;
            #endif
        }
        nec_bit_data[0] = (NEC_DATA_START_TIME*NEC_CLK_US)|(1<<15);
        if(((uint8_t*)txData)[(*bitIndex-1)/8]&(0x01<<((*bitIndex-1)%8)))
        {
            nec_bit_data[1] = NEC_DATA_BIT1_STOP_TIME*NEC_CLK_US;
        }
        else
        {
            nec_bit_data[1] = NEC_DATA_BIT0_STOP_TIME*NEC_CLK_US;
        }
        NEC_WriteData(nec_bit_data,2);
        (*bitIndex)++;
    }
    nec_bit_data[0] = (NEC_DATA_START_TIME*NEC_CLK_US)|(1<<15);/*添加截止符,模块启动一次发送后直到遇到数据0停止*/
    nec_bit_data[1] = 0;
    NEC_WriteData(nec_bit_data,2);
    if(start_index == 0)
    {
        NEC_Cmd(ENABLE);
        NEC_SetIrq(NEC_IRQ_DONE|NEC_IRQ_HALF_DONE,ENABLE);
    }
    nec_need_fill_data = false;
    return true;
}

bool NEC_SendRepeat(void)
{
    uint16_t nec_bit_data[4];
    if(NEC_DMA_BUF_SIZE-(NEC_GetDataCount()) <= 3)/*uint16_t count*/
    {
        return false;
    }
    NEC_Cmd(DISABLE);
    NEC_PtrInit();
    nec_bit_data[0] = (NEC_REPEAT_START_TIME*NEC_CLK_US)|(1<<15);
    nec_bit_data[1] = NEC_REPEAT_STOP_TIME*NEC_CLK_US;
    nec_bit_data[2] = (NEC_DATA_START_TIME*NEC_CLK_US)|(1<<15);
    nec_bit_data[3] = 0;/*添加截止符,模块启动一次发送后直到遇到数据0停止*/
    NEC_WriteData(nec_bit_data,sizeof(nec_bit_data)/sizeof(uint16_t));
    #if (NEC_USE_EXTERNAL_DRIVE)
    GPIO_Config(NEC_PIN, GPIO_MODE_NEC_SEND);/*非IO直驱时可配任意IO*/
    #else
    GPIO_Config(GPIO_0, GPIO_MODE_NEC_SEND);
    GPIO_Config(GPIO_1, GPIO_MODE_NEC_SEND);
    GPIO_Config(GPIO_2, GPIO_MODE_NEC_SEND);
    GPIO_Config(GPIO_3, GPIO_MODE_NEC_SEND);
    #endif
    NEC_Cmd(ENABLE);
    NEC_SetIrq(NEC_IRQ_DONE|NEC_IRQ_HALF_DONE,ENABLE);
    return true;
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    SYSTICK_Config(SYSTICK_ONE_MS*10);
    GPIO_Config(KEY1_PIN,GPIO_MODE_PULL_UP);
    yc_log("NEC send demo");
    NEC_TxInit();

    yc_log("1:short press s1 send key value");
    yc_log("2:long press s1 send key value and Repeat");

    while(1)
    {
        if(GPIO_ReadDataBit(KEY1_PIN)==0)
        {
            if(sended_key_value==false)
            {
                sended_key_value = true;
                nec_long_press = false;
                nec_send_bit_index=0;
                start_tick = SYSTICK_GetTick();
                if(NEC_Send(&nec_send_data,&nec_send_bit_index))
                {
                    yc_log("nec send code ok");
                }
                else
                {
                    yc_log("nec send code fail");
                }
            }
            else
            {
                if(nec_long_press == false)
                {
                    nec_interval_time = KEY_LONG_PRESS_TIME_MS;
                }
                else
                {
                    nec_interval_time = NEC_REPEAT_INTERVAL_TIME_MS;
                }
                if(SYSTICK_GetRelativeTime(start_tick)>=nec_interval_time)
                {
                    nec_long_press = true;
                    start_tick = SYSTICK_GetTick();
                    if(NEC_SendRepeat())
                    {
                        yc_log("nec send Repeat ok");
                    }
                    else
                    {
                        yc_log("nec send Repeat fail");
                    }
                }
            }
        }
        else
        {
            sended_key_value = false;
            nec_long_press = false;
        }
    }
}

_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    SystickCount++;
    SYSTICK_ClearIrq();
}

void NEC_IRQHandler(void)
{
    uint8_t necStatus=NEC_GeIrqStatus();
    if(necStatus&(NEC_IRQ_DONE))
    {
        #if (NEC_USE_EXTERNAL_DRIVE)
        GPIO_Config(NEC_PIN, GPIO_MODE_HI_Z);/*非IO直驱时可配任意IO*/
        #else
        GPIO_Config(GPIO_0, GPIO_MODE_HI_Z);
        GPIO_Config(GPIO_1, GPIO_MODE_HI_Z);
        GPIO_Config(GPIO_2, GPIO_MODE_HI_Z);
        GPIO_Config(GPIO_3, GPIO_MODE_HI_Z);
        #endif
    }
    if(nec_need_fill_data == true)
    {
        NEC_Send(&nec_send_data,&nec_send_bit_index);
    }
    NEC_ClearIrq(necStatus);
}
