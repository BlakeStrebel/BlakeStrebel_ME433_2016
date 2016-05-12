#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>        // sprintf
#include"ILI9163C.h"     // LCD
#include"LSM6DS33.h"     // IMU
#include"I2C.h"          // I2C

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
    
    
    // initialize modules
    SPI1_init(); // initialize SPI communication
    LCD_init();  // initialize LCD
    i2c_master_setup(); // initialize I2C communication on PIC
    initIMU();// initialize IMU
    if (WHOAMI() != 0b01101001)
    {
        LATAINV = 10000; // check IMU
    }
    
    __builtin_enable_interrupts();
    
     // Text formating
    LCD_clearScreen(WHITE);
    int x = 3;
    int y = 10;
    int xscale = 15;
    int yscale = 10;
    char message[100] ;
    sprintf(message,"Accelerometer: ");
    LCD_drawArray(x, y, BLUE, message);
    sprintf(message,"X = ");
    LCD_drawArray(x, y + yscale, BLUE, message);
    sprintf(message,"Y = ");
    LCD_drawArray(x, y + 2*yscale, BLUE, message);
    sprintf(message,"Z = ");
    LCD_drawArray(x, y + 3*yscale, BLUE, message);
    sprintf(message,"Gyroscope: ");
    LCD_drawArray(x, y + 4*yscale, BLUE, message);
    sprintf(message,"X = ");
    LCD_drawArray(x, y + 5*yscale, BLUE, message);
    sprintf(message,"Y = ");
    LCD_drawArray(x, y + 6*yscale, BLUE, message);
    sprintf(message,"Z = ");
    LCD_drawArray(x, y + 7*yscale, BLUE, message);
    sprintf(message,"Temperature: ");
    LCD_drawArray(x, y + 8*yscale, BLUE, message);
    sprintf(message,"T = ");
    LCD_drawArray(x, y + 9*yscale, BLUE, message);
    
    
    char length = 14;
    unsigned char IMU_data[length];
    short x_accel, y_accel, z_accel, x_gyro, y_gyro, z_gyro, temp;
    
   // while(1) {
        
       // _CP0_SET_COUNT(0);
        
        i2c_read_multiple(IMU_ADDRESS, OUT_TEMP_L, IMU_data, length);
        x_accel = (IMU_data[1] << 8 )| IMU_data[0];
        y_accel = (IMU_data[3] << 8 )| IMU_data[2];
        z_accel = (IMU_data[5] << 8 )| IMU_data[4];
        x_gyro = (IMU_data[7] << 8 )| IMU_data[6]; 
        y_gyro = (IMU_data[9] << 8 )| IMU_data[8];
        z_gyro = (IMU_data[11] << 8 )| IMU_data[10];
        temp = (IMU_data[13] << 8 )| IMU_data[12];
       
        sprintf(message," %d",x_accel);
        LCD_drawArray(x + xscale, y + yscale, BLUE, message);
        sprintf(message," %d",y_accel);
        LCD_drawArray(x + xscale, y + 2*yscale, BLUE, message);
        sprintf(message," %d",z_accel);
        LCD_drawArray(x + xscale, y + 3*yscale, BLUE, message);
        sprintf(message," %d",x_gyro);
        LCD_drawArray(x + xscale, y + 5*yscale, BLUE, message);
        sprintf(message," %d",y_gyro);
        LCD_drawArray(x + xscale, y + 6*yscale, BLUE, message);
        sprintf(message," %d",z_gyro);
        LCD_drawArray(x + xscale, y + 7*yscale, BLUE, message);
        sprintf(message," %d",temp);
        LCD_drawArray(x + xscale, y + 9*yscale, BLUE, message);
        
        
      //  while(_CP0_GET_COUNT() < 24000000) { 
        ;   // Wait for 1 s
      //  }
 //   } 
    
} 
