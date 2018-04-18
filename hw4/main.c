#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>

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

#define CLOCKTIME 40000000 // 40000 yields 0.001 s delay time when using Core Timer
#define CS LATAbits.LATA0 //A0 is the chip-select pin

void init_spi() {
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0X1;            // baud rate
    CS = 0; // sets CS to high/off
    RPA1Rbits.RPA1R = 0b0011; //A1R is an output connected to SDI
    SPI1CONbits.MSTEN = 1;  //
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 41
    return;
}

unsigned char spi1_io(unsigned char w){
  ///write a character to SPI1 Buffer
    SPI1BUF = w;
    while(!SPI1STATbits.SPIRBF){
        ;  //this is the equivalent of pass, wats for write to occur
    }
    return SPI1BUF;
}

void set_voltage(unsigned char channel, unsigned int voltage){
    unsigned char lsb;
    unsigned char msb;
    
    CS = 0; //turn on DAC
    lsb = voltage << 4;
    msb = ((channel << 7) | (0b111 << 4) | (voltage >> 4));
    spi1_io(msb);
    spi1_io(lsb);
    CS = 1; //turn off
    return;
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
    TRISAbits.TRISA4 = 0; // Pin 4 of Port A is LED1. Clear bit 
    TRISBbits.TRISB4 = 1; // Pin 4 of Port B is USER button (input)
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 1;
    // TRISA = 0x00;
    LATAbits.LATA4 = 1; // Turn LED1 ON

    __builtin_enable_interrupts();
    init_spi();
    _CP0_SET_COUNT(0);
    
    int x = 0;
    int delaynum = 4800000/4000;
    int period = 200;
    int amp = 256/2;
    double tri_x;
    while(1) {
        
	    // channel 0 is a sign wave
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < delaynum){
            ;//delay
        }
        
        double sinval = amp + amp*sin(x*2*3.14159/period) - 1;
        set_voltage(0,sinval);
        
        double trival = (2*amp - 1)*abs(period - x)/period;
        set_voltage(1,trival);
        
        x = (x+1)%(2*period);
    }
    return 0;
}

