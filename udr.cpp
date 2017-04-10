#include "CarControl.h"
#include "ClientSocketUDP.h"
#include <thread>
#include <iostream> // for temp debugging delete me

// "echo \"tacho-motor\" > /sys/class/lego-port/port6/mode" run this command on the ev3 before the game starts, for the laser

CarControl mCarControl;
ClientSocketUDP mClientSocket;

bool mPassedFinishLine, mOnFinishLine, mIsSlowedByHit, mUsingPowerUp, mProgramRunning, mGameStarted, mPassedCheckpoint;    // Gamestarted var is used to reset the gamelogic vars
char mCollectedMiniguns, mCollectedSpeedUps, mCollectedSlowDowns, mCurrentPowerUp, mPowerUpAmmo;

void ResetGameLogic(){
    mCollectedMiniguns = 0;
    mCollectedSpeedUps = 0;
    mCollectedSlowDowns = 0;
    mCurrentPowerUp = 0;
    mPowerUpAmmo = 0;
    mPassedFinishLine = false;
    mOnFinishLine = false;
    mIsSlowedByHit = false;
    mUsingPowerUp = false;
    mCarControl.FireMinigun(false);
    mCarControl.SetMaxSpeed_normal();
    mCarControl.Drive(mCarControl.stop);
    mCarControl.Steer(mCarControl.none);
    mGameStarted = false;
    mPassedCheckpoint = true;
}

void UsePowerUp(int powerUp){
    if(powerUp == 1){   // Minigun
        std::thread t([&] () {
            mUsingPowerUp = true;
            //int bullets = 10;
            //while(bullets > 0){
                mCarControl.FireMinigun(true);
                // Play sound
                //ev3dev::sound::play("~/drop/dsplasma.wav", true);
                // Turn laser on

                // wait
                std::this_thread::sleep_for(std::chrono::seconds(10));
                // Turn laser off
                mCarControl.FireMinigun(false);
                // wait

                // decrement bullets
                //ev3dev::sound::play("~/drop/dsplasma.wav", true);
                //std::this_thread::sleep_for(std::chrono::milliseconds(20));
                //bullets--;
            //}
            mUsingPowerUp = false;
            mCurrentPowerUp = 0;
        });
        t.detach();
    }else if(powerUp == 2 && !mIsSlowedByHit){    // Boost
        std::thread t([&] () {
            mUsingPowerUp = true;
            //std::cout << "FASTER" << std::endl;
            mCarControl.SetMaxSpeed_boost();
            std::this_thread::sleep_for(std::chrono::seconds(5));
            mCarControl.SetMaxSpeed_normal();
            //std::cout << "NORMAL" << std::endl;
            mUsingPowerUp = false;
            mCurrentPowerUp = 0;
        });
        t.detach();
    }else if(powerUp == 3 && !mIsSlowedByHit){ // Slow down
        std::thread t([&] () {
            mUsingPowerUp = true;
            //std::cout << "SLOWER" << std::endl;
            mCarControl.SetMaxSpeed_slow();
            std::this_thread::sleep_for(std::chrono::seconds(5));
            mCarControl.SetMaxSpeed_normal();
            //std::cout << "NORMAL" << std::endl;
            mUsingPowerUp = false;
            mCurrentPowerUp = 0;
        });
        t.detach();
    }
}

void HandlePacket(Packet& packet)
{
    /*int i;
        for(i=0;i<(sizeof(packet)/sizeof(char));i++)
            std::cout << (int)packet.data[i] << " ";
        std::cout << std::endl;*/
    if(packet.type == PT_CONTROLLER_BUTTON_STATE)
    {
        mGameStarted = true;    // When we get input from the controller, we know the game has started
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
        if(mCurrentPowerUp != 0 && mCurrentPowerUp != 3){
            if(p.GetButtonState(p.a)){          // Button A to use power up
                UsePowerUp(mCurrentPowerUp);
            }else if(p.GetButtonState(p.b)){    // Button B to ditch power up
                mCurrentPowerUp = 0;
            }
        }
    }
    else if(packet.type == PT_RESET_FINISHLINE_VAR) // Now we know that the server knows that we've passed the finish line
        mPassedFinishLine = false;
    else if(packet.type == PT_RESET_GAME_LOGIC)
        ResetGameLogic();
}

void Thread_ReceiveInfo()
{
    const int maxBufLength = 256;
    char readBuffer[maxBufLength];
    int n;
    while(mProgramRunning){   // Fix this
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
    while(mProgramRunning){
        PckCarData p(mCurrentPowerUp, mPowerUpAmmo, mCarControl.GetCurrentSpeed(), mPassedFinishLine, mCollectedMiniguns, mCollectedSpeedUps, mCollectedSlowDowns, mGameStarted);
        mClientSocket.SendData(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(){
    mProgramRunning = true;
    mCollectedMiniguns = 0;
    mCollectedSpeedUps = 0;
    mCollectedSlowDowns = 0;
    mCurrentPowerUp = 0;
    mPowerUpAmmo = 0;
    mPassedFinishLine = false;
    mOnFinishLine = false;
    mIsSlowedByHit = false;
    mUsingPowerUp = false;
    mGameStarted = false;
    mPassedCheckpoint = true;
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

	int color_finishLine = mCarControl.white;
    int color_checkpoint = mCarControl.black;
    int powerUpColor_minigun = mCarControl.blue;
    int powerUpColor_speedUp = mCarControl.green;
    int powerUpColor_slowDown = mCarControl.red;

    while (escape == 0)
    {
		up = ev3dev::button::up.pressed ();
		down = ev3dev::button::down.pressed ();
		left = ev3dev::button::left.pressed ();
		right = ev3dev::button::right.pressed ();
		enter = ev3dev::button::enter.pressed ();
		escape = ev3dev::button::back.pressed ();

        int colorLeft = mCarControl.GetColor(mCarControl.leftSensor);
        int colorRight = mCarControl.GetColor(mCarControl.rightSensor);

        // Check for checkpoint line
        if((colorLeft == color_checkpoint || colorRight == color_checkpoint) && !mOnFinishLine){    // Should check if both sensors have seen the line
            mOnFinishLine = true;
            if(!mPassedCheckpoint){
                ev3dev::sound::play("~/drop/CHKPOINT.WAV", false);
                mPassedCheckpoint = true;
            }else{
                // Player should drive over finish line first
                ev3dev::sound::play("~/drop/WRONGCP.WAV", false);
            }

        }else if(colorLeft != color_checkpoint && colorRight != color_checkpoint)
            mOnFinishLine = false;

        // Check for finish line
        if((colorLeft == color_finishLine || colorRight == color_finishLine) && !mOnFinishLine){
            mOnFinishLine = true;
            if(mPassedCheckpoint){
                ev3dev::sound::play("~/drop/CHKPOINT.WAV", false);
                mPassedFinishLine = true;
                mPassedCheckpoint = false;
            }else{
                // Player should drive over checkpoint first
                ev3dev::sound::play("~/drop/WRONGCP.WAV", false);
            }
        }else if(colorLeft != color_finishLine && colorRight != color_finishLine)  // We are not on the finish line anymore
            mOnFinishLine = false;  // The other var, mPassedFinishLine, is reset by the server application


        // Check for power ups
        if(mCurrentPowerUp == 0){   // Only check if we don't have a power up already
            if(colorLeft == powerUpColor_minigun || colorRight == powerUpColor_minigun){
                ev3dev::sound::play("~/drop/FlaskGone.wav", false);
                mCollectedMiniguns++;
                mCurrentPowerUp = 1;
            }else if(colorLeft == powerUpColor_speedUp || colorRight == powerUpColor_speedUp){
                ev3dev::sound::play("~/drop/FlaskGone.wav", false);
                mCollectedSpeedUps++;
                mCurrentPowerUp = 2;
            }else if((colorLeft == powerUpColor_slowDown || colorRight == powerUpColor_slowDown) && !mIsSlowedByHit){
                ev3dev::sound::play("~/drop/FlaskGone.wav", false);
                // Slow down, this powerup is activated immediately
                mCollectedSlowDowns++;
                mCurrentPowerUp = 3;
                UsePowerUp(3);
            }
        }

        // Check if player is hit
        if(mCarControl.IsCarHit()){
            if(!mIsSlowedByHit){
                    std::cout << "I'm hit!" << std::endl;
                mIsSlowedByHit = true;
                std::thread t([&] () {
                              //std::cout << "going slower" << std::endl;
                    mCarControl.SetMaxSpeed_slow();
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    mCarControl.SetMaxSpeed_normal();
                    mIsSlowedByHit = false;
                    //std::cout << "going faster" << std::endl;
                });
                t.detach();
            }
        }

		//printf ("up:%d down:%d left:%d right:%d enter:%d esc:%d\n", up, down, left, right, enter, escape);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    mProgramRunning = false;
    // Close the socket
    mClientSocket.Close();
}

