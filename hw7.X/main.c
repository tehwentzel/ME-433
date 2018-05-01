#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "st7735.h"
#include "i2c_master_noint.h"
#include<stdio.h>
#include "imu.h"
#include "lcd.h"

// DEVCFG0
#pragma config DEBUG = 0b11 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = 1 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 0b00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b11 // do not enable clock switch
#pragma config WDTPS = 0b10100 // slowest wdt
#pragma config WINDIS = 1 // no wdt window -- are this comment and the following one reversed?
#pragma config FWDTEN = 0 // wdt off by default -- see note on previous comment
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 0b001 // divide input clock to be in range 4-5MHz: div by 2
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV, 24x
#pragma config FPLLODIV = 0b001 // divide clock after FPLLMUL to get 48MHz: div by 2
#pragma config UPLLIDIV = 0b001 // divider (2x) for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0xFFFF // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module

#define DELAYTIME 4800000 // 40000 yields 0.001 s delay time when using Core Timer
#define COLORON 0x0020  //Blue?
#define COLOROFF 0xDDDD  //Slightly off-white
#define MAX_X 128
#define MAX_Y 160
#define MAX_STRLEN 50
#define XLUNIT .000061 //converts the short value for acceleration into g's

void delay(double seconds){
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < (DELAYTIME * seconds)){ ; }
}

void clearBars(int xlen, int ylen){
    int xpos;
    int ypos;
    if(xlen > 0){
        xpos = MAX_X/2;
        ypos = MAX_Y/2 - 2;
        while(xpos < MAX_X/2 + xlen){
            while(ypos < MAX_Y/2 + 2){
                LCD_drawPixel(xpos,ypos, COLORBAR);
                ypos++;
            }
            ypos = MAX_Y/2 - 2;
            xpos++;
        }
    }
    if(xlen < 0){
        xpos = MAX_X/2;
        ypos = MAX_Y/2 - 2;
        while(xpos > MAX_X/2 + xlen){
            while(ypos < MAX_Y/2 + 2){
                LCD_drawPixel(xpos,ypos, COLORBAR);
                ypos++;
            }
            ypos = MAX_Y/2 - 2;
            xpos--;
        }
    }
    if(ylen < 0){
        xpos = MAX_X/2 - 2;
        ypos = MAX_Y/2;
        while(ypos > MAX_Y/2 + ylen){
            while(xpos < MAX_X/2 + 2){
                LCD_drawPixel(xpos,ypos, COLORBAR);
                xpos++;
            }
            xpos = MAX_X/2 - 2;
            ypos--;
        }
    }
    if(ylen > 0){
        xpos = MAX_X/2 - 2;
        ypos = MAX_Y/2;
        while(ypos < MAX_Y/2 + ylen){
            while(xpos < MAX_X/2 + 2){
                LCD_drawPixel(xpos,ypos, COLORBAR);
                xpos++;
            }
            xpos = MAX_X/2 - 2;
            ypos++;
        }
    }
}

void drawBars(int len){
    int xpos = MAX_X/2;
    int ypos = MAX_Y/2 - 2;
    while(xpos < MAX_X/2 + len){
        while(ypos < MAX_Y/2 + 2){
            LCD_drawPixel(xpos,ypos, COLORBAR);
            ypos++;
        }
        ypos = MAX_Y/2 - 2;
        xpos++;
    }
    xpos = MAX_X/2;
    ypos = MAX_Y/2 - 2;
    while(xpos > MAX_X/2 - len){
        while(ypos < MAX_Y/2 + 2){
            LCD_drawPixel(xpos,ypos, COLORBAR);
            ypos++;
        }
        ypos = MAX_Y/2 - 2;
        xpos--;
    }
    xpos = MAX_X/2 - 2;
    ypos = MAX_Y/2;
    while(ypos > MAX_Y/2 - len){
        while(xpos < MAX_X/2 + 2){
            LCD_drawPixel(xpos,ypos, COLORBAR);
            xpos++;
        }
        xpos = MAX_X/2 - 2;
        ypos--;
    }
    xpos = MAX_X/2 - 2;
    ypos = MAX_Y/2;
    while(ypos < MAX_Y/2 + len){
        while(xpos < MAX_X/2 + 2){
            LCD_drawPixel(xpos,ypos, COLORBAR);
            xpos++;
        }
        xpos = MAX_X/2 - 2;
        ypos++;
    }
}

int main() {
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    //TRISBbits.TRISB7 = 0; // Chip Select
    TRISAbits.TRISA1 = 0; // A1 is SDO1
    
    // Turn off AN2 and AN3 pins (make B2 and B3 available for I2C)
    ANSELBbits.ANSB2 = 0; //SDA
    ANSELBbits.ANSB3 = 0; //SCL
    
    LCD_init();
    imu_init();
    __builtin_enable_interrupts();
    char msg_buffer[MAX_STRLEN];
    LCD_clearScreen(COLOROFF);

    unsigned char data[14];
    drawBars(48);
    while(1){
        read_imu_data(data, 14);
        signed short temperature = getTemp(data);
        signed short gyroX = getCoord(data, XGPOS);
        signed short gyroY = getCoord(data, YGPOS);
        signed short gyroZ = getCoord(data, ZGPOS);
        signed short accelX = getCoord(data, XXLPOS);
        signed short accelY = getCoord(data, YXLPOS);
        signed short accelZ = getCoord(data, ZXLPOS);
        //LCD_clearScreen(COLOROFF);
        
        if(accelX < 0){
            clearBars(-10,0);
            unsigned short color = COLORHIGH;
            int xpos = MAX_X/2 + 2;
            int ypos = MAX_Y/2 -2;
            int xcount = 0;
            while(xpos < MAX_X/2 + 48){
                if(xcount < accelX -10){
                    color = COLORBAR;
                }
                while(ypos < MAX_Y/2 + 2){
                    LCD_drawPixel(xpos,ypos, color);
                    ypos++;
                }
            ypos = MAX_Y/2 - 2;
            xpos++;
            xcount = xcount - 330;
            }
        }
        if(accelX > 0){
            clearBars(10,0);
            unsigned short color = COLORHIGH;
            int xpos = MAX_X/2 - 2;
            int ypos = MAX_Y/2 -2;
            int xcount = 0;
            while(xpos > MAX_X/2 - 48){
                if(xcount > accelX -10){
                    color = COLORBAR;
                }
                while(ypos < MAX_Y/2 + 2){
                    LCD_drawPixel(xpos,ypos, color);
                    ypos++;
                }
            ypos = MAX_Y/2 - 2;
            xpos--;
            xcount = xcount + 330;
            }
        }
        if(accelY < 0){
            clearBars(0,-10);
            unsigned short color = COLORHIGH;
            int xpos = MAX_X/2 - 2;
            int ypos = MAX_Y/2 + 2;
            int ycount = 0;
            while(ypos < MAX_Y/2 + 48){
                if(ycount < accelY){
                    color = COLORBAR;
                }
                while(xpos < MAX_X/2 + 2){
                    LCD_drawPixel(xpos,ypos, color);
                    xpos++;
                }
            xpos = MAX_X/2 - 2;
            ypos++;
            ycount = ycount - 330;
            }
        }
        if(accelY > 0){
            clearBars(0,10);
            unsigned short color = COLORHIGH;
            int xpos = MAX_X/2 - 2;
            int ypos = MAX_Y/2 - 2;
            int ycount = 0;
            while(ypos > MAX_Y/2 - 48){
                if(ycount > accelY){
                    color = COLORBAR;
                }
                while(xpos < MAX_X/2 + 2){
                    LCD_drawPixel(xpos,ypos, color);
                    xpos++;
                }
            xpos = MAX_X/2 - 2;
            ypos--;
            ycount = ycount + 330;
            }
        }
        sprintf(msg_buffer, "X %d", accelX);
        draw_string(85, 85, msg_buffer);
        sprintf(msg_buffer, "Y %d", accelY);
        draw_string(70, 30, msg_buffer);
        int val = getXL(data,XXLPOS)/2;
        sprintf(msg_buffer, "Z %d", val);
        draw_string(10,10, msg_buffer);
        delay(.01);
    }
    return 0;
}