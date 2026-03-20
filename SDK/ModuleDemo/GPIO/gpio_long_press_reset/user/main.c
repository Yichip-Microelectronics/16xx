#include "yc_log.h"
#include "yc_uart.h"
#include "yc_gpio.h"

#define LONG_PRESS_TIMEOUT LONG_PRESS_TIMEOUT_8S

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("gpio_LongPressReset demo");
    yc_log("GPIO_X press %ds Reset",8*(8-LONG_PRESS_TIMEOUT));
#ifdef GPIO_Long_Press_Reset_1
    GPIO_Config(GPIO_Long_Press_Reset_1,GPIO_MODE_PULL_UP);
    GPIO_SetLongPressReset(GPIO_20_LONG_PRESS_RESET,ENABLE);
    GPIO_SetLongPressResetTimeout(LONG_PRESS_TIMEOUT);
    yc_log("GPIO_%d init end\r\n",GPIO_Long_Press_Reset_1);
#endif

#ifdef GPIO_Long_Press_Reset_2
    GPIO_Config(GPIO_Long_Press_Reset_2,GPIO_MODE_PULL_UP);
    GPIO_SetLongPressReset(GPIO_26_LONG_PRESS_RESET,ENABLE);
    GPIO_SetLongPressResetTimeout(LONG_PRESS_TIMEOUT);
    yc_log("GPIO_%d init end\r\n",GPIO_Long_Press_Reset_2);
#endif

#ifdef GPIO_Long_Press_Reset_3
    GPIO_Config(GPIO_Long_Press_Reset_3,GPIO_MODE_PULL_UP);
    GPIO_SetLongPressReset(GPIO_31_LONG_PRESS_RESET,ENABLE);
    GPIO_SetLongPressResetTimeout(LONG_PRESS_TIMEOUT);
    yc_log("GPIO_%d init end\r\n",GPIO_Long_Press_Reset_3);
#endif
    while (1)
    {
        
    }
}

