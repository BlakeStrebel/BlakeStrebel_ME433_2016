#ifndef LSM6DS33_H
#define LSM6DS33_H

#define IMU_ADDRESS 0b1101011   // IMU address

// config registers
#define CTRL1_XL 0x10 
#define CTRL2_G 0x11 
#define CTRL3_C 0x12

// output registers
#define OUT_TEMP_L 0x20
#define OUTX_L_XL 0x28


void initIMU(void);
unsigned char WHOAMI(void);


#endif
