
/*
 * servo.h
 *
 * Created: 4/4/2017 9:54:53 AM
 *  Author: Josh
 */

#ifndef _SERVO_H
#define _SERVO_H

void init_servos(void);
void adjust_pan_angle(int angle);
void adjust_tilt_angle(int angle);

#endif /* _SERVO_H */