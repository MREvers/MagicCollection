#pragma once
#pragma message ("Starting Collection.h")
#include "ICollection.h"

#include "ICollectionObject.h"
#include "CollectionObject.h"

// Just use magic card object for now
#include "MagicCardObject.h"

#include "CollectionSource.h"

#include <vector>
#include <map>
#include <iterator>

class Collection : public ICollection
{
public:
   Collection(CollectionSource* aoSource);
   ~Collection();

   void AddItem(std::string aszNewItem) override;
   void AddItem(std::string aszNewItem, ICollection* aoCol) override;
   int FindInCollection(std::string aszItem) override;
   bool GetFromCollection(std::string aszName, ICollectionObject& rptColO) override;

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

#pragma message ("Finish Collection.h")