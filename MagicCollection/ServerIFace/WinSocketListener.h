#pragma once
#include "WinSocket.h"
#include "WinAPINetworkServices.h"
#include <ws2tcpip.h>
#include <map>
// Creates a WinSocket that listens for client connections.
// When a new connection is created, it creates a socket and stores it in m_Sessions.
// The constructor binds to a socket.
class WinSocketListener :
   public WinSocket
{
public:
   WinSocketListener(PCSTR iport = DEFAULT_PORT);
   ~WinSocketListener();

   // Begins listening on the Port
   int Begin();

   // Returns true if succesful connection. Stores connection id in id.
   bool AcceptNewConnection(unsigned int& id);

   int ReceiveData(unsigned int client_id, char * recvbuf);
   void SendToAll(char * packets, int totalSize);

private:
   
   // When accepting new clients, the client socket will be
   // build here, then copied into m_Sessions
   SOCKET m_ClientSocketBuffer;

   // table to keep track of each client's socket
   // Since this accepts SOCKET and not SOCKET*, the SOCKET will be copied in.
   std::map<unsigned int, SOCKET> m_Sessions;
};

