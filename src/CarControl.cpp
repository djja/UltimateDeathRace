#include "CarControl.h"

CarControl::CarControl() : mMotorLeft(MOTOR_LEFT), mMotorRight(MOTOR_RIGHT), mMotorSteering(MOTOR_STEERING), mColorSensorLeft(COLORSENSOR_LEFT), mColorSensorRight(COLORSENSOR_RIGHT), mColorSensorBack(COLORSENSOR_BACK), mMotorMinigun(MOTOR_MINIGUN)
{
    //ctor
    //mMotorMinigun.set_speed_sp(mMotorMinigun.max_speed());
    //std::system("echo \"tacho-motor\" > /sys/class/lego-port/port6/mode");    // port6 is outC, this is our laser
    SetMaxSpeed_normal();
    mMotorMinigun.set_stop_action("brake");
    mMotorMinigun.set_speed_sp(mMotorMinigun.max_speed());
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
    mDriveSpeed = (procent * mCurrentMaxDriveSpeed) / 100;
}

// Returns the current driving speed in kilometers per hour
float CarControl::GetCurrentSpeed(){
    float averageSpeed = (mMotorLeft.speed() + mMotorRight.speed()) / 2;
    float rps = averageSpeed / 360;
    float peripheryWheel = (M_PI * WHEEL_DIAMETER) / 1000;   // in meters
    return -3.6 * (rps * peripheryWheel);
}

int CarControl::GetColor(ColorSensorId id){
    if(id == leftSensor)
        return mColorSensorLeft.color();
    else if(id == rightSensor)
        return mColorSensorRight.color();
    else return mColorSensorBack.color();
}

int CarControl::IsCarHit(){
    return mColorSensorBack.ambient_light_intensity() > 30 ? true : false;
    //return mColorSensorBack.color() > 90 ? true : false;
}

void CarControl::FireMinigun(bool value){

    //std::cout << mMotorMinigun.stop_action() << std::endl;

    if(value){
        //mMotorMinigun.set_speed_sp(mMotorMinigun.max_speed());
        mMotorMinigun.run_forever();
    }else{
        //mMotorMinigun.set_speed_sp(0);
        mMotorMinigun.stop();
    }
}
