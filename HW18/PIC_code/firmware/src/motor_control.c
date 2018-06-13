#include<xc.h>  
#include "motor_control.h"

static int mSpeed1;
static int mSpeed2;

void delay(double seconds){
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < (4800000 * seconds)){ ; }
}

void initPWM(){  
    RPA0Rbits.RPA0R = 0b0101; // A0 to OC1
    RPB13Rbits.RPB13R = 0b0101; // B13 to OC4
    
    T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
    PR2 = MAX_DUTY; // 48000000 Hz / 20000 Hz / 1 - 1 = 2399 (20kHz PWM from 48MHz clock with 1:1 prescaler)
    TMR2 = 0; // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OCxCON bits are defaults
    MOTOR1_PWM = 0; // duty cycle
    OC1R = 0; // initialize before turning OC1 on; afterward it is read-only
    OC4CONbits.OCM = 0b110; // PWM mode without fault pin; other OCxCON bits are defaults
    MOTOR2_PWM = 0; // duty cycle
    OC4R = 0; // initialize before turning OC4 on; afterward it is read-only
    T2CONbits.ON = 1; // turn on Timer2
    OC1CONbits.ON = 1; // turn on OC1
    OC4CONbits.ON = 1; // turn on OC4
    
    MOTOR1_SENSOR = 0b0100; // B9 is read by T5CK
    MOTOR2_SENSOR = 0b0100; // B8 is read by T3CK
    
    T5CONbits.TCS = 1; // count external pulses
    PR5 = 0xFFFF; // enable counting to max value of 2^16 - 1
    TMR5 = 0; // set the timer count to zero
    T5CONbits.ON = 1; // turn Timer on and start counting
    T3CONbits.TCS = 1; // count external pulses
    PR3 = 0xFFFF; // enable counting to max value of 2^16 - 1
    TMR3 = 0; // set the timer count to zero
    T3CONbits.ON = 1; // turn Timer on and start counting
    
    T4CONbits.TCKPS = 2; // Timer4 prescaler N=4
    PR4 = 23999; // 48000000 Hz / 500 Hz / 4 - 1 = 23999 (500Hz from 48MHz clock with 4:1 prescaler)
    TMR4 = 0; // initial TMR4 count is 0
    T4CONbits.ON = 1;
    IPC4bits.T4IP = 4; // priority for Timer 4 
    IFS0bits.T4IF = 0; // clear interrupt flag for Timer4
    IEC0bits.T4IE = 1; // enable interrupt for Timer4
    mSpeed1 = 0;
    mSpeed2 = 0;
}

void runMotor(double leftSpeed, double rightSpeed){//assumes speed is a percentage
    TRISAbits.TRISA0 = (uint32_t)MAX_DUTY*leftSpeed;
    TRISBbits.TRISB13 = (uint32_t)MAX_DUTY*rightSpeed;
}

int pControl(int error){
    int left, right;
    //char error = data[0]; //240 is center of camera
    if (error< -50) { // slow down the left motor to steer to the left
        if (error < -400){
           left = MSCALE*(MAX_DUTY + EGAIN*error);
           right = 0.8*MSCALE*(MAX_DUTY - EGAIN*error);
        }
        else{
            left = MSCALE*(MAX_DUTY + EGAIN*error);
            right = MSCALE*(MAX_DUTY - EGAIN*error);
        }
    }
    else if (error > 50) { // slow down the right motor to steer to the right
        if (error > 400){
            right = MSCALE*(MAX_DUTY - EGAIN*error);
            left = 0.8*MSCALE*(MAX_DUTY + EGAIN*error);
        }
        else{
            right = MSCALE*(MAX_DUTY - EGAIN*error);
            left = MSCALE*(MAX_DUTY + EGAIN*error);
        }
    }
    else{
        left = 1.2*MSCALE*MAX_DUTY;
        right = 1.2*MSCALE*MAX_DUTY;
    }
    mSpeed2 = bound(left);
    mSpeed1 = bound(right);
    if(error == 9001){
        mSpeed2 = .9*MAX_DUTY;
        mSpeed1 = .9*MAX_DUTY;
        delay(.1);
    }
    return(error);
}

//void pdControl(char * data){
//    double left, right;
//    double e1 = 240.0 - data[0]; //assumeing e1 is the bottom pixel
//    double e2 = 240.0 - data[1];
//    double e3 = 240.0 -data[2];
//    double de_dt1 = Y_DIFF/(e2 - e1);
//    double de_dt2 = Y_DIFF/(e3 - e2);
//    double eAvg = (e1 + e2 + e3)/3.0;
//    double de_dt_avg = (de_dt1 + de_dt2)/2;
//    if(eAvg < 0){
//        left = (MAX_DUTY + PGAIN*eAvg + DGAIN*de_dt_avg)/MAX_DUTY;
//        right = MAX_DUTY;
//        }
//    else { // slow down the right motor to steer to the right
//        right = (MAX_DUTY - PGAIN*eAvg - DGAIN*de_dt_avg)/MAX_DUTY;
//        left = MAX_DUTY;
//        }
//    }

int bound(int signal){
    if (signal < - 2*MSCALE*MAX_DUTY){
        signal = - 2*MSCALE*MAX_DUTY;
    }
    if (signal > 2*MSCALE*MAX_DUTY){
        signal = 2*MSCALE*MAX_DUTY;
    }
    return(signal);
}

int boundError(int signal){
    if (signal < - MSCALE*MAX_DUTY){
        signal = - MSCALE*MAX_DUTY;
    }
    if (signal > MSCALE*MAX_DUTY){
        signal = MSCALE*MAX_DUTY;
    }
    return(signal);
}