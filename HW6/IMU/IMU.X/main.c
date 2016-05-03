#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"I2C.h"          // I2C
#include"LSM6DS33.h"     // IMU

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
    LATAbits.LATA4 = 0;
    
    // configure output compare pins
    RPA0Rbits.RPA0R = 0b0101;   // use A0 pin for OC1
    RPA1Rbits.RPA1R = 0b0101;   // use A1 pin for OC2
    
    // initialize I2C communication on PIC
     i2c_master_setup(); 
     
    // initialize IMU
     initIMU();
     if (WHOAMI() != 0b01101001)
     {
         LATAINV = 10000;
     }
 
    // Set up Peripheral Timer2, OC1 and OC2
    T2CONbits.TCKPS = 0b011;// Timer2 prescaler N = 8
    PR2 = 5999;             // Frequency = 1kHz
    TMR2 = 0;               // Timer2 initial count 0;
    
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin
    OC1CONbits.OCTSEL = 0;  // Use Timer2 for comparison
    OC1RS = 3000;           // Duty cycle 50%
    OC1R = 3000;
    
    OC2CONbits.OCM = 0b110; // PWM mode without fault pin
    OC2CONbits.OCTSEL = 0;  // Use Timer2 for comparison
    OC2RS = 3000;           // Duty cycle 50%
    OC2R = 3000;
    
    T2CONbits.ON = 1;       // Turn on Timer2
    OC1CONbits.ON = 1;      // Turn on Output Control 1
    OC2CONbits.ON = 1;      // Turn on Output Control 2
     
    __builtin_enable_interrupts();
   
    char length = 6;
    unsigned char accel_data[length];
    short x_accel, y_accel;
      
    while(1) {
        
        _CP0_SET_COUNT(0);
        
        i2c_read_multiple(IMU_ADDRESS, OUTX_L_XL, accel_data, length);
        x_accel = (accel_data[1] << 8 )| accel_data[0]; 
        y_accel = (accel_data[3] << 8 )| accel_data[2];
        
        OC1RS = x_accel*3000/16384 + 2999;
        OC2RS = y_accel*3000/16384 + 2999;
        
        while(_CP0_GET_COUNT() < 480000) { 
        ;   // Wait for 20 ms
        }
    }     
}