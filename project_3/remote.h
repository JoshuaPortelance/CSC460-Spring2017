#ifndef _REMOTE_H
#define _REMOTE_H

// Constants.
static float MINSERVO   = 546;    //minimum servo arc measured
static float MAXSERVO   = 2403;   //maximum servo arc measured
static float MAXSPEED   = 500;    //maximum Roomba forward velocity
static float MINSPEED   = -500;   //minimum Roomba reverse velocity
static float MAXRADIUS  = 2000;   //maximum Roomba turn radius
static float MINRADIUS  = -2000;  //minimum Roomba turn radius
static int   MAXINPUT   = 50;     //max receiving buffer size

// PAN/TILT Servo Variables.
int pan_current_speed  = 0;
int pan_target_speed   = 0;
int tilt_current_speed = 0;
int tilt_target_speed  = 0;
//int pan_servo_pin  = 2; This is just here for a reminder.
//int tilt_servo_pin = 3; This is just here for a reminder.

// Roomba Variables.
int roomba_target_speed   = 0;
int roomba_target_radius  = 0;
int roomba_current_speed  = 0;
int roomba_current_radius = 0;
//int roomba_brc_pin      = 23; This is just here for a reminder.
//int roomba_serial       = 1;  This is just here for a reminder.

// Laser Init.
//int laser_toggle_pin  = 22; This is just here for a reminder.
int laser_state         = 0;    //0 is off, 1 is on.
int laser_target_state  = 0;	//0 is off, 1 is on.

#endif /* _REMOTE_H */