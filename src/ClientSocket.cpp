#include "ClientSocket.h"

ClientSocket::ClientSocket()
{
    //ctor
    mSocket = -1;
}

ClientSocket::~ClientSocket()
{
    //dtor
}

char ClientSocket::IsConnected()
{
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (mSocket, SOL_SOCKET, SO_ERROR, &error, &len);
    if(mSocket < 0 || retval != 0 || error != 0)
        return 0x00;
    else
        return 0x01;
}

int ClientSocket::Connect(char* hostname, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // 1. Create a socket
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocket < 0)
        return -1;

    // 2. Connect the socket to the address of the server
    server = gethostbyname(hostname);
    if(server == NULL)
        return -1;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    return connect(mSocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
}

int ClientSocket::ReadData(char* data)
{
    int value;
    try
    {
        if(IsConnected() == 0x00)
        {
            std::cout << "CBR Socket not connected" << std::endl;
            Close();
            return -1;
        }
        /*else
            std::cout << "CBR Socket connected" << std::endl;*/
        value = read(mSocket, data, 1024);
    }
    catch(...)
    {
        std::cout << "CBS Catch block in read method" << std::endl;
        value = -1;
    }
    //std::cout << "CBS reading was successful" << std::endl;
    return value;
}

int ClientSocket::SendPacket(Packet& p)
{
    /*if(mSocket < 0)
    {
        std::cout << "No connection." << std::endl;
        return -1;
    }*/
    int value;
    try
    {
        if(IsConnected() == 0x00)
        {
            std::cout << "CBS Socket not connected" << std::endl;
            Close();
            return -1;
        }
        /*else
            std::cout << "CBS Socket connected" << std::endl;*/
        //value = write(mSocket, p.data, p.length);
        //std::cout << "CBS before sending" << std::endl;
        value = send(mSocket, p.data, p.length, 0);
        //std::cout << "CBS after sending" << std::endl;
    }
    catch(...)
    {
        std::cout << "CBS Catch block in send method" << std::endl;
        value = -1;
    }
    //std::cout << "CBS sending was successful" << std::endl;
    return value;
}

void ClientSocket::Close()
{
    //std::cout << "Closing socket..." << std::endl;
    // if?
    close(mSocket);
    //mSocket = -1;
    //std::cout << "Done closing socket!" << std::endl;
}
