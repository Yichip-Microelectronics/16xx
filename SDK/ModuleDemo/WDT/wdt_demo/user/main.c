#include "yc_log.h"
#include "yc_wdt.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"
#include "yc_vector.h"

#define VECTOR_IRQ_ENBALE       0
#define WDT_MODE                INTR_MODE
#define WDT_COUNTER             WDT_TIMER_1_5S

void WDt_Configuration(void)
{
    WDT_InitStruct initstruct;
    initstruct.mode = WDT_MODE;
    initstruct.reload = WDT_COUNTER;
    WDT_Init(&initstruct);
}

volatile int wdt_reload_cnt=0;
int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("wdt demo");
    if(SYSCTRL_GetSysEvent()&(SYS_EVENT_WDT_RESET))
    {
        yc_log("wdt reset");
        SYSCTRL_ClearSysEvent(SYS_EVENT_WDT_RESET);
    }
    WDt_Configuration();
    if(WDT_MODE==INTR_MODE)
    {
        #if VECTOR_IRQ_ENBALE
        VECTOR_EnableIRQ(VECTOR_WDT_IRQn);
        #else
        PLIC_SetPriority(WDT_IRQn,1);
        PLIC_EnableIRQ(WDT_IRQn);
        #endif
    }

    while (1)
    {
        if(wdt_reload_cnt<3)
        {
            delay_ms(1000);
            WDT_SetReload(WDT_COUNTER);
            yc_log("wdt reload");
            wdt_reload_cnt++;
        }
    }
}
#if VECTOR_IRQ_ENBALE
_interrupt void VECTOR_WDT_IRQ(void)
#else
void WDT_IRQHandler(void)
#endif
{
    yc_log("WDT interrupt");
    wdt_reload_cnt = 0;
    WDT_ClearIrqSatus();
}
