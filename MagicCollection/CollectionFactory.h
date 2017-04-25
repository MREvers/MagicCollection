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

   /* LoadCollection
   *  Loads a collection into memory by file name.
   *  If the file is missing a collection name entry, nullptr is returned
   *  @Param aszColFile File name of collection to load.
   *  @return Pointer to a successfully loaded collection, nullptr if fail.
   */
   Collection* LoadCollection( std::string aszColFile);

   // Returns the collection if loaded. Returns an empty collection otherwise.
   Collection* GetCollection(std::string aszCollectionName);

   // Returns true if the collection is loaded into memory.
   bool CollectionExists(std::string aszCollectionName);
   std::string GetCollectionNameFromFile(std::string aszCollectionFileName);

   std::vector<std::string> LoadedCollections;
private:
   std::vector<Collection> m_lstCollections;
   CollectionSource* m_ColSource;
};

