#include "rtthread.h"
#include "register.h"
#include "riscv-ops.h"
#include "yc_vector.h"

extern volatile rt_ubase_t  rt_interrupt_from_thread;
extern volatile rt_ubase_t  rt_interrupt_to_thread;
extern volatile rt_uint32_t rt_thread_switch_interrupt_flag;

void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to)
{
    if (rt_thread_switch_interrupt_flag == 0)
        rt_interrupt_from_thread = from;

    rt_interrupt_to_thread = to;
    rt_thread_switch_interrupt_flag = 1;

    /*trig soft interrupt*/
    CORE_CHIP_SYS->rtc_systick_sync_cmd.bit.software_irq =1;

    VECTOR_EnableIRQ(VECTOR_SOFT_IRQn);
}
