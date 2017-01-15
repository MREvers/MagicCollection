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
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
void Collection::AddItem(std::string aszNewItem)
{

   // Just use magic card shit for now

   // If this Collection doesn't already have this card...
   MagicCardObject* mCO = new MagicCardObject(aszNewItem);
   if (m_ColSource->GetCard(aszNewItem, *mCO))
   {
      mCO->IncludeInCollection(this, m_lstCollection);
   }
   else
   {
      delete mCO;
   }
   
}

int Collection::compareItems(std::string aszItemOne, std::string aszItemTwo)
{
   return aszItemOne.compare(aszItemTwo);
}

std::vector<ICollectionObject*> Collection::GetList()
{
   return m_lstCollection;
}