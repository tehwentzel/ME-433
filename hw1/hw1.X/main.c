#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

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

#define CLOCKTIME 48000000 // 40000 yields 0.001 s delay time when using Core Timer

void delay(void);

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
    TRISAbits.TRISA4 = 0; // Pin 4 of Port A is LED1. Clear bit 
                    // to zero, for output. Others are inputs.
    TRISBbits.TRISB4 = 1; // Pin 4 of Port B is USER button (input)
    // TRISA = 0x00;
    LATAbits.LATA4 = 1; // Turn LED1 ON

    __builtin_enable_interrupts();
    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the CPU speed
        double lps = 1000; //hz of the LED
        double delaynum = (CLOCKTIME/4)/lps; //converts clock cycels to hz
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < delaynum){
            if(!PORTBbits.RB4){
                LATAbits.LATA4 = 0;
            }
        }
        LATAINV = 0x0010;   // toggle LED1, 0x0010; 0xFFFF: toggle EVERYTHING
        ;
    }
    return 0;
}
