#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>

#include "JSONImporter.h"
#include "CollectionSource.h"
#include "CollectionFactory.h"

class __declspec(dllexport) CStoreFrontBackEnd
{
public:
   CStoreFrontBackEnd();
   ~CStoreFrontBackEnd();

   std::string LoadCollection(std::string aszCollectionFile);
   std::vector<std::string> GetLoadedCollections();

   std::vector<std::string> GetCollectionList(std::string aszCollection);
   std::string GetCardPrototype(std::string aszCardName);

   std::vector<std::string> GetAllCardsStartingWith(std::string aszSearch);

   std::string GetImagesPath();

   void SubmitBulkChanges(std::string aszCollection, std::vector<std::string> alstChanges);
   
   void ImportCollection();

private:
   CollectionFactory* m_ColFactory; // This will be the main interaction.
   CollectionSource* m_ColSource; // This can be a vector in the future
};

