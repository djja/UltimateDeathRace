#ifndef PCKCONTROLLERBUTTONSTATE_H
#define PCKCONTROLLERBUTTONSTATE_H

#include "Packet.h"

/*
    Xbox360 controller byte array layout
    0   - Controller buttons
    1   -
    2   - Trigger left
    3   - Trigger right
    4   - Left thumbstick X
    5   -
    6   - Left thumbstick Y
    7   -
*/

class PckControllerButtonState : public Packet
{
    public:
        enum ControllerButton{
            dpadUp,
            dpadDown,
            dpadLeft,
            dpadRight,
            y,
            b,
            a,
            x,
            shoulderLeft,
            shoulderRight
        };
        enum ControllerTrigger{
            left,
            right
        };

        PckControllerButtonState(char[]);
        bool GetButtonState(ControllerButton);
        char GetTriggerValues(ControllerTrigger);
    protected:

    private:
        unsigned int mButtonState;
};

#endif // PCKCONTROLLERBUTTONSTATE_H
