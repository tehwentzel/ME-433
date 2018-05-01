#ifndef IMU_H__    /* Guard against multiple inclusion */
#define IMU_H__

#define SLAVE 0b1101011 // I2C slave address for the IMUS
#define WHO_AM_I 0x0F //who_am_i register address
#define CTRL1_XL 0x10 // accelerometer control register address
#define CTRL_2G 0x11 //gyroscope control register address
#define CTRL_3C 0x12 //has if_inc bit which we set to 1 for serial communication
#define ONEG 16000 //value output of the imu for 1g of acceleration, lower bounded for error
//all of the 16-bit output ports H = lsb, L = ms, XL = accelerometer, G = gyro
#define OUT_TEMP_L 0x20

#define XGPOS 2
#define YGPOS 4
#define ZGPOS 6
#define XXLPOS 8
#define YXLPOS 10
#define ZXLPOS 12

void imu_init(void);
void read_imu_data(char *, int);
char read_whoami(void);
signed short getTemp(unsigned char *);
signed short getCoord(unsigned char *, int);
int getXL(unsigned char *, int);


#endif