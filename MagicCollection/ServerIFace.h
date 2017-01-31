#pragma once
#include <process.h>
#include "ServerIFace\Server.h"
class __declspec(dllexport) ServerIFace
{
public:
   ServerIFace();
   ~ServerIFace();

   Server* ServerActual;
private:
	static unsigned int __stdcall serverLoop(void * arg);
};

