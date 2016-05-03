#ifndef SPI_H
#define SPI_H

void initSPI1();
char SPI1_IO(unsigned char write);
void setVoltage(char channel, unsigned char voltage);

#endif
