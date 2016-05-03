#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"SPI.h"          // SPI and DAC
#include"I2C.h"          // I2C and Expander
#include<math.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt*
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN =  OFF// wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0000000000000001 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module


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
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LATAbits.LATA4 = 1;
    LATAINV = 10000;
    
    // turn off analog input for I2C2 pins
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
    __builtin_enable_interrupts();
    
    initSPI1();         // initialize SPI communication on PIC
    i2c_master_setup(); // initialize I2C communication on PIC
    
    initExpander(); // initialize Pin Expander
    
    float vb = 0;   // 5 Hz triangle wave
    float va = 127; // 10 Hz sine wave centered at Vref/2
    float t = .001; 
    
    setExpander(0x01,0);
    
    unsigned char button;
    
    while(1) {
        
        _CP0_SET_COUNT(0);  // Set system clock to zero
        while(_CP0_GET_COUNT() < 24000) { 
        ;   // Wait for 1 ms
        }
 
       // DAC
       setVoltage(0, va); // set DAC voltages
       setVoltage(1, vb);
      
       // update DAC voltage values
       vb = vb + 2.56;
       va = 127 + 127 * sin(62.83*t);   //sin(2*pi*f*t)
       t = t + .001;
               
       if (vb == 256) {
           vb = 0;
       }
       if (t == 1) {
           t = 0;
       }
       
       // Pin Expander
        button = getExpander();
        
        if ((button & 0x80) >> 7 != 0) {
            setExpander(0,1);
        }
        else {
            setExpander(0,0);
        }      
    }
}