#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#define CS LATBbits.LATB15       // chip select pin

void initSPI1()
{
  // set up chip select pin as an output
  // clear CS to low when a command is beginning
  // set CS to high when a command is ending
  TRISBbits.TRISB15 = 0;
  CS = 1;
  
  // configure PIC32 pins
  SDI1Rbits.SDI1R = 0b0100; // use B8 as SDI1 pin
  RPB13Rbits.RPB13R = 0b0011;  // use B13 as SD01 pin
  
  // setup SPI1
  SPI1CON = 0;              // turn off the SPI module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x1;            // baud rate to 12 MHz [SPI4BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on SPI1
}

// send a byte via SPI and return the response
char SPI1_IO(unsigned char write)
{
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
        ;
    }
    return SPI1BUF;
}

// set voltage for MCP4902 DAC
// channel is 0 or 1 (VoutA or VoutB)
// voltage is the 8-bit output level
void setVoltage(char channel, unsigned char voltage)
{    
    CS = 0; // start writing
    
    // write data
    // (0-3) config bits 
    // (4-11) 8-bit output level
    // (12-15) XXXX
    SPI1_IO((channel << 7 | 0b01110000)|(voltage >> 4));
    SPI1_IO(voltage << 4);
    
    CS = 1; // finish writing (latch data)
}