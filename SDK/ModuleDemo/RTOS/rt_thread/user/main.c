
/* TODO Add any manufacture supplied header files can be included
here. */
#include <stdio.h>
#include "rtthread.h"
#include "yc_log.h"
#include "rtthread.h"
#include "rtdef.h"
#include "yc_timer.h"
#include "yc_plic.h"
#include "board_config.h"

/*
请注意，使用rt_thread系统需要在eide工程配置 汇编器附加选项 中添加预处理宏定义 -DRTOS_RTTHREAD
*/

void test_cp(void *parameter)
{
    uint32_t i = 0;
    while (1)
    {
        rt_kprintf("------------ test_cp -------------%d\n", i++);
        rt_thread_mdelay(200);
    }
}

rt_uint8_t test_stack[512];
struct rt_thread test_thread;
int main()
{
    int32_t i = 0;
    rt_thread_t tid = &test_thread;
    rt_err_t result;
    result = rt_thread_init(tid, "test", test_cp, RT_NULL,
                            test_stack, sizeof(test_stack), 6, 10);
    RT_ASSERT(result == RT_EOK);
    rt_thread_startup(tid);
    GPIO_Config(GPIO_RISING_FALL_IRQ_PIN, GPIO_MODE_PULL_UP);
    GPIO_SetIrqMode(GPIO_RISING_FALL_IRQ_PIN, FALL_INTERRUPT);
    GPIO_SetIrq(GPIO_RISING_FALL_IRQ_PIN, ENABLE);
    PLIC_SetPriority(GPIO_IRQn,2);
    PLIC_EnableIRQ(GPIO_IRQn);

    while (1)
    {
        GPIO_Config(GPIO_LED_PIN, GPIO_MODE_OUT_HIGH);

        rt_kprintf(".......run %d\n", i++);
        rt_thread_mdelay(150);

        GPIO_Config(GPIO_LED_PIN, GPIO_MODE_OUT_LOW);
        rt_thread_mdelay(150);
    }
    return 0;
}


void GPIO_IRQHandler(void)
{
    uint8_t irq_status = 0;
    for(int i = 0;i <= GPIOE;i++) // find which group
    {
        irq_status=GPIO_GetPortIrqStatus(i);
        if(irq_status != 0)
        {
            for(int j=0;j<8;j++) // find which io
            {
                if(BIT_GET(irq_status,j))
                {
                    yc_log("gpio %d interrupt",j + (8*i));
                    GPIO_ClearIrq(j + (8*i));
                }
            }
        }
    }
}

/**
  * @brief  trap_c
  * @param  None
  * @retval None
  */
void trap_c(void)
{
    _ASSERT(0);
    while(1);
}
