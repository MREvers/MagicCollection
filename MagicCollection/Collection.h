#pragma once
#pragma message ("Starting Collection.h")
#include "ICollection.h"
#include "IName.h"

#include "ICollectionObject.h"
#include "CollectionObject.h"

// Just use magic card object for now
#include "MagicCardObject.h"

#include "CollectionSource.h"

#include <vector>
#include <map>
#include <iterator>

class Collection : public ICollection, IName
{
public:
   Collection(std::string aszName, CollectionSource* aoSource);
   ~Collection();

   void AddItem(std::string aszNewItem) override;
   void AddItem(std::string aszNewItem, ICollection* aoCol) override;

private:
   CollectionSource* m_ColSource;
   std::vector<int> m_lstCollection;
};

#pragma message ("Finish Collection.h")