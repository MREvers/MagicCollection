#pragma once
#include <process.h>
#include "ServerIFace\Server.h"

// Exposes members so that the C++/CLI wrapper class can access native code.
class __declspec(dllexport) ServerIFace
{
public:
   ServerIFace();
   ~ServerIFace();

   // Opens the "SOCKET" interface. Only necessary if communicating via sockets.
   void StartServer();

   Server* ServerActual;
private:
   static unsigned int __stdcall serverLoop(void * arg);
};

