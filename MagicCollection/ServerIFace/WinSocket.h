#pragma once
#include <WinSock2.h>

#define DEFAULT_PORT "6881"
#define DEFAULT_CLIENT_PORT "6881"

class WinSocket
{
public:
   WinSocket();
   ~WinSocket();

protected:
   // Returns 0 if initialization was successful.
   int isReady();

   // Store whether initialization was successful
   int m_iResult;

   // Wrap the socket.
   SOCKET m_Socket;

   // Stores socket info, not the concrete address.
   // Stores address family, etc.
   addrinfo m_SocketInfo;
   
};

