#pragma once
#include <WinSock2.h>


#define PACKET_SIZE 100000

class WinAPINetworkServices
{
public:
   static int sendMessage(SOCKET curSocket, char * message, int messageSize);
   static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize);
};

