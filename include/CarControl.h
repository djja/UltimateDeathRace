#ifndef CARCONTROL_H
#define CARCONTROL_H

// These port defines only work for the EV3 brick not with the pi brick
#define MOTOR_LEFT "outA"
#define MOTOR_RIGHT "outD"
#define MOTOR_STEERING "outB"

#define MOTORSPEED_STEERING 300

#include "ev3dev.h"

class CarControl
{
    public:
        int mDriveSpeed;

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
        void Steer(SteerDirection);
        void Drive(DriveDirection);
        void SetDriveSpeedByTriggerValue(char);

        float GetCurrentSpeed();

    protected:

    private:
        ev3dev::large_motor mMotorLeft, mMotorRight;
        ev3dev::medium_motor mMotorSteering;
};

#endif // CARCONTROL_H
