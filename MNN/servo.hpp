#ifndef SERVO_HPP_INCLUDED
#define SERVO_HPP_INCLUDED

class Servo
{

    public:

        /**
        * Constructor
        */
        Servo(int limit,
            int min_width,
            int max_width,
            int pulseWidth);


        /**
        * Move servo to given angle
        */
        int moveToAngle(int angle, int pin);

    protected:

    private:
        int _limit;
        int _min_width;
        int _max_width;
        int _pulseWidth;

        /**
        * initialize a servo
        */
        int start();


        /**
        * convert an angle to pulseWidth angle
        */
        int convertAngle(int angle);
};


#endif // SERVO_HPP_INCLUDED
