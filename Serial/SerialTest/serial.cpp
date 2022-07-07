/*
	UART communication on Raspberry Pi using C (pigpio Library)
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>
using namespace std;

const uint8_t numBytes = 32;
uint8_t receivedBytes[numBytes];
uint8_t numReceived = 0;
int serial_port, writeFlag;

void showNewData() {
    for (uint8_t n = 0; n < numReceived; n++) {
      printf("%c", receivedBytes[n]);
    }
    fflush(stdout);
}

int recvBytesWithStartEndMarkers() {
    static bool recvInProgress = false;
    static uint8_t ndx = 0;
    uint8_t startMarker = 0x3C; //this is the start marker <
    uint8_t endMarker = 0x3E; //this is the start marker >
    uint8_t rb;

    while (serDataAvailable (serial_port) > 0) {
        rb = serReadByte(serial_port);

        if (recvInProgress == true) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                showNewData();
                return 1;
            }
        }

        else if (rb == startMarker) {
            recvInProgress = true;
        }
    }
    return 0;
}


int main ()
{
  char serialName[] = "/dev/ttyS0";

  if (gpioInitialise() < 0)
  {
    cout<<"faliure";
    }

  if ((serial_port = serOpen (serialName, 115200, 0)) < 0)	/* open serial port */
  {
    cout<<"faliure";
    return 1;
  }

  char msg[] = "<hello esp>";

  while(1)
  {
    if(writeFlag){
        serWrite(serial_port, msg, sizeof(msg));
        writeFlag = 0;
    }
    else if(serDataAvailable (serial_port) )
    {
      writeFlag = recvBytesWithStartEndMarkers();
    }

  }

}
