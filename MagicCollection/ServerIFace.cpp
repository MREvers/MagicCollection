#include "ServerIFace.h"


ServerIFace::ServerIFace()
{
   ServerActual = new Server();
}


ServerIFace::~ServerIFace()
{
   delete ServerActual;
}

void ServerIFace::StartServer()
{
   _beginthreadex(0, 0, ServerIFace::serverLoop, this, 0, 0);
}

unsigned int ServerIFace::serverLoop(void * ap_this)
{
   ServerIFace* p_this = static_cast<ServerIFace*>(ap_this);
   while (true)
   {
      p_this->ServerActual->AcceptConnections();
      p_this->ServerActual->SendToAll("What");
      p_this->ServerActual->RecvFromClient(0);
   }
}