
#include "CollectionFactory.h"


CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
   m_ColSource = aoColSource;
   m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

Collection* CollectionFactory::LoadCollection(std::string aszCollectionName, std::string aszFileName)
{
   std::vector<std::pair<std::string, std::string>> lstForcedChanges;

   Collection* oCol = GetCollection(aszCollectionName);
   oCol->LoadCollection(aszFileName, lstForcedChanges);

   std::vector<std::pair<std::string, std::string>>::iterator iter_change = lstForcedChanges.begin();
   for (; iter_change != lstForcedChanges.end(); ++iter_change)
   {
      if (CollectionExists(iter_change->first))
      {
         Collection* oChangedCol = GetCollection(iter_change->first);
         oChangedCol->RecordForcedTransaction(iter_change->second);
      }
   }

   return oCol;
}

Collection* CollectionFactory::GetCollection(std::string aszCollectionName)
{
   // Check if we have the collection already.
   std::vector<Collection>::iterator iter_cols = m_lstCollections.begin();
   for (; iter_cols != m_lstCollections.end(); ++iter_cols)
   {
      if (aszCollectionName == iter_cols->GetName())
      {
         return &(*iter_cols);
      }
   }

   // If not, create one.
   Collection oCol(aszCollectionName, m_ColSource, &LoadedCollections);
   m_lstCollections.push_back(oCol);
   LoadedCollections.push_back(aszCollectionName);

   return &m_lstCollections.at(m_lstCollections.size()-1);
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
   std::vector<Collection>::iterator iter_cols = m_lstCollections.begin();
   for (; iter_cols != m_lstCollections.end(); ++iter_cols)
   {
      if (aszCollectionName == iter_cols->GetName())
      {
         return true;
      }
   }

   return false;
}