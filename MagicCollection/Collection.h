#pragma once
#include "CollectionObject.h"
#include "ICollection.h"
#include "CollectionSource.h"

// Just use magic card object for now
#include "MagicCardObject.h"

#include <vector>
#include <map>
#include <iterator>

class Collection : public ICollection
{
public:
   Collection();
   ~Collection();

   void AddItem(std::string aszNewItem) override;
   std::vector<ICollectionObject*> GetList();

private:
   CollectionSource* m_ColSource;
   std::vector<ICollectionObject*> m_lstCollection;
   std::map<std::string, ICollection*> m_mapChildren;
   ICollection* m_Parent;

   std::vector<std::string> m_lstStringCollection;

   int compareItems(std::string aszItemOne, std::string aszItemTwo);
   //std::string importItem(std::string aszName);
};

