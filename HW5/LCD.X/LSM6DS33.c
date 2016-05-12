#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include "I2C.h"
#include "LSM6DS33.h"

void initIMU(void) {
    
    // Configure accelerometer
    i2c_master_start();     // make the start bit
    i2c_master_send(IMU_ADDRESS<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(CTRL1_XL);  // write to IODIR
    i2c_master_send(0x80);  // Sample rate 1.66 kHz, 2g mode, 400 Hz filter
    i2c_master_stop();      // make the stop bit  
    
    // Configure gyroscope
    i2c_master_start();     // make the start bit
    i2c_master_send(IMU_ADDRESS<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(CTRL2_G);  // write to IODIR
    i2c_master_send(0x80);  // Sample rate 1.66 kHz, 245 dps
    i2c_master_stop();      // make the stop bit  
    
    // Configure device for multiple byte access
    i2c_master_start();     // make the start bit
    i2c_master_send(IMU_ADDRESS<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(CTRL3_C);  // write to IODIR
    i2c_master_send(0x04);  // register address automatically incremented during multiple byte access
    i2c_master_stop();      // make the stop bit 
    
}

unsigned char WHOAMI(void)
{
    i2c_master_start();             // make the start bit
    i2c_master_send(IMU_ADDRESS<<1|0);  // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(0x0F);          // read from WHOAMI
    i2c_master_restart();           // make the restart bit
    i2c_master_send(IMU_ADDRESS<<1|1);  // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    char r = i2c_master_recv();     // save the values
    i2c_master_ack(1);              // make the ack
    i2c_master_stop();              // make the stop bit
    return r;                       // return PORT values
}