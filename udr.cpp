#include "CarControl.h"
#include "ClientSocketUDP.h"
#include <thread>
#include <iostream> // for temp debugging delete me

CarControl mCarControl;
ClientSocketUDP mClientSocket;

bool mPassedFinishLine;
char mCollectedMiniguns, mCollectedSpeedUps, mCollectedSlowDowns, mCurrentPowerUp, mPowerUpAmmo;


void HandlePacket(Packet& packet)
{
    /*int i;
        for(i=0;i<(sizeof(packet)/sizeof(char));i++)
            std::cout << (int)packet.data[i] << " ";
        std::cout << std::endl;*/
    if(packet.type == PT_CONTROLLER_BUTTON_STATE)
    {
        PckControllerButtonState p(packet.data);
        if(p.GetButtonState(p.dpadLeft))
			mCarControl.Steer(mCarControl.right);
		else if(p.GetButtonState(p.dpadRight))
			mCarControl.Steer(mCarControl.left);
		else
			mCarControl.Steer(mCarControl.none);


        //std::cout << "Left: " << (int)p.data[HEADERLENGTH + 4] << " Right: " << (int)p.data[HEADERLENGTH + 5] << std::endl;
        //std::cout << "Left: " << (int)p.GetTriggerValues(p.left) << " Right: " << (int)p.GetTriggerValues(p.right) << std::endl;
        if(p.GetTriggerValues(p.right) > 10){
            mCarControl.SetDriveSpeedByTriggerValue(p.GetTriggerValues(p.right));
            mCarControl.Drive(mCarControl.forwards);
        }else if(p.GetTriggerValues(p.left) > 10){
            mCarControl.SetDriveSpeedByTriggerValue(p.GetTriggerValues(p.left));
            mCarControl.Drive(mCarControl.backwards);
        }else
            mCarControl.Drive(mCarControl.stop);
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

int main(){
    mCollectedMiniguns = 0;
    mCollectedSpeedUps = 0;
    mCollectedSlowDowns = 0;
    mCurrentPowerUp = 0;
    mPowerUpAmmo = 0;
    mPassedFinishLine = false;
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
            mPassedFinishLine = true;
            std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		/*if(left)
		{
			c.Steer(c.left);
		}
		else if(right)
		{
			c.Steer(c.right);
		}
		else
			c.Steer(c.none);

        if(up)
            c.Drive(c.forwards);
        else if(down)
            c.Drive(c.backwards);
        else
            c.Drive(c.stop);*/

		//printf ("up:%d down:%d left:%d right:%d enter:%d esc:%d\n", up, down, left, right, enter, escape);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close the socket
    mClientSocket.Close();
}

