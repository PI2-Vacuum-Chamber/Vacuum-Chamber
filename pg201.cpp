*/
Compile: g++ -Wall -pthread -o pg201 PG201.cpp -lrt
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>


int loopback_tst(void){
	int sfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY ); 
    	int count;
    	if (sfd == -1){
        	printf ("Error no is : %d\n", errno);
        	printf("Error description is : %s\n",strerror(errno));
        	return(-1);
    	}
    	char buf[]="hello world";
    	char buf2[11];
    	count = write(sfd, buf, 11);
    	count=read(sfd,buf2,11);buf2[11]=0;
    	printf("%s",buf2);
    	close(sfd);
    	return 0;
}


int main(int argc, char** argv){
    	char buf[] = "b[,][TAB]x.xxxxE±xx";
    	char bufp[] = "RPV[a]<CR>"; //ler pressão nos canais	
    	char buf2[100];
    	char c;
    	struct termios options;

    	system("sudo systemctl stop serial-getty@ttyAMA0.service");
    	int sfd = open("/dev/serial0", O_RDWR | O_NOCTTY); 
    	if (sfd == -1) {
        	printf("Error no is : %d\n", errno);
        	printf("Error description is : %s\n", strerror(errno));
        	return (-1);
    	};
    
    	tcgetattr(sfd, &options);
    
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
    	tcsetattr(sfd, TCSANOW, &options);

 
    	int count = write(sfd, bufp,strlen(bufp));
    	usleep(100000);
    	int bytes;
    	ioctl(sfd, FIONREAD, &bytes);
    	if(bytes!=0){
    		count = read(sfd, buf2, 100);
    	}
    		
    	printf("%s\n\r", buf2);
    	printf("Caso o PG201 estivesse connectado e nas condições corretas, a Resposta: %s\n\r", buf);

	close(sfd);
    
    	return (EXIT_SUCCESS);
}

