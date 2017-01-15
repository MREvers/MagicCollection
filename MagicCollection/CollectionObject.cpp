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

ICollectionObject* CollectionObject::IncludeInCollection(Collection* aoCol, std::vector<ICollectionObject*>& alstColOs)
{
   CollectionAnalytics cA;

   // Store the ID
   cA.ListCopies.push_back(++m_iAllCopies);

   // Store the parent copy location
   cA.MapParentCollection.insert(std::make_pair(m_iAllCopies, aoCol));

   // Prepare to store the collection info
   std::pair<Collection*, CollectionAnalytics> oPair(aoCol, cA);
   m_mapAnalytics.insert(oPair);

   // Make sure the collection has a pointer to this object.
   addSorted(this, alstColOs);

   return this;
}

std::vector<int> CollectionObject::GetCopies(Collection* aoCol)
{
   return m_mapAnalytics.at(aoCol).ListCopies;
}

int CollectionObject::GetNumberOfCopies(Collection* aoCol)
{
   return m_mapAnalytics.at(aoCol).ListCopies.size();
}

Collection* CollectionObject::GetResidentCollection(Collection* aoCol, int aiCardID)
{
   return m_mapAnalytics.at(aoCol).MapParentCollection.at(aiCardID);
}

void CollectionObject::addSorted(ICollectionObject* aoColO, std::vector<ICollectionObject*> alstColOs)
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