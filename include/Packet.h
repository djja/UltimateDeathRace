#ifndef PACKET_H
#define PACKET_H

#define HEADERLENGTH 6	// Bytes
#define MAXPACKETSIZE 1024

#define PT_CONTROLLER_BUTTON_STATE 0
#define PT_CAR_DATA 1
#define PT_RESET_FINISHLINE_VAR 2
#define PT_RESET_GAME_LOGIC 3

class Packet
{
    public:
        char data[MAXPACKETSIZE];

        unsigned short length, type;

        Packet(unsigned short, unsigned short);
        Packet(char[]);

    protected:
        char mIsBigEndian;

        unsigned short ReadUshort(int);
        void WriteUshort(unsigned short, int);
        unsigned int ReadUInt(int);
        void WriteFloat(float, int);

    private:
};

#endif // PACKET_H
