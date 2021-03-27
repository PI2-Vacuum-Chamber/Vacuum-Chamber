#include "MAX13856.h"
//compile with gcc -c Temperature.c -o ./Temperature.o ; g++  -o ./Temperature ./Temperature.o -lpigpio -lrt ; sudo ./Temperature

/*
void set_mux_address(int address ){
    address = address & 0b00001111;
	int value = address & ( 1 << 3 );
	printf("value: %d\n", value);
    if (value){
		//GPIO.output(MUX1_EN, GPIO.LOW)
		//GPIO.output(MUX2_EN, GPIO.HIGH)
	}
	else{
		//GPIO.output(MUX1_EN, GPIO.HIGH)
		//GPIO.output(MUX2_EN, GPIO.LOW)
	}
	value = address & ( 1 << 2 );
	if (value)
		//GPIO.output(MUX_A2, GPIO.HIGH); 
	else 
		//GPIO.output(MUX_A2, GPIO.LOW); 
		
	value = address & ( 1 << 1 );
	if (value)
		//GPIO.output(MUX_A1, GPIO.HIGH); 
	else
		//GPIO.output(MUX_A1, GPIO.LOW); 

	value = address & ( 1 << 0 );
	if (value)
		//GPIO.output(MUX_A0, GPIO.HIGH);
	else
		//GPIO.output(MUX_A0, GPIO.LOW);
    
}
*/
char *get_data&time(){
	time_t rawtime;
  	struct tm * timeinfo;
  	char buffer [18];

  	time (&rawtime);
  	timeinfo = localtime (&rawtime);

  	strftime (buffer,18,"%x %R",timeinfo);
  	//puts (buffer);
	return buffer;
}

double get_temp(){
	char temp[16] = {0};
	printf("%d", temp);
}

int main(void)
{
    struct MAX31856_S ThermocoupleBreakout;
    double ValR;
    

    gpioInitialise();
    ThermocoupleBreakout = SetupMAX31856(ThermocoupleBreakoutChipSelectGPIO, MAX31856_ThermocoupleType_T, MAX31856_Unit_Celcius);
    // Change any other setting here //
    ThermocoupleBreakout.AveragingMode = MAX31856_AveragingMode_2;
    MAX31856_CombineSettingsAndSend(&ThermocoupleBreakout);

    while(1)
    {
        usleep(500000);

        ValR = MAX31856GetTemperature(&ThermocoupleBreakout);

        printf("%.2lfC\n", ValR);
    }

    MAX31856_Stop(ThermocoupleBreakout);

    return 0;
}

while (1){
			int display_interval = 10, log_interval = 30;
            bool nextacqflag = false, nextlogflag = false;

            currentepochtime = time.time()
			nextacq = str( round( abs( nextacqtime - currentepochtime ) , 0 ) )
			nextlog = str( round( abs( nextlogtime - currentepochtime ) , 0 ) )

			if ( nextacqflag == False ):
				nextacqtime = currentepochtime + display_interval
				nextacqflag = True
			else:
				if ( currentepochtime > nextacqtime ):
					nextacq = "Aquiring"
					nextlog = "Hold"
					output_to_screen( window, arraytemp, nextacq, nextlog )
					arraytemp = acquire_temperatures( spi )	
					nextacqflag = False

			if ( logoutput == 'ON' ):
				if ( nextlogflag == False ):
					nextlogtime = currentepochtime + logging_interval
					nextlogflag =  True
				else:
					if ( currentepochtime > nextlogtime ):
						nextacq = "Hold"
						nextlog = "Logging"
						output_to_screen( window, arraytemp, nextacq, nextlog )
						arraytemp = acquire_temperatures( spi )	
						output_to_log( window, arraytemp, logfile, logfile_maximum, currentepochtime )
						nextlogflag = False
			else:
				nextlog = "Logging Disabled"

			output_to_screen( window, arraytemp, nextacq, nextlog )

			ch = window.getch()
			if ( (ch == ord('x')) or  (ch == ord('X')) ):
				curses.endwin()
				break

			time.sleep(1)
}