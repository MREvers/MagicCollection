#include "ClientSocket.h"
#include "WinAPINetworkServices.h"

#include <ws2tcpip.h>

// Windows only establishes connections using 1 SOCKET. A socket is either unconnected
//  which is analogous to TCP IP Socket. A connected Socket is akin to a connection.
ClientSocket::ClientSocket(PCSTR szIPString,
                           PCSTR iPort)
                            : WinSocket()
{
   if (isReady() == 0)
   {
      m_IP= szIPString;
      m_Port = iPort;
   }
}


ClientSocket::~ClientSocket()
{
}

bool ClientSocket::BeginConnect()
{
   struct addrinfo *result,
                     *ptr;


   //resolve server address and port 
   // This port IP is used to designate the target because it will be used in 'connect'
   m_SocketInfo.ai_family = AF_UNSPEC;
   m_iResult = getaddrinfo(m_IP, m_Port, &m_SocketInfo, &result);

   if (m_iResult != 0)
   {
      printf("getaddrinfo failed with error: %d\n", m_iResult);
      WSACleanup();
      exit(1);
   }

   // Attempt to connect to an address until one succeeds
   for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
   {

      // Create a SOCKET for connecting to server
      m_Socket = socket(ptr->ai_family, ptr->ai_socktype,
         ptr->ai_protocol);

      if (m_Socket == INVALID_SOCKET)
      {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         WSACleanup();
         exit(1);
      }

      // Connect to server.
      m_iResult = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);

      if (m_iResult == SOCKET_ERROR)
      {
         closesocket(m_Socket);
         m_Socket = INVALID_SOCKET;
         printf("The server is down... did not connect");
      }
   }



   // no longer need address info for server
   freeaddrinfo(result);



   // if connection failed
   if (m_Socket == INVALID_SOCKET)
   {
      printf("Unable to connect to server!\n");
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

   //disable nagle
   char value = 1;
   setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

   return false;
}

int ClientSocket::ReceiveData(char * recvbuf)
{
   m_iResult = WinAPINetworkServices::receiveMessage(m_Socket, recvbuf, PACKET_SIZE);

   if (m_iResult == 0)
   {
      printf("Connection closed\n");
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}

int ClientSocket::SendData(char* msg)
{
   m_iResult = WinAPINetworkServices::sendMessage(m_Socket, msg, PACKET_SIZE);

   if (m_iResult == 0)
   {
      printf("Connection closed\n");
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}
