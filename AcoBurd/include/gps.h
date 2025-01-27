#ifndef GPS_H
#define GPS_H

#define GPS_POWER GPIO14                      // Power control for Air530 GPS Module - active low

void gps_init(void);
void gps_wake(void);
void gps_sleep(void);
void gps_service(void);
void update_gps(void);

int is_air_available(void);

#endif