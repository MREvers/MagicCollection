
#include "WinSocket.h"


WinSocket::WinSocket()
{
   // Perform general Socket Overhead

   // create WSADATA object
   WSADATA wsaData;

   // Initialize the socket in memory.
   m_Socket = INVALID_SOCKET;

   // Initialize Winsock
   m_iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (m_iResult != 0)
   {
      //printf("WSAStartup failed with error: %d\n", m_iResult);
      exit(1);
   }

   // Initialize the m_SocketInfo 'hints'
   ZeroMemory(&m_SocketInfo, sizeof(m_SocketInfo));
   m_SocketInfo.ai_socktype = SOCK_STREAM;
   m_SocketInfo.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
   //hints.ai_flags = AI_PASSIVE; // server

}


WinSocket::~WinSocket()
{
}


int WinSocket::isReady()
{
   return m_iResult;
}
