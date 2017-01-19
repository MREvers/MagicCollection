#include "CollectionObject.h"


CollectionObject::CollectionObject(std::string aszName)
{
   m_iAllCopies = 0;
   m_szName = aszName;
}

CollectionObject::~CollectionObject()
{
}

bool CollectionObject::MapAttributes(std::string aszName, std::string aszValue)
{
   return true;
}

std::string CollectionObject::GetName()
{
   return m_szName;
}

CopyObject* CollectionObject::AddCopy(std::string aszCollectionName)
{
   CopyObject oNewCopy;
   oNewCopy.ParentCollection = aszCollectionName;
   oNewCopy.ResidentCollections.push_back(aszCollectionName);
   m_lstCopies.push_back(oNewCopy);
   return &m_lstCopies.at(m_lstCopies.size()-1);
}

std::vector<CopyObject*> CollectionObject::GetLocalCopies(std::string aszCollectionName)
{
   std::vector<CopyObject*> rLstRetVal;
   std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
   for (; card_iter != m_lstCopies.end(); ++card_iter)
   {
      if (card_iter->ParentCollection == aszCollectionName)
      {
         CopyObject* oFoundCard = card_iter._Ptr;
         rLstRetVal.push_back(oFoundCard);
      }
   }

   return rLstRetVal;
}

std::vector<CopyObject*> CollectionObject::GetCopies(std::string aszCollectionName)
{
   std::vector<CopyObject*> rLstRetVal;
   std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
   for (; card_iter != m_lstCopies.end(); ++card_iter)
   {
      std::vector<std::string>::iterator resi_iter = card_iter->ResidentCollections.begin();
      for (; resi_iter != card_iter->ResidentCollections.end(); ++resi_iter)
      {
         if (*resi_iter == aszCollectionName)
         {
            CopyObject* oFoundCard = card_iter._Ptr;
            rLstRetVal.push_back(oFoundCard);
            break;
         }
      }
   }

   return rLstRetVal;
}