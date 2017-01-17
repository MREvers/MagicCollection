#include "CollectionObject.h"


CollectionObject::CollectionObject(std::string aszName)
{
   m_szName = aszName;
   m_iAllCopies = 0;
}


CollectionObject::~CollectionObject()
{
}

std::string CollectionObject::GetName()
{
   return m_szName;
}

bool CollectionObject::MapAttributes(std::string aszName, std::string aszValue)
{
   return true;
}

// Creates the analytic's object for this Collection. THERE SHOULD BE ONLY ONE.
ICollectionObject* CollectionObject::IncludeInCollection(ICollection* aoCol, std::vector<ICollectionObject*>& alstColOs)
{
   CollectionAnalytics cA;

   // Store the ID
   cA.ListCopies.push_back(++m_iAllCopies);

   // Store the parent copy location
   cA.Map_ID_Parent_Collection.insert(std::make_pair(m_iAllCopies, aoCol));

   // Prepare to store the collection info
   std::pair<ICollection*, CollectionAnalytics> oPair(aoCol, cA);
   m_mapAnalytics.insert(oPair);

   // Make sure the collection has a pointer to this object.
   addSorted(this, alstColOs);

   return this;
}

std::vector<int> CollectionObject::GetCopies(ICollection* aoCol)
{
   return m_mapAnalytics.at(aoCol).ListCopies;
}

int CollectionObject::GetNumberOfCopies(ICollection* aoCol)
{
   return m_mapAnalytics.at(aoCol).ListCopies.size();
}

std::vector<int> CollectionObject::GetLocalCopies(ICollection* aoCol)
{
   std::vector<int> lstLocalCopies;
   for (std::map<int, ICollection*>::iterator iter = m_mapAnalytics.at(aoCol).Map_ID_Parent_Collection.begin();
      iter != m_mapAnalytics.at(aoCol).Map_ID_Parent_Collection.end(); ++iter)
   {
      if (iter->second == aoCol)
      {
         lstLocalCopies.push_back(iter->first);
      }
   }

   return lstLocalCopies;
}

int CollectionObject::GetNumOfLocalCopies(ICollection* aoCol)
{
   return GetLocalCopies(aoCol).size();
}

ICollection* CollectionObject::GetResidentCollection(ICollection* aoCol, int aiCardID)
{
   return m_mapAnalytics.at(aoCol).Map_ID_Parent_Collection.at(aiCardID);
}

void CollectionObject::IncrementItem(ICollection* aoColO)
{
   // Create a new copy with new id.
   m_mapAnalytics.at(aoColO).ListCopies.push_back(++m_iAllCopies);

   // Store that copy's parent.
   m_mapAnalytics.at(aoColO).Map_ID_Parent_Collection.insert(std::make_pair(m_iAllCopies, aoColO));
}

void CollectionObject::RemoveItem(ICollection* aoColO, int aiCardID)
{
   std::vector<int>::iterator id_iter = m_mapAnalytics.at(aoColO).ListCopies.begin();
   for (; id_iter != m_mapAnalytics.at(aoColO).ListCopies.end(); ++id_iter)
   {
      if (*id_iter == aiCardID)
      {
         m_mapAnalytics.at(aoColO).ListCopies.erase(id_iter);
      }
   }
}

bool CollectionObject::AddItem(ICollection* aoCallingCol, ICollection* aoSourcingCol)
{
   bool bFound = false;
   // Get the first card id stored in the sourcing col not already borrowed
   if (m_mapAnalytics.at(aoSourcingCol).ListCopies.size() > 0)
   {
      // Iterate through all the copies.
      std::vector<int>::iterator copy_iter = m_mapAnalytics.at(aoSourcingCol)
         .ListCopies.begin();
      for (; copy_iter != m_mapAnalytics.at(aoSourcingCol)
         .ListCopies.end(); ++copy_iter)
      {
         int iNumSharedCopies = 0;
         // If there aren't any derived copies, we don't have to check.
         if (m_mapAnalytics.at(aoSourcingCol)
            .Map_ID_Derived_Collections.count(*copy_iter) > 0)
         {
            std::vector<ICollection*> lstDerivedCols = m_mapAnalytics.at(aoSourcingCol)
               .Map_ID_Derived_Collections.at(*copy_iter);
            std::vector<ICollection*>::iterator derived_iter = lstDerivedCols.begin();
            for (; derived_iter != lstDerivedCols.end(); ++derived_iter)
            {
               if (*derived_iter == aoCallingCol)
               {
                  iNumSharedCopies++;
                  break;
               }
            }
         }
         if (!(iNumSharedCopies > 0))
         {
            bFound = true;

            // Stop if we found a copy that is unborrowed by aoCallingCol
            break;
         }

      }
      if (bFound)
      {
         return AddItem(aoCallingCol, aoSourcingCol, *copy_iter);
      }
      else
      {
         return false;
      }

   }
}

bool CollectionObject::AddItem(ICollection* aoCallingCol, ICollection* aoSourcingCol, int aiCardID)
{

   // Check if we are already borrowing this card.
   int iNumSharedCopies = 0;
   if (m_mapAnalytics.at(aoSourcingCol)
      .Map_ID_Derived_Collections.count(aiCardID) > 0)
   {
      std::vector<ICollection*> lstDerivedCols = m_mapAnalytics.at(aoSourcingCol)
         .Map_ID_Derived_Collections.at(aiCardID);
      std::vector<ICollection*>::iterator derived_iter = lstDerivedCols.begin();
      for (; derived_iter != lstDerivedCols.end(); ++derived_iter)
      {
         if (*derived_iter == aoCallingCol)
         {
            iNumSharedCopies++;
            break;
         }
      }
   }
   if (!(iNumSharedCopies > 0))
   {
      // Register that the copy is shared with the sourcing col
      if (m_mapAnalytics.at(aoSourcingCol)
         .Map_ID_Derived_Collections.count(aiCardID) > 0)
      {
         m_mapAnalytics.at(aoSourcingCol)
            .Map_ID_Derived_Collections.at(aiCardID).push_back(aoCallingCol);
      }
      else
      {
         std::vector<ICollection*> lstDerivedCols;
         lstDerivedCols.push_back(aoCallingCol);
         m_mapAnalytics.at(aoSourcingCol)
            .Map_ID_Derived_Collections.insert(std::make_pair(aiCardID, lstDerivedCols));
      }
      

      // Register the copy with the calling call
      if (m_mapAnalytics.count(aoCallingCol) > 0)
      {
         m_mapAnalytics.at(aoCallingCol)
            .Map_ID_Parent_Collection.insert(
               std::make_pair(aiCardID, aoSourcingCol)
            );
         m_mapAnalytics.at(aoCallingCol).ListCopies.push_back(aiCardID);
      }
      else
      {
         CollectionAnalytics cA;

         // Store the ID
         cA.ListCopies.push_back(aiCardID);

         // Store the parent copy location
         cA.Map_ID_Parent_Collection.insert(std::make_pair(aiCardID, aoSourcingCol));

         // Prepare to store the collection info
         std::pair<ICollection*, CollectionAnalytics> oPair(aoCallingCol, cA);
         m_mapAnalytics.insert(oPair);

         // Make sure the collection has a pointer to this object.
         addSorted(this, aoCallingCol->GetList());
      }
      
      return true;
   }

   return false;
}

void CollectionObject::addSorted(ICollectionObject* aoColO, std::vector<ICollectionObject*>& alstColOs)
{
   std::string szName = aoColO->GetName();

   int iLastSpot = 0;
   while (iLastSpot < alstColOs.size())
   {
      int iComparisonResult = alstColOs.at(iLastSpot)->GetName().compare(szName);
      if (iComparisonResult < 0)
      {
         iLastSpot++;
      }
      else
      {
         break;
      }
   }

   std::vector<ICollectionObject*>::iterator iter = alstColOs.begin() + iLastSpot;
   alstColOs.insert(iter, aoColO);
}