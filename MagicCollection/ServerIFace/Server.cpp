
#include "Server.h"


Server::Server()
{
   m_ServerSocket = new WinSocketListener();
   m_ServerSocket->Begin();
   m_Clients = 0;
}


Server::~Server()
{
}

void Server::AcceptConnections()
{
   // get new clients
   if (m_ServerSocket->AcceptNewConnection(m_Clients))
   {
      printf("client %d has been connected to the server\n", m_Clients);

      m_Clients++;
   }
}

void Server::RecvFromClient(int iClient)
{
   int iBitsReceived = m_ServerSocket->ReceiveData(iClient, m_RecvBuf);
   if (iBitsReceived  > 0)
   {
	   std::cout << m_RecvBuf << std::endl;
   }
}

void Server::SendToAll(char* msg)
{
   m_ServerSocket->SendToAll(msg, 32);
}