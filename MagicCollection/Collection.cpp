#include "Collection.h"


Collection::Collection(std::string aszName, CollectionSource* aoSource)
{
   m_ColSource = aoSource;
   m_szName = aszName;
}

Collection::~Collection()
{
}

std::string Collection::GetName()
{
   return m_szName;
}

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::AddItem(std::string aszNewItem)
{
   int iCard = m_ColSource->LoadCard(aszNewItem);
   if (iCard != -1)
   {
      // Make sure we don't already 'have' this card.
      bool bNewCard = true;
      for (int i = 0; i < m_lstCollection.size(); i++)
      {
         bNewCard &= m_lstCollection.at(i) != iCard;
      }

      if (bNewCard)
      {
         m_lstCollection.push_back(iCard);
      }

      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card.
      oCard->AddCopy(m_szName);

   }

}

void Collection::PrintList()
{
   for (int i = 0; i < m_lstCollection.size(); i++)
   {
      for (int t = 0; t < m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetCopies(m_szName).size(); t++)
      {
         std::cout << m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetName() << std::endl;
      }
      
   }
}
