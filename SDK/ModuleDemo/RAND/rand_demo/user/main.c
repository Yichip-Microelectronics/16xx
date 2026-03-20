
#include "yc_rand.h"
#include "yc_timer.h"
#include "yc_plic.h"
#include "yc_systick.h"

#define RANDOM_DATA_SIZE        (10)

uint16_t random_data[RANDOM_DATA_SIZE]={0};
extern uint32_t LPM_RETENTION SystickCount;

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    SYSTICK_Config(SYSTICK_ONE_MS*10);
    yc_log("rand_demo");
    yc_log("press s1 gen random data");
    GPIO_Config(KEY1_PIN,GPIO_MODE_PULL_UP);

    while (1)
    {
        if(GPIO_ReadDataBit(KEY1_PIN) == 0)
        {
            YC_RandInit(SystickCount);
            for(int i=0;i<RANDOM_DATA_SIZE;i++)
            {
                random_data[i] = YC_RandGet();
            }
            yc_log("random data:");
            for(int i=0;i<RANDOM_DATA_SIZE;i++)
            {
                yc_log("%d",random_data[i]);
            }
            delay_ms(300);
        }
    }
}

_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    SystickCount++;
    SYSTICK_ClearIrq();
}
