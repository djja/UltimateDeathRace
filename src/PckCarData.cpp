#include "PckCarData.h"

PckCarData::PckCarData(char currentPowerUp, char powerUpAmmo, float currentSpeed, bool passedFinishLine, char collectedPowerUps_Minigun, char collectedPowerUps_SpeedUp, char collectedPowerUps_SlowDown) : Packet(HEADERLENGTH + 10, PT_CAR_DATA)
{
    data[HEADERLENGTH] = currentPowerUp;
    data[HEADERLENGTH + 1] = powerUpAmmo;
    WriteFloat(currentSpeed, HEADERLENGTH  + 2);
    data[HEADERLENGTH + 6] = passedFinishLine;
    data[HEADERLENGTH + 7] = collectedPowerUps_Minigun;
    data[HEADERLENGTH + 8] = collectedPowerUps_SpeedUp;
    data[HEADERLENGTH + 9] = collectedPowerUps_SlowDown;
}
