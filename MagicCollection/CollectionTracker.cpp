#include "CollectionTracker.h"
#include <functional>
#include "CollectionItem.h"
#include "Collection.h"
#include "ListHelper.h"

CollectionTracker::CollectionTracker(Collection* aptrTCollection)
{
   m_ptrTrackedCollection = aptrTCollection;
}

CollectionTracker::~CollectionTracker()
{
}

void 
CollectionTracker::Track()
{
   std::vector<int>::iterator iter_Item;
   std::vector<int> lstNewItems;
   RealCopyList::iterator iter_Copy;
   CollectionSource* source;
   Address colAddress;

   m_mapOld = m_mapNew;
   m_mapNew.clear();

   source = m_ptrTrackedCollection->m_ptrCollectionSource;
   colAddress = m_ptrTrackedCollection->GetIdentifier();

   lstNewItems = m_ptrTrackedCollection->getCollection();
   for( iter_Item  = lstNewItems.begin();
        iter_Item != lstNewItems.end();
        iter_Item++ )
   {
      CollectionItem* item = source->GetCardPrototype(*iter_Item);
      RealCopyList lstCList = item->
         GetCopiesForCollection(colAddress, CollectionItemType::All);
      for ( iter_Copy  = lstCList.begin(); 
            iter_Copy != lstCList.end();
            iter_Copy++ )
      {
         // Map a copy of the current state so that it may be used to track.
         m_mapNew[*iter_Item].push_back(std::make_pair(*iter_Copy, CopyItem(**iter_Copy)));
      }
   }

}

CollectionDeltaClass 
CollectionTracker::CalculateChanges()
{
   std::function<CopyItem*(SnapShot)> fnPtrCmper;
   std::map<unsigned int, CopyList> mapOld(m_mapOld);
   std::map<unsigned int, CopyList> mapNew(m_mapNew);
   std::map<unsigned int, CopyList>::iterator iter_FoundItem;

   std::map<unsigned int, CopyList> mapAdded;
   std::map<unsigned int, CopyList> mapChanged;
   std::map<unsigned int, CopyList> mapChangedPrevState;
   std::map<unsigned int, CopyList> mapRemoved;

   Address colAddress;
   CollectionSource* colSource;

   CopyList::iterator iter_CopyCurrent;
   CopyList::iterator iter_CopyOld;

   fnPtrCmper = [](SnapShot sptr)->CopyItem* { return sptr.first.get(); };

   std::map<unsigned int, CopyList>::iterator iter_Current;
   for( iter_Current  = mapNew.begin(); 
        iter_Current != mapNew.end();
        iter_Current++ )
   {
      iter_FoundItem = mapOld.find(iter_Current->first);
      if (iter_FoundItem != mapOld.end())
      {
         // Excellent, this type of thing exists both before
         // and after.
         for (iter_CopyCurrent  = iter_Current->second.begin();
              iter_CopyCurrent != iter_Current->second.end();
              iter_CopyCurrent++)
         {
            CopyItem* cToFind = iter_CopyCurrent->first.get();
            int iFound = ListHelper::List_Find( cToFind,
                                                iter_FoundItem->second, 
                                                fnPtrCmper );
            if (iFound != -1 )
            {
               CopyItem cFound = iter_FoundItem->second.at(iFound).second;
               if (cToFind->GetHash() != cFound.GetHash())
               {
                  // If this item has been changed since last track.
                  // Otherwise, do nothing.
                  mapChanged[iter_Current->first].
                     push_back(*iter_CopyCurrent);

                  mapChangedPrevState[iter_Current->first].
                     push_back(iter_FoundItem->second[iFound]);
               }

               // Erase the item from mapOld.
               iter_FoundItem->
                  second.erase(iter_FoundItem->second.begin() + iFound);
            }
            else
            {
               // This item has been added.
               mapAdded[iter_Current->first].push_back(*iter_CopyCurrent);
            }
         }
      }
      else
      {
         // All these items have been added since last track.
         for (iter_CopyCurrent  = iter_Current->second.begin();
              iter_CopyCurrent != iter_Current->second.end();
              iter_CopyCurrent++)
         {
            mapAdded[iter_Current->first].push_back(*iter_CopyCurrent);
         }
      }
   }

   // Anything left in mapOld has been removed since last track.
   std::map<unsigned int, CopyList>::iterator iter_Old;
   for (iter_Old  = mapOld.begin();
        iter_Old != mapOld.end();
        iter_Old++)
   {
      for (iter_CopyOld  = iter_Old->second.begin();
           iter_CopyOld != iter_Old->second.end();
           iter_CopyOld++)
      {
         mapRemoved[iter_Old->first].push_back(*iter_CopyOld);
      }
   }

   // All items have been accounted for.
   // We have added, removed, and before+afters
   // Create the strings for each
   std::vector<std::string> lstAdds;
   std::vector<std::string> lstChanges;
   std::vector<std::string> lstRemoves;
   std::string szNote = "";

   colAddress = m_ptrTrackedCollection->GetIdentifier();
   colSource = m_ptrTrackedCollection->m_ptrCollectionSource;

   CopyList::iterator iter_Copy;
   std::map<unsigned int, CopyList>::iterator iter_Items;
   for ( iter_Items  = mapAdded.begin();
         iter_Items != mapAdded.end();
         iter_Items++ )
   {
      CollectionItem* item = colSource->GetCardPrototype(iter_Items->first);
      for (iter_Copy  = iter_Items->second.begin();
           iter_Copy != iter_Items->second.end();
           iter_Copy++)
      {
         szNote = "+ " + CollectionItem::ToCardLine(
            (*iter_Copy).first->GetAddress(),
            item->GetName(),
            (*iter_Copy).first->GetIdentifyingAttributes(),
            (*iter_Copy).first->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstAdds.push_back(szNote);
      }
   }

   for (iter_Items  = mapRemoved.begin();
        iter_Items != mapRemoved.end();
        iter_Items++)
   {
      CollectionItem* item = colSource->GetCardPrototype(iter_Items->first);
      for (iter_Copy  = iter_Items->second.begin();
           iter_Copy != iter_Items->second.end();
           iter_Copy++)
      {
         szNote = "- " + CollectionItem::ToCardLine(
            (*iter_Copy).first->GetAddress(),
            item->GetName(),
            (*iter_Copy).first->GetIdentifyingAttributes(),
            (*iter_Copy).first->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstRemoves.push_back(szNote);
      }
   }

   for (iter_Items = mapChanged.begin();
        iter_Items != mapChanged.end();
        iter_Items++)
   {
      CollectionItem* item = colSource->GetCardPrototype(iter_Items->first);
      for (iter_Copy  = iter_Items->second.begin();
           iter_Copy != iter_Items->second.end();
           iter_Copy++)
      {
         szNote = "% " + CollectionItem::ToCardLine(
            (*iter_Copy).second.GetAddress(),
            item->GetName(),
            (*iter_Copy).second.GetIdentifyingAttributes(),
            (*iter_Copy).second.GetMetaTags(MetaTagType::Visible),
            colAddress);
         szNote += "-> " + CollectionItem::ToCardLine(
            (*iter_Copy).first->GetAddress(),
            item->GetName(),
            (*iter_Copy).first->GetIdentifyingAttributes(),
            (*iter_Copy).first->GetMetaTags(MetaTagType::Visible),
            colAddress);
         lstChanges.push_back(szNote);
      }
   }

   CollectionDeltaClass deltas;
   deltas.Additions = lstAdds;
   deltas.Changes = lstChanges;
   deltas.Removals = lstRemoves;

   return deltas;
}

