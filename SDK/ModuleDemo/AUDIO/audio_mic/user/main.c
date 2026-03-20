/*
 * Copyright (c) 2006-2020, YICHIP Development Team
 * @file     main.c
 * @brief    audio adc function tests.
 *
 * Change Logs:
 * Date           Author             Version        Notes
 * 2024-05-27     qin.su             V1.0.0         the first version
 */

/* Includes ------------------------------------------------------------------*/
#include "yc_uart.h"
#include "yc_audio.h"
#include "board_config.h"
#include "yc_vector.h"
#include "yc_plic.h"

#define SAMPLE_RATE             SAMPLE_RATE_8K
#define MIC_MODE                MIC_MODE_ANA
#define MIC_DATA_TO_UART        (0)

uint8_t mic_dma_buf[AUDIO_DATA_MS(200,SAMPLE_RATE)] = {0};
uint8_t read_buf[AUDIO_DATA_MS(50,SAMPLE_RATE)] = {0};
void DAC_GpioInit(void)
{
    GPIO_Config(GPIO_DAC_OUT_P_PIN, GPIO_MODE_DAC_OUT);
    GPIO_Config(GPIO_DAC_OUT_N_PIN, GPIO_MODE_DAC_OUT);
}
int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    #if (MIC_DATA_TO_UART==0)
    yc_log("audio mic demo");
    #endif

    AUDIO_AdcInitStruct adcInitStruct;

    adcInitStruct.sampleRate=SAMPLE_RATE;
    if(MIC_MODE==MIC_MODE_PDM)
    {
        GPIO_Config(GPIO_PDM_CLK_PIN,GPIO_MODE_MICCLK_PDM);
        GPIO_Config(GPIO_PDM_DATA_PIN,GPIO_MODE_MICDAT_PDM);
        adcInitStruct.mode=MIC_MODE_PDM;
        adcInitStruct.pdmSampleEdge=MIC_RISING_SAMPLE;
    }
    else
    {
        GPIO_Config(GPIO_17,GPIO_MODE_HI_Z);
        GPIO_Config(GPIO_18,GPIO_MODE_HI_Z);
        GPIO_Config(GPIO_19,GPIO_MODE_HI_Z);
        adcInitStruct.mode=MIC_MODE_ANA;
    }

    AUDIO_AdcInit(&adcInitStruct);
    AUDIO_AdcDmaBufInit(mic_dma_buf,sizeof(mic_dma_buf));
    AUDIO_AdcSetVol(0x1000);
    AUDIO_AdcCmd(ENABLE);
    #if (MIC_DATA_TO_UART==0)
    DAC_GpioInit();
    AUDIO_DacInit(SAMPLE_RATE);
    AUDIO_DacDmaBufInit(mic_dma_buf, sizeof(mic_dma_buf));
    AUDIO_DacSetVol(0);
    AUDIO_DacCmd(ENABLE);
    #endif

    while(1)
    {
        #if (MIC_DATA_TO_UART==1)
        if(AUDIO_AdcGetDataCount() >= sizeof(read_buf))
        {
            AUDIO_AdcGetData(read_buf,sizeof(read_buf));
            UART_SendBuff(PRINT_PORT,read_buf,sizeof(read_buf));
        }
        #endif
    }
}
