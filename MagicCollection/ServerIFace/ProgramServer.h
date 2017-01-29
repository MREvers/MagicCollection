#pragma once
#include "ClientSocket.h"
#include "Server.h"
// used for multi-threading
#include <process.h>
#include <mutex>


class __declspec(dllexport) ProgramServer
{
public:
   ProgramServer();
   ~ProgramServer();

   void BeginListeningServer();
   void BeginConnectClient();

   void SendToAll(char* msg) const;
   void RecvMsg(char* buffer) const;

   static ProgramServer* Instance();
private:
   static ProgramServer* _instance;

   ClientSocket* client;
   Server* server;

   std::mutex* sendLock;
   std::mutex* recvLock;

   void listenForClients();
};

