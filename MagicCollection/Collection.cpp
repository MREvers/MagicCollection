#include "Collection.h"


Collection::Collection()
{
}


Collection::~Collection()
{
   for (auto i : m_lstCollection)
   {
      delete i;
   }
}

// Looks up from the source collection then adds it.
void Collection::AddItem(std::string aszNewItem)
{
   int iLastSpot = 0;
   while (iLastSpot < m_lstCollection.size())
   {
      int iComparisonResult = compareItems(m_lstCollection.at(iLastSpot)->GetName(), aszNewItem);
      if (iComparisonResult < 0)
      {
         iLastSpot++;
      }
      else
      {
         break;
      }
   }
   CollectionObject* colNewCollectionObject = new CollectionObject(aszNewItem);
   std::vector<ICollectionObject*>::iterator iter = m_lstCollection.begin() + iLastSpot;
   m_lstCollection.insert(iter, colNewCollectionObject);
}

int Collection::compareItems(std::string aszItemOne, std::string aszItemTwo)
{
   return aszItemOne.compare(aszItemTwo);
}

std::vector<ICollectionObject*> Collection::GetList()
{
   return m_lstCollection;
}