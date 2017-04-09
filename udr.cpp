#include "CarControl.h"
#include "ClientSocketUDP.h"
#include <thread>
#include <iostream> // for temp debugging delete me

// "echo \"tacho-motor\" > /sys/class/lego-port/port6/mode" run this command on the ev3 before the game starts, for the laser

CarControl mCarControl;
ClientSocketUDP mClientSocket;

bool mPassedFinishLine, mOnFinishLine;
char mCollectedMiniguns, mCollectedSpeedUps, mCollectedSlowDowns, mCurrentPowerUp, mPowerUpAmmo;


void HandlePacket(Packet& packet)
{
    /*int i;
        for(i=0;i<(sizeof(packet)/sizeof(char));i++)
            std::cout << (int)packet.data[i] << " ";
        std::cout << std::endl;*/
    if(packet.type == PT_CONTROLLER_BUTTON_STATE)
    {
        // Button and trigger handling for driving and steering
        PckControllerButtonState p(packet.data);
        if(p.GetButtonState(p.dpadLeft))
			mCarControl.Steer(mCarControl.right);
		else if(p.GetButtonState(p.dpadRight))
			mCarControl.Steer(mCarControl.left);
		else
			mCarControl.Steer(mCarControl.none);

        if(p.GetTriggerValues(p.right) > 10){
            mCarControl.SetDriveSpeedByTriggerValue(p.GetTriggerValues(p.right));
            mCarControl.Drive(mCarControl.forwards);
        }else if(p.GetTriggerValues(p.left) > 10){
            mCarControl.SetDriveSpeedByTriggerValue(p.GetTriggerValues(p.left));
            mCarControl.Drive(mCarControl.backwards);
        }else
            mCarControl.Drive(mCarControl.stop);

        // Button handling for power ups
        if(mCurrentPowerUp != 0){
            if(p.GetButtonState(p.a)){          // Button A to use power up

            }else if(p.GetButtonState(p.b)){    // Button B to ditch power up
                mCurrentPowerUp = 0;
            }
        }
    }
    else if(packet.type == PT_RESET_FINISHLINE_VAR) // Now we know that the server knows that we've passed the finish line
        mPassedFinishLine = false;
}

void Thread_ReceiveInfo()
{
    const int maxBufLength = 256;
    char readBuffer[maxBufLength];
    int n;
    while(1){   // Fix this
        n = mClientSocket.ReceiveDataFrom(readBuffer, maxBufLength);
        if(n < 1){
            std::cout << "n < 1" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Packet p(readBuffer);
        if(p.length != n)
            std::cout << "n and packet.length don't match" << std::endl;
        else
            HandlePacket(p);
    }
	std::cout << "Thread_ReceiveInfo exited..." << std::endl;
}

void Thread_SendInfo(){
    while(1){
        PckCarData p(mCurrentPowerUp, mPowerUpAmmo, mCarControl.GetCurrentSpeed(), mPassedFinishLine, mCollectedMiniguns, mCollectedSpeedUps, mCollectedSlowDowns);
        mClientSocket.SendData(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void UsePowerUp(int powerUp){

}

int main(){
    mCollectedMiniguns = 0;
    mCollectedSpeedUps = 0;
    mCollectedSlowDowns = 0;
    mCurrentPowerUp = 0;
    mPowerUpAmmo = 0;
    mPassedFinishLine = false;
    mOnFinishLine = false;
    // Initialize socket
    if(mClientSocket.CreateSocket() != 0)
        return 1;
    // Socket created, start listening and sending
    std::thread t1(Thread_ReceiveInfo);
	t1.detach();
	std::thread t2(Thread_SendInfo);
	t2.detach();

    bool up = false, down = false, left = false, right = false, enter = false,
	escape = false;

    while (escape == 0)
    {
		up = ev3dev::button::up.pressed ();
		down = ev3dev::button::down.pressed ();
		left = ev3dev::button::left.pressed ();
		right = ev3dev::button::right.pressed ();
		enter = ev3dev::button::enter.pressed ();
		escape = ev3dev::button::back.pressed ();

		if(enter){
            /*Packet p(HEADERLENGTH, 99);
            int n;
            if((n = mClientSocket.SendData(p)) == HEADERLENGTH)
                std::cout << "Full packet sent!" << std::endl;
            else
                std::cout << n << "/" << HEADERLENGTH << " bytes sent..." << std::endl;*/
            std::cout << "Finish line set to true" << std::endl;
            //mPassedFinishLine = true;
            //std::this_thread::sleep_for(std::chrono::seconds(2));
            ev3dev::sound::play("~/drop/ENGINE3.WAV", true);
            ev3dev::sound::play("~/drop/FlaskGone.wav", true);
            std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		if(left)
		{
			mCarControl.FireMinigun(true);
		}
		else if(right)
		{
		    std::cout << "Turning laser off" << std::endl;
			mCarControl.FireMinigun(false);
		}
		/*else
			c.Steer(c.none);

        if(up)
            c.Drive(c.forwards);
        else if(down)
            c.Drive(c.backwards);
        else
            c.Drive(c.stop);*/

        int color_finishLine = mCarControl.red;

        int colorLeft = mCarControl.GetColor(mCarControl.leftSensor);
        int colorRight = mCarControl.GetColor(mCarControl.rightSensor);
        // Check for finish line
        if((colorLeft == color_finishLine || colorRight == color_finishLine) && !mOnFinishLine){
            mOnFinishLine = true;
            mPassedFinishLine = true;
        }else if(colorLeft != color_finishLine && colorRight != color_finishLine)  // We are not on the finish line anymore
            mOnFinishLine = false;  // The other var, mPassedFinishLine, is reset by the server application

        int powerUpColor_minigun = mCarControl.blue;
        int powerUpColor_speedUp = mCarControl.green;
        int powerUpColor_slowDown = mCarControl.yellow;
        // Check for power ups
        if(mCurrentPowerUp == 0){   // Only check if we don't have a power up already
            if(colorLeft == powerUpColor_minigun || colorRight == powerUpColor_minigun){
                mCollectedMiniguns++;
                mCurrentPowerUp = 1;
            }else if(colorLeft == powerUpColor_speedUp || colorRight == powerUpColor_speedUp){
                mCollectedSpeedUps++;
                mCurrentPowerUp = 2;
            }else if(colorLeft == powerUpColor_slowDown || colorRight == powerUpColor_slowDown){
                // Slow down, this powerup is activated immediately
                mCollectedSlowDowns++;
                mCurrentPowerUp = 3;
            }
        }

        // Check if player is hit

		//printf ("up:%d down:%d left:%d right:%d enter:%d esc:%d\n", up, down, left, right, enter, escape);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    // Close the socket
    mClientSocket.Close();
}

