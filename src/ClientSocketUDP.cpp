#include "ClientSocketUDP.h"

ClientSocketUDP::ClientSocketUDP()
{
    //ctor
}

ClientSocketUDP::~ClientSocketUDP()
{
    //dtor
}

int ClientSocketUDP::CreateSocket(){
    struct addrinfo hints;
    int rv;

    memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(SERVERADDRESS, SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

    return 0;
}

int ClientSocketUDP::SendData(Packet& pck){
    return sendto(sockfd, pck.data, pck.length, 0, p->ai_addr, p->ai_addrlen);
}

int ClientSocketUDP::ReceiveDataFrom(char buffer[], int bufferLength){
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;   // I don't care about the server's ip, can I throw in NULL? :)
    return recvfrom(sockfd, buffer, bufferLength, 0, (struct sockaddr *)&their_addr, &addr_len);
}

void ClientSocketUDP::Close(){
    freeaddrinfo(servinfo);
    close(sockfd);
}
