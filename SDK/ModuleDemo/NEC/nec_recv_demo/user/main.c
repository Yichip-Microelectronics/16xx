#include "board_config.h"
#include "yc_nec.h"
#include "yc_timer.h"
#include "yc_plic.h"
#include "yc_systick.h"

#define NEC_CLK_US                      (1)
#define NEC_USE_EXTERNAL_DRIVE          (0)                                                     /*0:使用内部放大电路,1:使用外部放大电路*/
#define NEC_IRQ_ENABLE                  (0)                                                     /*0:不使能NEC中断,1:使能NEC中断*/
#define NEC_GAIN_LEVEL                  (GAIN_HIGHEST)                                          /* 红外内部放大电路增益*/
#define NEC_DMA_BUF_SIZE                (50)

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
extern uint32_t LPM_RETENTION SystickCount;
uint32_t start_tick = 0;
NEC_DataStruct nec_receive_data = {0};
uint8_t necByteIndex = 0;
uint8_t necBitIndex = 0;
uint8_t necByteData = 0;
#if (NEC_IRQ_ENABLE)
uint32_t last_items = 0;
#endif

void NEC_RxInit(void)
{
    NEC_InitStruct initStruct = {0};

    initStruct.bufSaddr =  (uint32_t)nec_dma_buf;
    initStruct.bufSize  = sizeof(nec_dma_buf);
    initStruct.clk      = NEC_CLK_US*1000000;
    initStruct.mode     = NEC_RX;

    NEC_Init(&initStruct);
    #if (NEC_USE_EXTERNAL_DRIVE)
    GPIO_Config(NEC_PIN, GPIO_MODE_NEC_REC);/*非IO直驱时可配任意IO*/
    NEC_RxSrcSel(NEC_FROM_GPIO);
    #else
    GPIO_AnalogEn(GPIO_0,ENABLE);
    GPIO_Config(GPIO_0, GPIO_MODE_NEC_REC);
    NEC_RxSrcSel(NEC_FROM_IR);
    NEC_SetRxGainLevel(NEC_GAIN_LEVEL);
    #endif
    #if (NEC_IRQ_ENABLE)
    NEC_SetIrq(NEC_IRQ_DONE|NEC_IRQ_HALF_DONE,ENABLE);
    PLIC_SetPriority(NEC_IRQn,1);
    PLIC_EnableIRQ(NEC_IRQn);
    #endif
    NEC_Cmd(ENABLE);
}

NEC_StatusEnum NEC_Receive(NEC_DataStruct *rxData)
{
    uint32_t nec_rev_data = 0;

    while(NEC_GetDataCount() >= 2)
    {
        nec_rev_data = NEC_ReadData();
        nec_rev_data = nec_rev_data / NEC_CLK_US;
        if(nec_rev_data > (NEC_DATA_BIT0_STOP_TIME - NEC_ERROR_VALUE))
        {
            if((nec_rev_data > (NEC_BOOT_CODE_STOP_TIME - NEC_ERROR_VALUE)) && (nec_rev_data < (NEC_BOOT_CODE_STOP_TIME + NEC_ERROR_VALUE)))
            {
                necByteIndex = 0;
                necBitIndex = 0;
            }
            if((nec_rev_data > (NEC_DATA_BIT0_STOP_TIME - NEC_ERROR_VALUE)) && (nec_rev_data < (NEC_DATA_BIT0_STOP_TIME + NEC_ERROR_VALUE)))
            {
                necByteData &= (~(1 << necBitIndex++));
            }
            else if((nec_rev_data > (NEC_DATA_BIT1_STOP_TIME - NEC_ERROR_VALUE)) && (nec_rev_data < (NEC_DATA_BIT1_STOP_TIME + NEC_ERROR_VALUE)))
            {
                necByteData |= (1 << necBitIndex++);
            }
            else if((nec_rev_data > (NEC_REPEAT_STOP_TIME - NEC_ERROR_VALUE)) && (nec_rev_data < (NEC_REPEAT_STOP_TIME + NEC_ERROR_VALUE)))
            {
                NEC_Cmd(DISABLE);
                NEC_PtrInit();
                NEC_Cmd(ENABLE);
                necByteIndex = 0;
                return NEC_RECV_REPEAT;
            }
            if(necBitIndex == 8)
            {
                ((uint8_t *)rxData)[necByteIndex++] = necByteData;
                necByteData = 0;
                necBitIndex = 0;
            }
            if(necByteIndex == 4)
            {
                if(rxData->address != ((~rxData->addressInverse) & 0x000000ff))
                {
                    memset((uint8_t *)&rxData, 0, sizeof(NEC_DataStruct));
                    NEC_Cmd(DISABLE);
                    NEC_PtrInit();
                    NEC_Cmd(ENABLE);
                    return NEC_ADDR_CHECK_ERR;
                }
                if(rxData->data != ((~rxData->dataInverse) & 0x000000ff))
                {
                    memset((uint8_t *)&rxData, 0, sizeof(NEC_DataStruct));
                    NEC_Cmd(DISABLE);
                    NEC_PtrInit();
                    NEC_Cmd(ENABLE);
                    return NEC_DATA_CHECK_ERR;
                }
                necByteIndex = 0;
                return NEC_RECV_SUCC;
            }
        }
    }
    return NEC_NONE_DATA;
}

void nec_recv_process(void)
{
    switch(NEC_Receive(&nec_receive_data))
    {
        case NEC_RECV_SUCC:
            yc_log_buf("nec recv data",(uint8_t *)&nec_receive_data,sizeof(NEC_DataStruct));
            break;
        case NEC_RECV_REPEAT:
            yc_log("nec recv repeat");
            break;
        case NEC_ADDR_CHECK_ERR:
            yc_log("nec addr check err");
            break;
        case NEC_DATA_CHECK_ERR:
            yc_log("nec data check err");
            break;
        default:
            break;
    }
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    #if (NEC_IRQ_ENABLE)
    SYSTICK_Config(SYSTICK_ONE_MS*10);
    #endif
    yc_log("NEC receive demo");
    NEC_RxInit();

    while(1)
    {
        #if (NEC_IRQ_ENABLE == 0)
        nec_recv_process();
        #endif
    }
}

#if (NEC_IRQ_ENABLE)
_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    SystickCount++;
    uint32_t temp_items = NEC_GetDataCount();
    if(temp_items != last_items)
    {
        last_items = temp_items;
        start_tick = SystickCount;
    }
    else
    {
        if(SYSTICK_GetRelativeTime(start_tick) > 20)
        {
            nec_recv_process();
        }
    }
    SYSTICK_ClearIrq();
}
#endif

#if (NEC_IRQ_ENABLE)
void NEC_IRQHandler(void)
{
    uint8_t necStatus=NEC_GetIrqStatus();
    nec_recv_process();
    NEC_ClearIrq(necStatus);
}
#endif
