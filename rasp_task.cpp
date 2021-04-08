/**********************************************************
Raspberry como Mestre SPI
Compilar:
g++ -Wall -pthread -o rasp_task rasp_task.cpp -lpigpio -lrt
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <cstring>
#include <pigpio.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <iostream>
#include <string.h>
#include <termios.h>
#include <errno.h>

#define HS 24
using namespace std;
int fd;
char result2[19];
char rxDat[14];
double Temp[13] = {-18.4, -17.6, -15.3, -5.0, 6.5, 7.3, 7.6, 8.2, 9.4, 10.0, 11.1, 12.4, 13.8};

//buffer para transmissão RS232 para PG201 e buffer para receber valores de pressão e outros dados
char buf[] = "b[,][TAB]x.xxxxE±xx";
char buftx_serial[] = "RPV[a]<CR>"; //ler pressão nos canais	
char bufrx_serial[100];

struct termios options;

/**********************************************************
spiTxRx
 Transmite 1 byte via SPI e retorna 1 byte
 como resultado.
***********************************************************/

char* spiTxRx(char* txDat)
{
	struct spi_ioc_transfer spi;

	memset (&spi, 0, sizeof (spi));

	spi.tx_buf        = (unsigned long)txDat;
	spi.rx_buf        = (unsigned long)&rxDat;
	spi.len           = 14;
	//spi.delay_usecs = 100; //100
	//spi.speed_hz  = 500000;
	spi.bits_per_word = 8;  
	ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
	cout << txDat;
	return rxDat;
}

int init_gpio(void){
	if (gpioInitialise() < 0)
		return -1;
		
	gpioSetMode(HS, PI_INPUT);
	gpioSetPullUpDown(HS, PI_PUD_UP);
	return 1;
}


/***********************************************************
init RS232 interface
***********************************************************/
int init_serial(int file){
    
    	tcgetattr(file, &options);
    
    	cfsetspeed(&options,B9600); //seta baudrate entrada e saída (B9600 B19200 B38400->pg201)
    	cfmakeraw(&options); //simplifica configurações em raw mode comentadas abaixo
    	//options.c_cflag &= ~CSIZE;  //operação AND com máscara para zerar bit de dados 
    	//options.c_cflag |= CS8;   // operação OR para setar a flag com tamanho 8 bits (pg201)
    	//options.c_cflag &= ~PARENB; // sem bit de pariedade (pg201)
    	//options.c_cflag |= PARODD; //pariedade impar 
    	//options.c_cflag &= ~PARODD; //pariedade par
    	options.c_cflag &= ~CSTOPB; //1 bit de parada
    	options.c_cflag |= CLOCAL;
    	options.c_cflag |= CREAD;
    	options.c_cc[VTIME]=1;
    	options.c_cc[VMIN]=50;
    	//options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //raw mode sem echo
    	//options.c_oflag |= OPOST;  //output raw (raw mode in device to device transfer)
    	tcsetattr(file, TCSANOW, &options);
	return 1;
}

int write_read_serial(int file, char *transmissao){
	printf("Enviando Comando para ler Pressão");
	printf("\n");

	//int count = write(file, buftx_serial,strlen(buftx_serial));
	int count = write(file, transmissao,strlen(transmissao));
    	gpioDelay(100000);
    	int bytes;
    	ioctl(file, FIONREAD, &bytes);
    	if(bytes!=0){
    		count = read(file, bufrx_serial, 100);
    	}
	
	printf("\nRecebendo Dados da interface por loopback\n");
    	printf("%s\n\r", bufrx_serial);
	
	printf("Caso o PG201 estivesse connectado, a Resposta: %s\n\r", buf);

	close(file);
    	return (EXIT_SUCCESS);
}


/***********************************************************
Main
  Inicia Master spi 1.0 (CS0) para leitura e escrita em 5MHz 
***********************************************************/

int main (void){
	init_gpio();
	
	//Abre arquivo para escrita na interface serial
	system("sudo systemctl stop serial-getty@ttyAMA0.service");
    	int sfd = open("/dev/serial0", O_RDWR | O_NOCTTY); 
    	if (sfd == -1) {
        	printf("Error no is : %d\n", errno);
        	printf("Error description is : %s\n", strerror(errno));
        	return (-1);
    	};
	init_serial(sfd); //configura flags 
	
	//descritor de arquivo para spi com esp
	fd = open("/dev/spidev1.0", O_RDWR); 
	unsigned int speed = 5000000;  //clock spi master
	ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	
	char Temp_string[13][14];
	
   	
	while (1)
   	{
		//Funções de aquisição de temperatura AQUI//
		
		
		// transforma temp em string para envio
		for (int j=0; j<13; j++){ 
			sprintf(&Temp_string[j][14],"%2.1lf",Temp[j]);
			//cout<<&Temp_string[j][14]; //debug
		}
		
		write_read_serial(sfd, buftx_serial);
		
		memset(rxDat, 0xA5, 14); //garante que buffer rx começa vazio
		while(gpioRead(HS)==0);  //espera pino da esp ativar para começar spi
		for (int i = 0; i < 13; i++) 
      		{
         
	 		gpioDelay(10000);    
	 		printf("Enviando Temp[%d]: %lf\n",i,Temp[i]);
			spiTxRx(&Temp_string[i][14]); 
      	 		printf("\n");
         		printf("rx buffer: %s\n", rxDat);
      		}	
      		printf(" delay now\n\n");
		gpioDelay(10000);         //30000
   	}
	
	return 0;
}

