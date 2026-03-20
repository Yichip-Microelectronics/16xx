
#include "yc_uart.h"
#include "yc_sadc.h"
#include "yc_gpio.h"
#include "yc_plic.h"
#include "yc_timer.h"

#define SADC_CONTINUOUS_MODE        0   /*SADC_CONTINUOUS_MODE==1时不推荐开启ADC中断,也不能与SADC_TIMER_TRIGGER_MODE同时开启*/
#define SADC_TIMER_TRIGGER_MODE     0   /*SADC_TIMER_TRIGGER_MODE==1时推荐开启ADC中断*/
#define SADC_WDT_EN                 0
#define SADC_IRQ                    0
#define TIMER_CH                    SADC_TIMER6
#define TIMER_CLKDIV                (7)
#define ADC_GPIO_CH                 SADC_CHANNEL_GPIO24
#define ADC_GPIO_PIN                GPIO_24

uint16_t regular_adc_value=0;
uint16_t current_ch=0;

/**
  * @brief  ADC_Configuration
  * @param  None
  * @retval None
  */
static void ADC_Configuration(void)
{
    SADC_InitStruct SADC_InitStruct;
    #if (SADC_CONTINUOUS_MODE==1)
    SADC_InitStruct.continuous_mode=ENABLE;
    #else
    SADC_InitStruct.continuous_mode=DISABLE;
    #endif
    SADC_InitStruct.auto_inject=DISABLE;
    SADC_InitStruct.etsrc=SADC_SOFTWARE;
    SADC_InitStruct.etsic=SADC_SOFTWARE;
    SADC_Init(&SADC_InitStruct);
    SADC_SetRegularChannel(1<<ADC_GPIO_CH);
}

static void ADC_GpioConfig()
{
    GPIO_Config(ADC_GPIO_PIN,GPIO_MODE_HI_Z);
    GPIO_AnalogEn(ADC_GPIO_PIN,ENABLE);
}

#if (SADC_WDT_EN==1)
static void ADC_WDT_Configuration(void)
{
    SADC_WdtStruct wdt;
    wdt.wdt_ren=ENABLE;
    wdt.wdt_ien=DISABLE;
    wdt.wdt_ch=ADC_GPIO_CH;
    wdt.wdt_sc=1;
    SADC_WdtConfig(&wdt,100,3000);
    SADC_SetIrq(SADC_WDT,ENABLE);
}
#endif

#if (SADC_TIMER_TRIGGER_MODE==1)
static void TIM_CoreParaConfig(TIM_PortEnum timx, uint8_t clkDiv, uint32_t period)
{
    _ASSERT(period<=0xffff);
    TIM_InitStruct TIM_InitStruct;
    TIM_InitStruct.timx = timx;
    TIM_InitStruct.period = period;
    TIM_InitStruct.clockdiv = clkDiv;
    TIM_InitStruct.autoLoadEn=ENABLE;
    /*timer core init*/
    TIM_Init(&TIM_InitStruct);
    /*timer enable*/
    TIM_Cmd(timx, ENABLE);
}
#endif

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("SADC demo");
    ADC_GpioConfig();
    ADC_Configuration();
    #if (SADC_WDT_EN==1)
    ADC_WDT_Configuration();
    #endif
    #if (SADC_TIMER_TRIGGER_MODE==1)
    SADC_RegularTrigsel(TIMER_CH);
    TIM_CoreParaConfig((TIM_PortEnum)TIMER_CH, TIMER_CLKDIV, TIMER_1MS(TIMER_CLKDIV) * 100);
    #endif
    #if (SADC_TIMER_TRIGGER_MODE==1 || SADC_IRQ==1 || SADC_WDT_EN==1)
    #if (SADC_TIMER_TRIGGER_MODE==1 || SADC_IRQ==1)
    SADC_SetIrq(SADC_EORC,ENABLE);
    #endif
    PLIC_SetPriority(SARADC_IRQn,2);
    PLIC_EnableIRQ(SARADC_IRQn);
    #endif
    #if (SADC_CONTINUOUS_MODE==1)
    SADC_RegularTrigger();
    #endif
    current_ch=ADC_GPIO_CH;
    while(1)
    {
        #if (SADC_CONTINUOUS_MODE==1)
        regular_adc_value=SADC_CalculateVoltage(ADC_GPIO_CH,SADC_GetRegularConversionValue());
        yc_log("GPIO Voltage=%d",regular_adc_value);
        #else
        #if (SADC_TIMER_TRIGGER_MODE==0)
        if(current_ch==ADC_GPIO_CH)
        {
            current_ch=SADC_CHANNEL_VBAT;
        }
        else
        {
            current_ch=ADC_GPIO_CH;
        }
        SADC_SetRegularChannel(1<<current_ch);
        SADC_RegularTrigger();
        #if (SADC_IRQ==0)
        regular_adc_value=SADC_CalculateVoltage(current_ch,SADC_GetRegularConversionValue());
        if(current_ch==ADC_GPIO_CH)
        {
            yc_log("gpio voltage=%d",regular_adc_value);
        }
        else
        {
            yc_log("vbat Voltage=%d",regular_adc_value);
        }
        #endif
        #endif
        #endif
        delay_ms(500);
    }
}

#if (SADC_TIMER_TRIGGER_MODE==1 || SADC_IRQ==1 || SADC_WDT_EN==1)
void SADC_IRQHandler(void)
{
    uint8_t irq_status=SADC_GetIrqStatus();
    regular_adc_value=SADC_GetRegularConversionValue();
    if(irq_status&SADC_WDT)
    {
        yc_log("adc wdt irq[regular_adc_value=%d]",regular_adc_value);
    }
    if(irq_status&SADC_EORC)
    {
        if(current_ch==ADC_GPIO_CH)
        {
            yc_log("gpio voltage=%d",SADC_CalculateVoltage(current_ch,regular_adc_value));
        }
        else
        {
            yc_log("vbat Voltage=%d",SADC_CalculateVoltage(current_ch,regular_adc_value));
        }
    }
    SADC_ClearIrq(irq_status);
}
#endif
