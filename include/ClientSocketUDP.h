#ifndef CLIENTSOCKETUDP_H
#define CLIENTSOCKETUDP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "PacketTypes.h"

#define SERVERADDRESS "10.1.10.100"
#define SERVERPORT "45621"

class ClientSocketUDP
{
    public:
        ClientSocketUDP();
        virtual ~ClientSocketUDP();
        int CreateSocket();
        int SendData(Packet&);
        int ReceiveDataFrom(char[], int);
        void Close();
    protected:

    private:
        int sockfd;
        struct addrinfo *p, *servinfo;
        //int numbytes;
};

#endif // CLIENTSOCKETUDP_H
