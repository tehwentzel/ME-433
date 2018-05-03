#include <xc.h>
#include "i2c_master_noint.h" // I2C library
#include "imu.h" // high-level IMU library

void imu_init(){
    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(SLAVE << 1);
    i2c_master_send(CTRL1_XL); //accelerometer
    i2c_master_send(0b10000010); //1.6kHz, 2g, 100Hz filter
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE << 1);
    i2c_master_send(CTRL_2G); 
    i2c_master_send(0b10001000); //1.6kHz, 1000 dps
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE << 1);
    i2c_master_send(CTRL_3C);   
    i2c_master_send(0b00000100); //se if_inc to 1 for sequential reading (even tho this is the default)
    i2c_master_stop();
}

void read_imu_data(char* outputs, int num){
    i2c_master_start();
    i2c_master_send((SLAVE << 1));
    i2c_master_send(OUT_TEMP_L);
    i2c_master_restart();
    i2c_master_send((SLAVE << 1)|1);
    int count = 0;
    while(count < num){
        outputs[count] = i2c_master_recv();
        if(count >= num - 1){
            i2c_master_ack(1);
        }
        else{
            i2c_master_ack(0);
        }
        count++;
    }
    i2c_master_stop();
}

char read_whoami(){
    char whom;
    i2c_master_start();
    i2c_master_send((SLAVE << 1));
    i2c_master_send(WHO_AM_I);
    i2c_master_restart();
    i2c_master_send((SLAVE << 1) | 1);
    whom = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return whom;
}

signed short getTemp(unsigned char * data){
    signed short temp = (data[1]<<8)|data[0];
    temp = (temp+400)/16;  //temperature in celcius
    return temp;
}

signed short getCoord(unsigned char * data, int pos){
    signed short coord = (data[ pos+1 ]<<8)|data[ pos ];
    return coord;
}

int getXL(unsigned char * data, int pos){
    int coord = (data[ pos+1 ]<<8)|data[ pos ];
    return coord;
}