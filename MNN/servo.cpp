#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>
#include "servo.hpp"

/*
# servo_demo.c
# 2016-10-08
# Public Domain

gcc -Wall -pthread -o servo_demo servo_demo.c -lpigpio

sudo ./servo_demo          # Send servo pulses to GPIO 4.
sudo ./servo_demo 23 24 25 # Send servo pulses to GPIO 23, 24, 25.
*/


Servo::Servo(int limit,
            int min_width,
            int max_width,
            int pulseWidth)
        : _limit(limit),
        _min_width(min_width),
        _max_width(max_width),
        _pulseWidth(pulseWidth)
{}

int Servo::moveToAngle(int angle, int pin)
{
   //initialize local variables
   int step, pulseWidth, notThere;

   //Start the servo
   if (start() == -1) return 0;

   //Convert the angle to a pulseWidth target
   angle = convertAngle(angle);

   //set the pulseWidth to our current location
   pulseWidth = _pulseWidth;



   notThere = 1;
   while(notThere)
   {

      gpioServo(pin, pulseWidth);
      pulseWidth += step;

      if (pulseWidth == angle)
      {
         notThere = 0;
      }

      // TODO: make faster, get rid of delay
      // currently this helps make it not jerky

      time_sleep(0.01);
   }

   //reset instance variable to remember where we are
   _pulseWidth = pulseWidth;

   //tidy up the gpio
   gpioServo(pin, 0);
   gpioTerminate();

   return 0;

}

int Servo::start()
{
   //not sure what these lines do but we need them
   if (gpioInitialise() < 0) return -1;
   //gpioSetSignalFunc(SIGINT, stop);
   return 1;
}


int Servo::convertAngle(int angle)
{
   float angle_convert;
   int pulseWidth;

   if(angle<0){
      angle = 0;
   }
   else if (angle > 270){
      angle = 270;
   }

   angle_convert = angle/270.0;

   pulseWidth = angle_convert*(_max_width-_min_width);

   return pulseWidth;
}
