/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
char msg_buffer[MAX_STRLEN];
unsigned char data[14];
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/
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

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
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
    LCD_clearScreen(COLOROFF);
    drawBars(48);
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
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
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
