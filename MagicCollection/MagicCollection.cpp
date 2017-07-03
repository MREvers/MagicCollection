// MagicCollection.cpp : Defines the entry point for the console application.
//
#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>

#include "ItemCollection.h"
#include "JSONImporter.h"
#include "CollectionSource.h"
#include "CollectionFactory.h"

#include "ServerIFace\ClientSocket.h"
#include "ServerIFace\Server.h"


void serverLoop(void *);
void clientLoop(void);

ClientSocket * client;
Server * server;

int main()
{

	// initialize the server
	server = new Server();

	// create thread with arbitrary argument for the run function
	serverLoop(NULL);

	// initialize the client 
	//client = new ClientSocket();
	//client->BeginConnect();


	//clientLoop();
	/*
   CollectionSource CS;
   CS.LoadLib("AllSets.json.out");

   CollectionFactory CF(&CS);

   Collection* Col = CF.LoadCollection("Primary", "TestCollection.txt");
   Col->CreateBaselineHistory();
   Col->RemoveItem("Sylvan Advocate");
   
   

   Collection* ColM = CF.LoadCollection("Main", "TestCollectionTwo.txt");
   ColM->CreateBaselineHistory();
   Col->SaveCollection("ColOut.txt");
   ColM->SaveCollection("ColTwoOut.txt");
   Col->PrintList();
   std::cin.get();
   */
    return 0;
}

std::string escape(const std::string& src)
{
   std::stringstream dst;
   for (char ch : src)
   {
      switch (ch)
      {
      case '&': dst << "&amp;"; break;
      case '\'': dst << "&apos;"; break;
      case '"': dst << "&quot;"; break;
      case '<': dst << "&lt;"; break;
      case '>': dst << "&gt;"; break;
      default: dst << ch; break;
      }
   }
   return dst.str();
}



void serverLoop(void * arg)
{

	while (true)
	{
		server->AcceptConnections();
		server->SendToAll("What");
		server->RecvFromClient(0);
	}
}

void clientLoop()
{
	char* recvBuf = new char[10000];
	int iBitsReceived = 0;
	while (true)
	{
		if (iBitsReceived = client->ReceiveData(recvBuf) > 0)
		{
			std::cout << recvBuf << std::endl;
			client->SendData("HEY");
		}
		
	}
}
