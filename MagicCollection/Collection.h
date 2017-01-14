#pragma once
#include "CollectionObject.h"
#include "ICollection.h"

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
   std::vector<ICollectionObject*> m_lstCollection;
   std::map<std::string, ICollection*> m_mapChildren;
   ICollection* m_Parent;

   std::vector<std::string> m_lstStringCollection;

   int compareItems(std::string aszItemOne, std::string aszItemTwo);
   //std::string importItem(std::string aszName);
};

