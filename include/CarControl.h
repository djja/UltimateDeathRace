#ifndef CARCONTROL_H
#define CARCONTROL_H

#include <cmath>    // For pi

// These port defines only work for the EV3 brick not with the pi brick
#define MOTOR_LEFT "outA"
#define MOTOR_RIGHT "outD"
#define MOTOR_STEERING "outB"
#define MOTOR_MINIGUN "outC"

#define COLORSENSOR_LEFT "in1"
#define COLORSENSOR_RIGHT "in4"
#define COLORSENSOR_BACK "in3"

#define MOTORSPEED_STEERING 400
#define MOTORSPEED_DRIVING_MAX 1050

#define WHEEL_DIAMETER 55   // In millimeter

#include "ev3dev.h"

//#include <iostream>     // Delete me!!!!!!!!!!

class CarControl
{
    public:
        int mDriveSpeed;    // Can't this be private?
        int mCurrentMaxDriveSpeed;

        CarControl();
        virtual ~CarControl();

        enum SteerDirection{
            left,
            right,
            none
        };
        enum DriveDirection{
            forwards,
            backwards,
            stop
        };
        enum ColorSensorId{
            leftSensor,
            rightSensor,
            backSensor
        };
        enum Color{
            noColor,
            black,
            blue,
            green,
            yellow,
            red,
            white,
            brown
        };
        void Steer(SteerDirection);
        void Drive(DriveDirection);
        void SetDriveSpeedByTriggerValue(char);
        void FireMinigun(bool);

        float GetCurrentSpeed();
        int GetColor(ColorSensorId);

    protected:

    private:
        ev3dev::large_motor mMotorLeft, mMotorRight, mMotorMinigun;
        ev3dev::medium_motor mMotorSteering;
        ev3dev::color_sensor mColorSensorLeft, mColorSensorRight, mColorSensorBack;
        //ev3dev::dc_motor mMotorMinigun;
};

#endif // CARCONTROL_H
