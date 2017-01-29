
#include "WinSocketListener.h"



WinSocketListener::WinSocketListener(PCSTR iport) : WinSocket()
{
   // Check if initialization was successful
   if (isReady() == 0)
   {
      m_ClientSocketBuffer = INVALID_SOCKET;

      // Specialize Socket.. AI_PASSIVE means that the Socket will be used in a bind call.
      // See MS website
      m_SocketInfo.ai_family = AF_INET;
      m_SocketInfo.ai_flags = AI_PASSIVE;

      // Stores concrete address.
      struct addrinfo* result;

      // Resolve the server address and port. Get a concrete address and port.
      // Note that later, the Socket will listen to the 'DEFAULT_PORT'
      // Since this addrinfo is used in "Bind" is is specifying the local port.
      m_iResult = getaddrinfo(NULL, iport, &m_SocketInfo, &result);
      if (m_iResult != 0)
      {
         printf("getaddrinfo failed with error: %d\n", m_iResult);
         WSACleanup();
         exit(1);
      }

      // Create a concrete socket object
      m_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
      if (m_Socket == INVALID_SOCKET)
      {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         freeaddrinfo(result);
         WSACleanup();
         exit(1);
      }

      // Set the mode of the socket to be nonblocking
      u_long iMode = 1;
      m_iResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
      if (m_iResult == SOCKET_ERROR)
      {
         printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
         closesocket(m_Socket);
         WSACleanup();
         exit(1);
      }

      // Setup the TCP listening socket. Bind the socket
      m_iResult = bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);
      if (m_iResult == SOCKET_ERROR)
      {
         printf("bind failed with error: %d\n", WSAGetLastError());
         freeaddrinfo(result);
         closesocket(m_Socket);
         WSACleanup();
         exit(1);
      }

      // no longer need address information
      freeaddrinfo(result);
   }
}


WinSocketListener::~WinSocketListener()
{
}

int WinSocketListener::Begin()
{
   // start listening for new clients attempting to connect
   m_iResult = listen(m_Socket, SOMAXCONN);
   if (m_iResult == SOCKET_ERROR)
   {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}

bool WinSocketListener::AcceptNewConnection(unsigned int& id)
{
   // if client waiting, accept the connection and save the socket
   m_ClientSocketBuffer = accept(m_Socket, NULL, NULL);

   if (m_ClientSocketBuffer != INVALID_SOCKET)
   {
      //disable nagle on the client's socket
      char value = 1;
      setsockopt(m_ClientSocketBuffer, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

      // insert new client into session id table
      m_Sessions.insert( std::pair<unsigned int, SOCKET>(id, m_ClientSocketBuffer) );

      return true;
   }

   return false;
}

// receive incoming data
int WinSocketListener::ReceiveData(unsigned int client_id, char * recvbuf)
{
   if (m_Sessions.find(client_id) != m_Sessions.end())
   {
      SOCKET currentSocket = m_Sessions[client_id];
      m_iResult = WinAPINetworkServices::receiveMessage(currentSocket, recvbuf, PACKET_SIZE);

      if (m_iResult == 0)
      {
         printf("Connection closed\n");
         closesocket(currentSocket);
      }

      return m_iResult;
   }

   return 0;
}

// send data to all clients
void WinSocketListener::SendToAll(char * packets, int totalSize)
{
   SOCKET currentSocket;
   std::map<unsigned int, SOCKET>::iterator iter;
   int iSendResult;

   for (iter = m_Sessions.begin(); iter != m_Sessions.end(); iter++)
   {
      currentSocket = iter->second;
      iSendResult = WinAPINetworkServices::sendMessage(currentSocket, packets, totalSize);

      if (iSendResult == SOCKET_ERROR)
      {
         printf("send failed with error: %d\n", WSAGetLastError());
         closesocket(currentSocket);
      }
   }
   Sleep(1000);
}