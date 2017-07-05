#pragma once
#include <vector>
#include <string>

#include "Collection.h"
#include "StringHelper.h"
#include "CollectionSource.h"
class CollectionFactory
{
public:
   CollectionFactory(CollectionSource* aoColSource);
   ~CollectionFactory();

   bool CollectionExists(std::string aszCollectionName);
   Collection* GetCollection(std::string aszCollectionName);

   std::string LoadCollectionFromFile( std::string aszColFile);

   std::vector<std::string> GetLoadedCollections();

private:
   std::vector<Collection*> m_lstCollections;
   CollectionSource* m_ColSource;
};

