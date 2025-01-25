#include <stdlib.h>

#include "get_set_macros.h"
#include "device_state.h"
#include "motor.h"

GET_SET_DEF(int, battery_percent, 0);
GET_SET_DEF(bool, display_active, false);

// GPS Globals
GET_SET_DEF(bool, gps_enabled, false);
GET_SET_DEF(bool, gps_lock, false);
// Initial value hack needed to aquire GPS fix on bootup
GET_SET_DEF(long, last_gps_fix, -3600);


//Motor Globals
GET_SET_DEF(int, motor_position, 0);

//Sleep Globals
GET_SET_DEF(bool, low_power, false);
GET_SET_DEF(bool, sleep_inhibit, true);
GET_SET_DEF(long, reed_switch_first_press, 0);
GET_SET_DEF(int, timer_tap_multiplier1, 0);
GET_SET_DEF(int, timer_tap_multiplier2, 0);

GET_SET_DEF(bool, reed_switch1, false);
GET_SET_DEF(bool, reed_switch2, false);
GET_SET_DEF(bool, waiting_to_be_retrieved, false);
GET_SET_DEF(bool, release_is_open, false);
GET_SET_DEF(bool, release_last_position, false);

void am_i_waiting_to_be_recovered(){
  if(abs(get_motor_position()) < (OPEN_POSITION + 1000)){
    set_release_is_open(true);
  }
  else if(abs(get_motor_position()) > (CLOSED_POSITION - 1000)){
    set_release_is_open(false);
  }

  if ( get_reed_switch1() || get_reed_switch2() ){
    set_waiting_to_be_retrieved(false);
  }
  else if((get_release_is_open() == true) && (get_release_last_position() == true)){
    set_waiting_to_be_retrieved(true);
  }

  // Save state for next time
  set_release_last_position(get_release_is_open());
}