#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "st7735.h"
#include<stdio.h>

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

void draw_char(unsigned short x, unsigned short y, char msg){
    //all code shamelessly stolen from the whiteboard in class
    int col = 0;
    char pixels;
    int j;
    for(col = 0; col < 5; col++){  //letters are 5 pixels wide
        pixels = ASCII[msg-32][col];  //ASCII starts at 32, the array we're reading starts at 0
        for(j = 0; j<8;j++){ //letters are 8 bits tall
            if((x + col < MAX_X)&(y+j < MAX_Y)){
                if((pixels>>j)&1){
                    LCD_drawPixel(x+col,y+j,COLORON);
                }
                else{
                    LCD_drawPixel(x+col,y+j,COLOROFF);
                }
            }
        }
    }
}

void draw_string(unsigned short x, unsigned short y, char* msg){
    char idx = 0;
    int i = 0;
    int j = 0;
    while(msg[idx]){
        if(x+i+10 > MAX_X){//test wrapping
            j = j+12;
            i = 0;
        }
        draw_char(x+i,y+j,msg[idx]);
        i = i+5;
        idx++;
    }
}

void draw_loading_bar(unsigned short x, unsigned short y, unsigned int percentage){
    unsigned int bar_width = MAX_X - 5 -  x;  //give ten pixels of space on the end 
    int bar_height;
    if(y + 10 < MAX_Y){
        bar_height = 10;
    }
    else{
        bar_height = MAX_Y - y;
    }
    unsigned int loaded = bar_width*percentage/100;
    int i = 0;
    int j;
    while(x+i <= loaded){
        j = 0;
        while(j < bar_height){
            LCD_drawPixel(x+i,y+j,BLUE);
            j++;
        }
        i++;
    }
    while(x+i <= bar_width){
        j = 0;
        while(j < bar_height){
            LCD_drawPixel(x+i,y+j,GREEN);
            j++;
        }
        i++;
    }
}

void delay(double seconds){
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < (DELAYTIME * seconds)){ ; }
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
    //LATBbits.LATB7 = 1;
    
    LCD_init();
    __builtin_enable_interrupts();
    char msg_buffer[MAX_STRLEN];
    int percent;
    
    while(1){
        LCD_clearScreen(COLOROFF);
        for(percent = 0; percent <= 100; percent++){
            sprintf(msg_buffer, "%d %% complete...", percent);
            draw_string(5,6,msg_buffer);
            draw_loading_bar(5,18,percent);
            //delay(1);
        }
        delay(3);
        LCD_clearScreen(COLOROFF);
        sprintf(msg_buffer, "Well, this is         anti-climactic");
        draw_string(10,MAX_Y/2 -4,msg_buffer);
        delay(10);
    }
    return 0;
}