#include<xc.h>           // processor SFR definitions
#include "lcd.h"
#include "st7735.h"
#include<stdio.h>
#include "i2c_master_noint.h"


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
