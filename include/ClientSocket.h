#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <iostream> // for the couts

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "PacketTypes.h"

class ClientSocket
{
    public:
        int mSocket;

        ClientSocket();
        virtual ~ClientSocket();

        char IsConnected();
        int Connect(char*, int);
        int ReadData(char*);
        int SendPacket(Packet&);
        void Close();
    protected:

    private:
};

#endif // CLIENTSOCKET_H
