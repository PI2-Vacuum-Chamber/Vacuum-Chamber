/**********************************************************
Raspberry como Mestre SPI
Compilar:
g++ -Wall -pthread -o PI2 spiPI2.cpp -lpigpio -lrt
***********************************************************/

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <pigpio.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <string.h>

#define HS 24
using namespace std;
int fd;
//unsigned char hello[128] =  "Hello esp";
unsigned char result;
char result2[19];
char rxDat[19];
double Temp[13] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0};
char* spiTxRx(unsigned long txDat);


/**********************************************************
Main
  Setup SPI
    Open file spidev0.0 (chip enable 0) for read/write 
      access with the file descriptor "fd"
    Configure transfer speed (1MkHz)
  Start an endless loop that repeatedly sends the characters
    in the hello[] array to the Ardiuno and displays
    the returned bytes
***********************************************************/

int main (void){

   	if (gpioInitialise() < 0)
		return -1;
	gpioSetMode(HS, PI_INPUT);
	gpioSetPullUpDown(HS, PI_PUD_UP);

	fd = open("/dev/spidev1.0", O_RDWR);

	unsigned int speed = 5000000;  //9000000
	ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	while (1)
   	{
		memset(rxDat, 0xA5, 16);
		while(gpioRead(HS)==0);
		for (unsigned long i = 0; i < 13; i++) 
      		{
         
	 		gpioDelay(10000);    
	 		//result2 = spiTxRx(i);
         		//cout << result2;
	 		spiTxRx(Temp[i]);      
      	 		printf("\n");
         		printf("rx buffer: %s\n", rxDat);
      		}	
      		printf(" delay now\n\n");
		gpioDelay(10000);         //30000
   	}

}

/**********************************************************
spiTxRx
 Transmite 1 byte via SPI e retorna 1 byte
 como resultado.
***********************************************************/

char* spiTxRx(unsigned long txDat)
{
	struct spi_ioc_transfer spi;

	memset (&spi, 0, sizeof (spi));

	spi.tx_buf        = (unsigned long)&txDat;
	spi.rx_buf        = (unsigned long)&rxDat;
	spi.len           = 19;
	//spi.delay_usecs = 100; //100
	//spi.speed_hz  = 500000;
	spi.bits_per_word = 8;  
	ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
	cout << txDat;
	return rxDat;
}

