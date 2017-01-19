#pragma once
#pragma message ("Starting Collection.h")

#include <vector>
#include <map>
#include <iterator>

#include "ICollection.h"
#include "ICollectionObject.h"
#include "CollectionObject.h"
// Just use magic card object for now
#include "MagicCardObject.h"
#include "CollectionSource.h"


class Collection : public ICollection
{
public:
   Collection(std::string aszName, CollectionSource* aoSource);
   ~Collection();

   // Meta
   std::string GetName();

   void AddItem(std::string aszNewItem) override;

   void PrintList();

private:
   CollectionSource* m_ColSource;

   // Collections just know that they have AT LEAST a copy of a card. It has to access
   //  the Col object to see how many copies.
   std::vector<int> m_lstCollection;
   std::string m_szName;
};

#pragma message ("Finish Collection.h")