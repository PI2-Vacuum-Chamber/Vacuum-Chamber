
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int lclconf[16] = {0};

const char *get_data(){
	time_t rawtime;
  	struct tm *timeinfo;
  	char buffer[18];

  	time (&rawtime);
  	timeinfo = localtime (&rawtime);

  	strftime (buffer,18,"%x %R",timeinfo);
  	//puts (buffer);
	printf(strlen(buffer));
	return buffer;
}

void get_temp(){
	int temp[16] = {0};

	for(int channel = 0; channel <=16; channel++){
		sleep(100);
		if (channel == 16)
			//MAX31856GetCJtemp(&ThermocoupleBreakout);
			printf("coldj");
		else {
			//set_mux_address(channel);
			//MAX31856GetTemperature(&ThermocoupleBreakout); 
		}
	}
}

int main(){
 	get_temp();
	//printf("%s",get_data());

	return 0;
}