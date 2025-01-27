#ifndef GPS_DATA_CLASS_H
#define GPS_DATA_CLASS_H

#include <Arduino.h>

class GGADataClass{
public:
  GGADataClass(String NMEA_data);
  String time_string;
  String latitude_string;
  String longitude_string;
  String north_south_string;
  String east_west_string;
  String fix_string;
  String satellites_string;
  String hdop_string;
  String altitude_string;

  int time;
  int time_hours;
  int time_minutes;
  
  float latitude;
  float longitude;
  float hdop;
private:
  void process_string_info(String NMEA_Data);
  void convert_to_number();
  void get_latitude();
  void get_longitude();
};

#endif