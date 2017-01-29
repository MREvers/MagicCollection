
#include "ProgramServer.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


ProgramServer::ProgramServer()
{
   sendLock = new std::mutex();
   recvLock = new std::mutex();

   // initialize the server
   server = new Server();

   // initialize the client 
   client = new ClientSocket();
}


ProgramServer::~ProgramServer()
{
}

void ProgramServer::BeginConnectClient()
{
   client->BeginConnect();
}

void ProgramServer::BeginListeningServer()
{
   // 'this' is the implicit first parameter.
   std::thread t(&ProgramServer::listenForClients, this);
   t.join();
}

void ProgramServer::listenForClients()
{
   while (true)
   {
      server->AcceptConnections();
   }
}


void ProgramServer::SendToAll(char* msg) const
{
   sendLock->lock();
   server->SendToAll(msg);
   sendLock->unlock();
}

void ProgramServer::RecvMsg(char* buffer) const
{
   recvLock->lock();
   client->ReceiveData(buffer);
   recvLock->unlock();
}

ProgramServer* ProgramServer::_instance = nullptr;
ProgramServer* ProgramServer::Instance()
{
   if (_instance ==  nullptr)
   {
      _instance = new ProgramServer();
   }

   return _instance;
}