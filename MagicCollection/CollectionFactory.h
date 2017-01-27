#pragma once
#include <vector>
#include <string>

#include "Collection.h"
#include "CollectionSource.h"
class CollectionFactory
{
public:
   CollectionFactory(CollectionSource* aoColSource);
   ~CollectionFactory();

   Collection* GetCollection(std::string aszCollectionName);
   bool CollectionExists(std::string aszCollectionName);

   std::vector<std::string> LoadedCollections;
private:
   std::vector<Collection> m_lstCollections;
   CollectionSource* m_ColSource;
};

