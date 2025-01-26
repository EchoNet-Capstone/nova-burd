#include <stdio.h>
#include <GPS_Air530Z.h>
#include <Arduino.h>

#include "gps.h"
#include "gps_data_class.h"
#include "timers.h"
#include "globals.h"
#include "device_state.h"
#include "my_clock.h"
#include "watchdog.h"
#include "burd_radio.h"

// GPS Configuration

// if GPS module is Air530, use this
// Air530Class  Air530; 

// if GPS module is Air530Z, use this
Air530ZClass Air530;

void gps_init(){
  //Setup GPS Power Control
  // This is controlled by Air530 library
  pinMode(GPS_POWER, OUTPUT);
  gps_wake();
  Air530.begin();

  // Air530.reset();

  // Disable GPS module on boot
  gps_sleep();
  Serial.printf("GPS Initialized\n");
}

int is_air_available(){
  return Air530.available();
}

void gps_wake(){
  if(get_gps_enabled() == false){
    // This is controlled by Air530 library?
    // digitalWrite(gps_power, LOW);
    // Air530.begin(9600);
    // Doesn't seem to do anything
    // Air530.setPPS(3, 200);
    // Air530.setmode(MODE_GPS);
    // Air530.sendcmd("$PGKC115,1,1,1,1*2B\r\n"); // mode: gps, glonass, beidou, galileo [0: off, 1: on]
    // Air530.setNMEA(NMEA_GGA | NMEA_RMC | NMEA_VTG);
    // Air530.setNMEA(NMEA_GST);
    // Air530.setNMEA(NMEA_GGA);
  }

  set_gps_enabled(true);

  // Keep CPU awake if GPS is in use
  // sleep_inhibit = 1;
}

void gps_sleep(){
  if(get_gps_enabled() == 1){
    Air530.end();
  }

  set_gps_enabled(0);

  // Allow CPU to go to sleep if GPS is not in use
  // sleep_inhibit = 0;
}

void gps_service(){
  // Enable GPS if release is waiting to be retrieved
  if(GPS_ENABLE && get_waiting_to_be_retrieved() && ((InternalClock() - get_last_gps_fix()) > GPS_INTERVAL)) {
    // Serial.printf("Waking GPS.\n");
    // This will only wake if GPS is asleep
    gps_wake();

    if (is_air_available() > 0) {
      if (DEBUG){
        Serial.printf("GPS Available.\n");
      }

      update_gps();
    }
  }
  else{
    gps_sleep();
  }
}

void update_gps(){
  // Pet the watchdog
  feedInnerWdt();

  while(Air530.available() > 0){
    Air530.encode(Air530.read());
  }

  String NMEA = Air530.getGGA();

  if (NMEA == "0"){
    return;
  }

  if(DEBUG){
    Serial.println(NMEA);
  }

  GGADataClass gps_data = GGADataClass(NMEA);

  char lora_tx_packet[LORA_BUFFER_SIZE];

  int lora_packet_len = sprintf(lora_tx_packet, "%05d, %f, %f, %i%%\n", UNIT_ID, gps_data.latitude, gps_data.longitude, get_battery_percent());

  //sprintf(gps_latitude_temp_string, "%s %f", "Lat:", gps_latitude);

  //snprintf(txpacket, sizeof txpacket, "%s%s%s%s", str1, str2, str3, str4);
  if(DEBUG){
    Serial.printf("GPS Time: %02d:%02d", gps_data.time_hours, gps_data.time_minutes);
    Serial.printf(" Lat: ");
    Serial.print(gps_data.latitude, 6);
    Serial.print(" Long: ");
    Serial.print(gps_data.longitude, 6);
    Serial.print(" HDOP: ");
    Serial.print(gps_data.hdop, 2);
    Serial.println();
    Serial.println(lora_tx_packet);
  }

  if((gps_data.hdop > 0.1 ) && (gps_data.hdop < 1.5)){
    set_last_gps_fix(InternalClock());
  }

  if(DEBUG){
    Serial.println();
  }
}