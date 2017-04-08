#include "CarControl.h"

CarControl::CarControl() : mMotorLeft(MOTOR_LEFT), mMotorRight(MOTOR_RIGHT), mMotorSteering(MOTOR_STEERING)
{
    //ctor
}

CarControl::~CarControl()
{
    //dtor
}

// Steer the car
void CarControl::Steer(SteerDirection sd){
    if(sd == left){
        mMotorSteering.set_speed_sp(MOTORSPEED_STEERING);
        mMotorSteering.run_forever();
    } else if(sd == right){
        mMotorSteering.set_speed_sp(-MOTORSPEED_STEERING);
        mMotorSteering.run_forever();
    } else {
        mMotorSteering.stop();
    }
}

// Move the Car
void CarControl::Drive(DriveDirection dd){
    //int mDriveSpeed = (1050 * speedInProcent) / 100;
    if(dd == forwards){
        mMotorLeft.set_speed_sp(-mDriveSpeed);
        mMotorRight.set_speed_sp(-mDriveSpeed);
        mMotorLeft.run_forever();
        mMotorRight.run_forever();
    }else if(dd == backwards){
        mMotorLeft.set_speed_sp(mDriveSpeed);
        mMotorRight.set_speed_sp(mDriveSpeed);
        mMotorLeft.run_forever();
        mMotorRight.run_forever();
    }else{
        mMotorLeft.stop();
        mMotorRight.stop();
    }
}

// Set the drive speed by triggervalue (0-255)
void CarControl::SetDriveSpeedByTriggerValue(char triggerValue){
    float procent = (triggerValue * 100) / 255;
    mDriveSpeed = (procent * 1050) / 100;
}

// Returns the current driving speed in kilometers per hour
float CarControl::GetCurrentSpeed(){
    return 3.21;
}
