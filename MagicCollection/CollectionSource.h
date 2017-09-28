#pragma once
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

#include "MetaTag.h"
#include "SourceObject.h"
#include "CollectionItem.h"
#include "Config.h"
#include "TryGet.h"

class CollectionItem;
class Collection;


class CollectionSource
{
public:
   CollectionSource();
   ~CollectionSource();

   // Database Functions
   void LoadLib(std::string aszFileName);
   void HotSwapLib(std::string aszFileName);

   // Return the cache location if successful, -1 otherwise.
   int LoadCard(std::string aszCardName);

   // Expose the Collection Object to get copies etc..
   TryGet<CollectionItem> GetCardPrototype(int aiCacheIndex);
   TryGet<CollectionItem> GetCardPrototype(std::string aszCardName);

   // This needs to be called whenever a child collection adds a card.
   // It will let other collections know that they need to check their lists.
   void NotifyNeedToSync(const Location& aAddrForcedSync);
   bool IsSyncNeeded(const Location& aAddrNeedSync);

   std::vector<int> 
      GetCollectionCache( Location aAddrColID,
                          CollectionItemType aColItemType = CollectionItemType::All );
   std::vector<std::shared_ptr<CopyItem>> 
      GetCollection( Location aAddrColID,
                     CollectionItemType aColItemType = CollectionItemType::All );

   std::vector<std::string> GetAllCardsStartingWith(std::string aszText);

   void ClearCache();
private:
   std::vector<SourceObject> m_lstCardBuffer;
   std::vector<CollectionItem> m_lstoCardCache;
   
   // This is used to track whether or not a collection has the most up to date
   //  list of its cards. A collection should only come out of sync if a child
   //  collection of it adds a card.
   std::vector<std::pair<Location, bool>> m_lstSync;

   // Used for caching searches over 5 chars.
   std::vector<std::pair<std::string, std::vector<SourceObject>>> m_lstSearchCache;

   int findInBuffer(std::string aszName, bool abCaseSensitive);
   int findInCache(std::string aszName, bool abCaseSensitive);

   std::string convertToSearchString(std::string& aszSearch);
   bool isSearchCharacter(char c);

   void resetBuffer();
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
