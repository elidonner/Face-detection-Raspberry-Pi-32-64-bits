/*
	UART communication on Raspberry Pi using C (pigpio Library)
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>
using namespace std;
int main ()
{
  int serial_port ;
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

  //if(serWriteByte(serial_port, 1))
  //{
    //cout<<"Failed write";
  //}

  char msg[] = "<hello world>";
  if(serWrite(serial_port, msg, 13))
  {
    cout<<"Failed write";
  }

  while(1)
  {

    if(serDataAvailable (serial_port) )
	{
		dat = serReadByte(serial_port);		/* receive character serially*/
		printf("%c", dat) ;
    }
  }

}
