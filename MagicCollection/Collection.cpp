#include "Collection.h"


Collection::Collection(CollectionSource* aoSource)
{
   m_ColSource = aoSource;
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
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::AddItem(std::string aszNewItem)
{
   int iFound;
   if ((iFound = FindInCollection(aszNewItem)) == -1)
   {
      // If this Collection doesn't already have this card...
      // Create a copy of the desired subtype. If the source has one already, this will get lost,
      //  because the pointer to it will be changed.
      CollectionObject* mExisting = nullptr;
      CollectionObject* mNew = new MagicCardObject(aszNewItem);
      if (m_ColSource->GetCard(aszNewItem, *mNew, mExisting))
      {
         if (mExisting == nullptr)
         {
            // Get a card from the collection (need card id) then add it.

            mNew->IncludeInCollection(this, m_lstCollection);
         }
         else
         {
            mExisting->IncludeInCollection(this, m_lstCollection);
            delete mNew;
         }

      }
   }
   else
   {
      // This card is already in this collection
      m_lstCollection.at(iFound)->IncrementItem(this);
   }

}

void Collection::AddItem(std::string aszNewItem, ICollection* aoCol)
{
   // First we have to check if that collection has that card
   if (aoCol == this)
   {
      return;
   }

   CollectionObject* mExisting = nullptr;
   CollectionObject* mNew = new MagicCardObject(aszNewItem);
   if (m_ColSource->GetCard(aszNewItem, *mNew, mExisting))
   {
      if (mExisting == nullptr)
      {
         // The card doesn't exist so obviously just escape.
         delete mNew;
         return;
         
      }
      else
      {
         mExisting->AddItem(this, aoCol);
         delete mNew;
      }

   }
}

int Collection::FindInCollection(std::string aszName)
{
   int iLeft = 0;
   int iRight = m_lstCollection.size();
   if (iRight < 1)
   {
      return -1;
   }

   while (iLeft <= iRight)
   {
      int middle = (iLeft + iRight) / 2;

      if (middle < 0 || middle >= m_lstCollection.size())
      {
         return -1;
      }

      if (m_lstCollection.at(middle)->GetName() == aszName)
         return middle;
      else if (aszName.compare(m_lstCollection.at(middle)->GetName()) < 0)
         iRight = middle - 1;
      else
         iLeft = middle + 1;
   }

   return -1;
}

bool Collection::GetFromCollection(std::string aszName, ICollectionObject& rptColO)
{
   bool bRetval = false;
   int iFound = FindInCollection(aszName);
   if (iFound != -1)
   {
      bRetval = true;
   }

   return bRetval;
}

int Collection::compareItems(std::string aszItemOne, std::string aszItemTwo)
{
   return aszItemOne.compare(aszItemTwo);
}

std::vector<ICollectionObject*>& Collection::GetList()
{
   return m_lstCollection;
}