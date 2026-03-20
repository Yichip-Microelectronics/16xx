/*
File Name    : yc_audio.h
Author       : Yichip
Version      : V1.0
Date         : 2024/05/27
Description  : yc audio define.
*/

#ifndef __YC_AUDIO_H__
#define __YC_AUDIO_H__

#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_DATA_MS(ms,sample_rate)                   ((ms)*((sample_rate)*(2)))
/* Aduio ADC */
#define ADC_FADE                                        ENABLE
#define ADC_HPF                                         ENABLE
#define ADC_DATA_TYPE                                   MIC_SIGNED_MODE
#define ADC_VOL_INIT                                    0x1000                      /* must less than ADC_VOL_MAX */
#define ADC_VOL_MAX                                     0x3fff                      /* max 14 bits = 0x3fff */
#define ADC_VOL_STEP                                    1                           /* 0x1~0xff, WARNING: set this to 0 and ADC_FADE ENABLE at same time will disable volume change */
/* Audio DAC */
#define    __ASSERT_DAC_VOL(vol)                        _ASSERT(vol<=7)
#define    __ASSERT_DAC_DMA_ADDR_ALIGNMENT(addr)        _ASSERT(((uint32_t)addr & 0x3) == 0)
#define DAC_MODE_SINGLE                                 (0)
#define DAC_MODE_CYCLE                                  (1)

/**
 * @brief audio sample rate enum
 *
 */
typedef enum
{
    SAMPLE_RATE_8K        =  8,
    SAMPLE_RATE_16K       = 16,
    SAMPLE_RATE_24K       = 24,
    SAMPLE_RATE_32K       = 32,
    SAMPLE_RATE_44P1K     = 44,
    SAMPLE_RATE_48K       = 48
} AUDIO_SampleRateEnum;
/**
 * @brief audio adc clock div enum
 *
 */
typedef enum
{
    MIC_CLK_DIV_NONE,
    MIC_CLK_DIV_2,
    MIC_CLK_DIV_3
} AUDIO_MicClkDivEnum;

/**
 * @brief audio adc clock div enum
 *
 */
typedef enum
{
    MIC_CLK_DIV_SEL_NONE,
    MIC_CLK_DIV_SEL_2,
    MIC_CLK_DIV_SEL_4
} AUDIO_MicClkDivSelEnum;

/**
 * @brief audio adc data mode enum
 *
 */
typedef enum
{
    MIC_SIGNED_MODE,
    MIC_UNSIGNED_MODE
} AUDIO_AdcDataModeEnum;
/**
 * @brief audio adc pdm data sample edge enum
 *
 */
typedef enum
{
    MIC_RISING_SAMPLE,
    MIC_FALLING_SAMPLE
} AUDIO_AdcSampleEdgeEnum;

/**
 * @brief audio adc(mic) mode enum
 *
 */
typedef enum
{
    MIC_MODE_ANA,
    MIC_MODE_PDM
} AUDIO_AdcModeEnum;

/**
 * @brief audio adc init struct
 *
 */
typedef struct
{
    AUDIO_SampleRateEnum    sampleRate;
    AUDIO_AdcSampleEdgeEnum pdmSampleEdge;
    AUDIO_AdcModeEnum       mode;
} AUDIO_AdcInitStruct;

void AUDIO_AdcInit(AUDIO_AdcInitStruct *adcInitStruct);
void AUDIO_AdcDmaBufInit(uint8_t *buf,uint16_t bufSize);
void AUDIO_AdcCmd(FunctionalState state);
void AUDIO_AdcSetVol(uint16_t vol);
void AUDIO_AdcSetMute(FunctionalState newState);
uint32_t AUDIO_AdcGetDataCount(void);
uint32_t AUDIO_AdcGetData(uint8_t *buf,uint32_t len);

void AUDIO_DacInit(AUDIO_SampleRateEnum sampleRate);
void AUDIO_DacSetVol(uint8_t vol);
void AUDIO_DacDmaBufInit(uint8_t *buf, uint32_t len);
uint32_t AUDIO_DacPutData(uint8_t *buf, uint32_t len);
uint32_t AUDIO_DacPutZero(uint32_t len);
void AUDIO_DacSetMode(uint8_t mode);
void AUDIO_DacSetMute(FunctionalState mute);
void AUDIO_DacCmd(FunctionalState state);
uint32_t AUDIO_DacGetAvailableBufSize(void);
void AUDIO_DacSetIrq(FunctionalState state);
void AUDIO_DacSetIrqLevel(uint16_t level);
void AUDIO_DacClearIrq(void);

#ifdef __cplusplus
}
#endif

#endif