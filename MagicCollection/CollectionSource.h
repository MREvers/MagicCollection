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

private:
   std::vector<SourceObject> m_lstptCardBuffer;
   std::vector<CollectionObject> m_lstoCardCache;

   int findInBuffer(std::string aszName);
};

#pragma message ("Finish CollectionSource.h")