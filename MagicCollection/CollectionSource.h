#pragma once
#pragma message ("Starting CollectionSource.h")
#include "SourceObject.h"
#include "CollectionObject.h"

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "json.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>

class CollectionObject;
class Collection;


class CollectionSource
{
public:
   CollectionSource();
   ~CollectionSource();

   // Database Functions
   void LoadLib(std::string aszFileName);

   // Return the cache location if successful, -1 otherwise.
   int LoadCard(std::string aszCardName);

   // Expose the Collection Object to get copies etc..
   CollectionObject* GetCardPrototype(int aiCacheIndex);

   // Returns all copies with parent == name
   std::vector<std::pair<std::string, CopyObject*>> GetCollection(std::string aszCollectionName);

   std::vector<std::string> GetAllCardsStartingWith(std::string aszText);

private:
   std::vector<SourceObject> m_lstptCardBuffer;
   std::vector<CollectionObject> m_lstoCardCache;

   int findInBuffer(std::string aszName, bool abCaseSensitive);

   void finalizeBuffer();

   // This buffer stores ALL characters for every card.
   // It is a list of keyVal sequences where the KEYS are all 3 CHARS LONG.
   // So, in order to specify a pair, you need the start point, and the length of the val.
   // This info can be all stored in a single 4 byte int
   // Bit 1: NonUnique trait flag
   // Bits 2-10: Value Length
   // Bits 11-32: Start point.
   char* m_AllCharBuff;
   unsigned int m_iAllCharBuffSize;
};

#pragma message ("Finish CollectionSource.h")