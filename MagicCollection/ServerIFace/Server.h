#pragma once
#include "WinSocketListener.h"
#include "WinAPINetworkServices.h"
#include <iostream>

class __declspec(dllexport) Server
{
public:
   Server();
   ~Server();

   void AcceptConnections();
   void RecvFromClient(int iClient);
   void SendToAll(char* msg);

private:
   WinSocketListener* m_ServerSocket;

   unsigned int m_Clients;

   char m_RecvBuf[PACKET_SIZE];
};

