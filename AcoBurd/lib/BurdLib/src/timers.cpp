#include <stdint.h>

#include "get_set_macros.hpp"
#include "my_clock.hpp"

#include "timers.hpp"

// timer vars
GET_SET_FUNC_DEF(uint32_t, lora_timer, 0)
GET_SET_FUNC_DEF(uint32_t, display_timer, 0)
GET_SET_FUNC_DEF(uint32_t, encoder_timer, 0)
GET_SET_FUNC_DEF(uint32_t, release_timer, 0)
GET_SET_FUNC_DEF(uint32_t, release_timer1, 0)
GET_SET_FUNC_DEF(uint32_t, release_timer2, 0)
GET_SET_FUNC_DEF(uint32_t, battery_timer, 0)
GET_SET_FUNC_DEF(uint32_t, time_until_release, 0)

void
release_service(
    void
){
    set_time_until_release(get_release_timer() - InternalClock());

    if(get_time_until_release() < 0){
        set_time_until_release(0);
    }
}