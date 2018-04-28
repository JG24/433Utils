/*
RFSniffer

Usage: ./RFSniffer [<pulseLength>]
[] = optional

Hacked from http://code.google.com/p/rc-switch/
by @justy to provide a handy RF code sniffer
*/

#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <time.h>

RCSwitch mySwitch;

using namespace std;

void wait(int seconds)
{
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {}
}

int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	int PIN = 2;
	
	time_t timer;
	char currentTime[26];
	struct tm* tm_info;
	
	if(wiringPiSetup() == -1) {
		printf("wiringPiSetup failed, exiting...");
		return 0;
	}
	
	std::string fileName = "RFSniffer.log";
	if (argv[1] != NULL) fileName = argv[1];

	int pulseLength = 0;
	if (argv[2] != NULL) pulseLength = atoi(argv[2]);

	mySwitch = RCSwitch();
	if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
	mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2

	while(1) {

		if (mySwitch.available()) {
			
			time(&timer);
			tm_info = localtime(&timer);
			strftime(currentTime, 26, "%Y-%m-%d %H:%M:%S", tm_info);
			
			int value = mySwitch.getReceivedValue();
			
			if (value == 0) {
				printf("Unknown encoding\n");
			} else {
				FILE* pFile = fopen(fileName.c_str(), "a");
				fprintf(pFile, "%s %i\n", currentTime, mySwitch.getReceivedValue());
				fclose(pFile);
				printf("%s %i\n", currentTime, mySwitch.getReceivedValue());
				wait(1);
			}
			
			mySwitch.resetAvailable();

		}

		usleep(1000);

	}

	exit(0);


}
