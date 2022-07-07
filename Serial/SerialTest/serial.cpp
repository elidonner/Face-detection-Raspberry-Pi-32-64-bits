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


int recvBytesWithStartEndMarkers() {
    static bool recvInProgress = false;
    static uint8_t ndx = 0;
    uint8_t startMarker = 0x3C; //this is the start marker <
    uint8_t endMarker = 0x3E; //this is the start marker >
    uint8_t rb;
   
    while (serDataAvailable () > 0 && newData == false) {
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
                break;
            }
        }

        else if (rb == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    Serial.print("This just in (HEX values)... ");
    for (uint8_t n = 0; n < numReceived; n++) {
      printf("%c", receivedBytes[n]) ;
    }
    Serial.println();
}


int main ()
{

  int serial_port, writeFlag;
  char dat;
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

  char msg[] = "<hello world>";

  while(1)
  {
    if(serDataAvailable (serial_port) )
	  {
      writeFlag = recvBytesWithStartEndMarkers();
      dat = serReadByte(serial_port);		/* receive character serially*/
    }
    if 
    (

    )
  }

}
