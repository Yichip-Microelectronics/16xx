#include "yc_uart.h"
#include "yc_audio.h"
#include "board_config.h"
#include "yc_plic.h"
#include "wav.h"
// #include "16k.h"

#define SAMPLING_RATE               SAMPLE_RATE_8K
#define DAC_DMA_MODE                DAC_MODE_CYCLE

#define DAC_STATUS_PLAY_START       0
#define DAC_STATUS_HALF_SIZE_IRQ    1
#define DAC_STATUS_BUF_END_IRQ      2
#define DAC_STATUS_PLAY_END         3

#define DAC_PACKET_LEN      AUDIO_DATA_MS(40,SAMPLING_RATE)

#if (DAC_DMA_MODE == DAC_MODE_CYCLE)
uint8_t dac_dma_buf[AUDIO_DATA_MS(80,SAMPLING_RATE)] = {0};
uint32_t played_len = 0;
uint32_t packet_len = 0;
#endif
volatile uint8_t dac_status = DAC_STATUS_PLAY_START;

void DAC_GpioInit(void)
{
    GPIO_Config(GPIO_DAC_OUT_P_PIN, GPIO_MODE_DAC_OUT);
    GPIO_Config(GPIO_DAC_OUT_N_PIN, GPIO_MODE_DAC_OUT);
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("audio dac demo");

    DAC_GpioInit();

    AUDIO_DacInit(SAMPLING_RATE);
    AUDIO_DacSetMode(DAC_DMA_MODE);
    AUDIO_DacSetVol(0);
    #if (DAC_DMA_MODE == DAC_MODE_CYCLE)
    memset(dac_dma_buf, 0, sizeof(dac_dma_buf));
    AUDIO_DacDmaBufInit(dac_dma_buf, sizeof(dac_dma_buf));
    #endif
    PLIC_SetPriority(GDAC_IRQn, 1);
    PLIC_EnableIRQ(GDAC_IRQn);

    while(1)
    {
        if(dac_status == DAC_STATUS_PLAY_START)
        {
            #if (DAC_DMA_MODE == DAC_MODE_CYCLE)
            packet_len = sizeof(dac_dma_buf);/*默认声音文件大于dma buf*/
            AUDIO_DacPutData((uint8_t*)wav,packet_len);
            played_len = packet_len;
            AUDIO_DacSetIrqLevel(sizeof(dac_dma_buf)/2);
            dac_status = DAC_STATUS_HALF_SIZE_IRQ;
            #else
            AUDIO_DacDmaBufInit((uint8_t*)wav, sizeof(wav));
            AUDIO_DacSetIrqLevel(0);
            dac_status = DAC_STATUS_BUF_END_IRQ;
            #endif
            AUDIO_DacCmd(ENABLE);
            AUDIO_DacSetIrq(ENABLE);
            yc_log("play start");
        }
        else if(dac_status == DAC_STATUS_PLAY_END)
        {
            yc_log("play end");
            delay_ms(1000);
            dac_status = DAC_STATUS_PLAY_START;
        }
    }
}

void GDAC_IRQHandler(void)
{
    AUDIO_DacClearIrq();
    #if (DAC_DMA_MODE == DAC_MODE_CYCLE)
    if(played_len < sizeof(wav))
    {
        if(sizeof(wav)-played_len > sizeof(dac_dma_buf)/2)
        {
            packet_len = sizeof(dac_dma_buf)/2;
            AUDIO_DacPutData((uint8_t*)&wav[played_len],packet_len);
        }
        else
        {
            packet_len = sizeof(wav)-played_len;
            AUDIO_DacPutData((uint8_t*)&wav[played_len],packet_len);
            AUDIO_DacPutZero(sizeof(dac_dma_buf)/2 - packet_len);
        }
        played_len += packet_len;
        if(dac_status == DAC_STATUS_HALF_SIZE_IRQ)
        {
            AUDIO_DacSetIrqLevel(0);
            dac_status = DAC_STATUS_BUF_END_IRQ;
        }
        else
        {
            AUDIO_DacSetIrqLevel(sizeof(dac_dma_buf)/2);
            dac_status = DAC_STATUS_HALF_SIZE_IRQ;
        }
    }
    else
    {
        AUDIO_DacCmd(DISABLE);
        AUDIO_DacSetIrq(DISABLE);
        played_len = 0;
        dac_status = DAC_STATUS_PLAY_END;
    }
    #else
    AUDIO_DacCmd(DISABLE);
    AUDIO_DacSetIrq(DISABLE);
    dac_status = DAC_STATUS_PLAY_END;
    #endif
}
