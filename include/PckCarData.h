#ifndef PCKCARDATA_H
#define PCKCARDATA_H

#include "Packet.h"

class PckCarData : public Packet
{
    public:
        PckCarData(char, char, float, bool, char, char, char);

    protected:

    private:
};

#endif // PCKCARDATA_H
