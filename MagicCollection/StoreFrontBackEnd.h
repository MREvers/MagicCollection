#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>

#include "MetaTag.h"
#include "ItemCollection.h"
#include "JSONImporter.h"
#include "CollectionSource.h"
#include "CollectionFactory.h"
//#include "ServerIFace.h"

class __declspec(dllexport) CStoreFrontBackEnd
{
public:
   CStoreFrontBackEnd();
   ~CStoreFrontBackEnd();

   std::string LoadCollection(std::string aszCollectionFile);
   std::vector<std::string> GetLoadedCollections();
   std::vector<std::string>
	   GetCollectionList(std::string aszCollection);

   std::vector<std::string> GetAllCardsStartingWith(std::string aszSearch);

   std::string GetImagesPath();

private:
   //ServerIFace* m_ServerIFace;
   CollectionFactory* m_ColFactory; // This will be the main interaction.
   CollectionSource* m_ColSource; // This can be a vector in the future
};

