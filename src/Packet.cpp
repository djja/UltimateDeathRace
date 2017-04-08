#include "Packet.h"

Packet::Packet(unsigned short length, unsigned short type)
{
	this->type = type;
	this->length = length;
	mIsBigEndian = 0x01;                                     // we shouldn't check of endian when writing, the reader should adjust
	WriteUshort(1, 0);
	WriteUshort(length, 2);
	WriteUshort(type, 4);
}

Packet::Packet(char data[])
{
	//this->data = data;
	for(int i=0;i<MAXPACKETSIZE;i++)
    {
		this->data[i] = data[i];
	}
	char byte1 = data[0];
	char byte2 = data[1];
	if(((byte1 << 8) + byte2) == 1)
		mIsBigEndian = 0x01;
	else
		mIsBigEndian = 0x00;
	length = ReadUshort(2);
	type = ReadUshort(4);
}

unsigned short Packet::ReadUshort(int offset)
{
	char byte1 = data[offset];
	char byte2 = data[offset + 1];
	if(mIsBigEndian == 0x01)
		return (byte1 << 8) + byte2;
	else
		return (byte2 << 8) + byte1;
}

void Packet::WriteUshort(unsigned short value, int offset)
{
	if(mIsBigEndian == 0x01)
    {
        data[offset] = ((value >> 8) & 0xFF);
        data[offset + 1] = (value & 0xFF);
	}
	else
    {
		data[offset] = (value & 0xFF);
		data[offset + 1] = ((value >> 8) & 0xFF);
	}
}

unsigned int Packet::ReadUInt(int offset){
    char byte1 = data[offset];
	char byte2 = data[offset + 1];
	char byte3 = data[offset + 2];
	char byte4 = data[offset + 3];
	if(mIsBigEndian == 0x01)
		return (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + byte4;
	else
		return (byte4 << 8) + (byte3 << 8) + (byte2 << 8) + byte1;
}

void Packet::WriteFloat(float value, int offset)
{
    union flobyt_t
    {
        float f;
        char bytes[4];
    }flobyt;
    //flobyt_t flobyt;
    flobyt.f = value;
    if(mIsBigEndian == 0x01)
    {
        data[offset] = flobyt.bytes[0];
        data[offset + 1] = flobyt.bytes[1];
        data[offset + 2] = flobyt.bytes[2];
        data[offset + 3] = flobyt.bytes[3];
	}
	else
    {
		data[offset] = flobyt.bytes[3];
		data[offset + 1] = flobyt.bytes[2];
		data[offset + 2] = flobyt.bytes[1];
		data[offset + 3] = flobyt.bytes[0];
	}
}
