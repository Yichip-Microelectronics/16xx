#ifndef __BLE_TIMER_H_
#define __BLE_TIMER_H_

tmrtask *add_anchor_timer(uint ahead, void (*cb)(), int arg);
#endif // __BLE_TIMER_H_