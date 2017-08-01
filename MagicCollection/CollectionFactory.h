#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Collection.h"
#include "StringHelper.h"
#include "CollectionSource.h"
class CollectionFactory
{
public:
   CollectionFactory(CollectionSource* aoColSource);
   ~CollectionFactory();

   bool CollectionExists(std::string aszCollectionName);
   bool CollectionExists(const Address& aAddrColID);
   Collection* GetCollection(std::string aszCollectionName);

   std::string LoadCollectionFromFile(std::string aszColFile);
   std::string CreateNewCollection(std::string aszColName, std::string aszParent = "");

   std::vector<std::string> GetLoadedCollections();

private:
   std::vector<std::shared_ptr<Collection>> m_lstCollections;
   CollectionSource* m_ColSource;

   std::string getNextChildName(std::string aszParentID);
};

