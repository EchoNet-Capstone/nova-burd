#include <Arduino.h>
#include "gps_data_class.h"

GGADataClass::GGADataClass(String NMEA_data){
  process_string_info(NMEA_data);
  convert_to_number();
}

void GGADataClass::process_string_info(String NMEA_data){
  int commas[15];
  commas[0] = NMEA_data.indexOf(',');

  for(int i = 1; i < 10; i++){
    commas[i] = NMEA_data.indexOf(',', commas[i - 1] + 1 );
  }

  time_string = NMEA_data.substring(commas[0] + 1, commas[1]);
  latitude_string = NMEA_data.substring(commas[1] + 1, commas[2]);
  north_south_string = NMEA_data.substring(commas[2] + 1, commas[3]);
  longitude_string = NMEA_data.substring(commas[3] + 1, commas[4]);
  east_west_string = NMEA_data.substring(commas[4] + 1, commas[5]);
  fix_string = NMEA_data.substring(commas[5] + 1, commas[6]);
  satellites_string = NMEA_data.substring(commas[6] + 1, commas[7]);
  hdop_string = NMEA_data.substring(commas[7] + 1, commas[8]);
  altitude_string = NMEA_data.substring(commas[8] + 1, commas[9]);
}

void GGADataClass::convert_to_number(){
  // Convert time
  time = time_string.toInt();
  time_hours = time / 10000;
  time_minutes = (time - (time_hours * 10000) ) / 100;

  get_latitude();

  get_longitude();

  hdop = hdop_string.toFloat();
}

void GGADataClass::get_latitude(){
  float temp_latitude = latitude_string.toFloat();

  if(north_south_string == "S"){
    temp_latitude = -temp_latitude;
  }

  temp_latitude = temp_latitude / 100;

  int latitude_degrees = (int) temp_latitude;
  float latitude_minutes = 100 * ((temp_latitude - latitude_degrees) / 60);

  latitude = (float)latitude_degrees + latitude_minutes;
}

void GGADataClass::get_longitude(){
  float temp_longitude = longitude_string.toFloat();

  if(east_west_string == "W"){
    temp_longitude = -temp_longitude;
  }

  temp_longitude = temp_longitude / 100;

  int longitude_degrees = (int)temp_longitude;
  float longitude_minutes = 100 * ( (temp_longitude - longitude_degrees) / 60 );

  longitude = (float)longitude_degrees + longitude_minutes;
}