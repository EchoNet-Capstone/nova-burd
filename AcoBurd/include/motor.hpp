#ifndef MOTOR_H
#define MOTOR_H


void motor_sleep(void);
void motor_wake_up(void);
void motor_off(void);
void motor_reverse(void);
void motor_forward(void);
void motor_quadrature_interrupt(void);
void motor_run_to_position(int target);
void wiggle_motor(void);

// AHHHHH
void set_wiggle_timer(long new_wiggle_timer);
long get_wiggle_timer(void);

void set_motor_position(int new_motor_position);
int get_motor_position(void);

void set_motor_target(int new_motor_target);
int get_motor_target(void);

void set_motor_target_last(int new_motor_target_last);
int get_motor_target_last(void);

void set_last_quad_a_state(bool new_last_quad_a_state);
bool get_last_quad_a_state(void);

void set_last_quad_b_state(bool new_last_quad_b_state);
bool get_last_quad_b_state(void);

void set_is_motor_running(bool new_is_motor_running);
bool get_is_motor_running(void);

void set_motor_last_position1(int new_motor_last_position1);
int get_motor_last_position1(void);

void set_motor_last_position2(int new_motor_last_position2);
int get_motor_last_position2(void);

void set_motor_last_position3(int new_motor_last_position3);
int get_motor_last_position3(void);



#endif