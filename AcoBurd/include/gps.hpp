#ifndef GPS_H
#define GPS_H

void gps_wake(void);
void gps_sleep(void);
void update_gps(void);

long get_last_gps_fix(void);
void set_last_gps_fix(long new_last_gps_fix);

int is_air_available(void);

#endif