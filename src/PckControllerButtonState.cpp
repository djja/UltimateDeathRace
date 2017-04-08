#include "PckControllerButtonState.h"

PckControllerButtonState::PckControllerButtonState(char buffer[]) : Packet(buffer)
{
    mButtonState = ReadUInt(HEADERLENGTH);
}

bool PckControllerButtonState::GetButtonState(ControllerButton cb){
    unsigned int buttonBit = 0;
    switch(cb){
        case dpadUp:
            buttonBit = 1;
            break;
        case dpadDown:
            buttonBit = 2;
            break;
        case dpadLeft:
            buttonBit = 4;
            break;
        case dpadRight:
            buttonBit = 8;
            break;
        case shoulderLeft:
            buttonBit = 0x100;
            break;
        case shoulderRight:
            buttonBit = 0x200;
            break;
        case a:
            buttonBit = 0x1000;
            break;
        case b:
            buttonBit = 0x2000;
            break;
        case x:
            buttonBit = 0x4000;
            break;
        case y:
            buttonBit = 0x8000;
    }
    bool returnValue = (mButtonState & buttonBit) == buttonBit;
    return returnValue;
}

char PckControllerButtonState::GetTriggerValues(ControllerTrigger ct){
    if(ct == left)
        return data[HEADERLENGTH + 4];
    else
        return data[HEADERLENGTH + 5];
}
