#pragma once
#include "WinSocket.h"
#include <iostream>

class ClientSocket :
   public WinSocket
{
public:
   ClientSocket(PCSTR szIPString = "127.0.0.1",
      PCSTR iPort = DEFAULT_CLIENT_PORT);
   ~ClientSocket();

   bool BeginConnect();

   int ReceiveData(char * recvbuf);
   int SendData(char* msg);

private:
   PCSTR m_IP;
   PCSTR m_Port;
};

