#ifndef MOTOR_CONTROL_H
#define	MOTOR_CONTROL_H

#define MOTOR1_PWM OC1RS
#define MOTOR2_PWM OC4RS
#define MOTOR1_SENSOR T5CKRbits.T5CKR
#define MOTOR2_SENSOR T3CKRbits.T3CKR 
#define MAX_DUTY 2399.0
#define TICKS_PER_ROTATION 441.0 
#define Y_DIFF 100.0 //placeholder for number of pixels that the android app offsets the y readings by
#define PGAIN 50
#define EGAIN 2.5  //should be roughly full screen off = max difference?
#define IGAIN .3
#define REF_TICKS 735 //assume 100% signal -> 100 rpm, this is the reference signal at max speed?
#define MSCALE .25
//static int refCount;
void delay(double);
void runMotor(double, double);
int pControl(int);
//void pdControl(char *);
void initPWM(void);
int bound(int);
int boundError(int);


#endif	/* MOTOR_CONTROL_H */

