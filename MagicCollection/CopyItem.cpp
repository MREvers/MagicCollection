#include "CopyItem.h"
#include "Addresser.h"
#include <time.h>
#include <sstream>
#include <cstdlib>

using namespace std;

CopyItem::CopyItem(vector<TraitItem>* alstTraits, Address aAddrParentIdentifier)
{
   Addresser addr;

   m_lstResidentIn.push_back(aAddrParentIdentifier);
   m_plstRestrictedTraits = alstTraits;

   // Use the interface that is used to load the parent from the meta file.
   m_Address = aAddrParentIdentifier;

   vector<TraitItem>::iterator iter_DefaultVals = m_plstRestrictedTraits->begin();
   for (; iter_DefaultVals != m_plstRestrictedTraits->end(); ++iter_DefaultVals)
   {
      m_lstIdentifyingTags.push_back(make_pair(iter_DefaultVals->GetKeyName(),
                                               iter_DefaultVals->GetDefaultValue()));
   }

   // Set the chain ID and session here. If one is set later, it will just overwrite this...
   setChainID(to_string(addr.GetRandom()));
   itemChanged();

   m_bNeedHash = true;
}


CopyItem::~CopyItem()
{
   m_lstMetaTags.clear();
}

CopyItem::CopyItem(vector<TraitItem>* alstTraits,
   Address aAddrParentIdentifier,
   vector<Tag> alstAttrs,
   vector<Tag> alstMetaTags)
   : CopyItem(alstTraits, aAddrParentIdentifier)
{
   vector<Tag>::iterator iter_Attrs = alstAttrs.begin();
   for (; iter_Attrs != alstAttrs.end(); 
          ++iter_Attrs)
   {
      SetIdentifyingAttribute( iter_Attrs->first,
                               iter_Attrs->second, 
                               false );
   }

   for ( iter_Attrs = alstMetaTags.begin(); 
         iter_Attrs != alstMetaTags.end(); 
         ++iter_Attrs )
   {
      SetMetaTag( iter_Attrs->first, 
                  iter_Attrs->second, 
                  DetermineMetaTagType(iter_Attrs->first), 
                  false );
   }

   // Populate the Hash metatag for convenience.
   GetHash();
}

string CopyItem::GetHash()
{
   function<string(MetaTag)> fnExtractor
      = GetMetaTagKeyViewer();
   int iMetaHash = ListHelper::List_Find(
      string(Config::HashKey),
      m_lstMetaTags,
      fnExtractor);

   if (iMetaHash == -1 || m_bNeedHash)
   {
      string szHashString = m_Address.GetFullAddress();
      vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
      for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
      {
         // This requires that the tags have an ordering. This ordering can be determined,
         // by the order of the MetaTag object in the collection object.
         szHashString += iter_Tags->second;
      }

      vector<Tag> lstMetaList = this->GetMetaTags(Public);
      vector<Tag>::iterator iter_MetaTags = lstMetaList.begin();
      for (; iter_MetaTags != lstMetaList.end(); ++iter_MetaTags)
      {
         szHashString += iter_MetaTags->first + iter_MetaTags->second;
      }
      string szHash = Config::Instance()->GetHash(szHashString);
      SetMetaTag(Config::HashKey, szHash, Hidden, false);

      m_bNeedHash = false;
      return szHash;
   }
   else
   {
      return m_lstMetaTags[iMetaHash].GetVal();
   }
}

string  CopyItem::GetParent()
{
   return m_Address.GetFullAddress();
}

Address CopyItem::GetAddress()
{
   return m_Address;
}

bool CopyItem::IsParent(Address aAddrParent)
{
   Addresser addr;
   Address aDummy;
   return addr.DoesLocationContain(aAddrParent, m_Address, aDummy);
}

// This will detect if the adding 'resident' is a subset of the parent, if so, it will adjust the parent address.
void CopyItem::AddResident(const Address& aAddrAddress)
{
   Addresser addr;
   addr.InceptLocation(m_Address, aAddrAddress);
}

void CopyItem::RemoveResident(const Address& aAddrAddress)
{
   Addresser addr;
   addr.PitheLocation(m_Address, aAddrAddress);
}

std::vector<Address> CopyItem::GetResidentIn()
{
   return m_lstResidentIn;
}

bool CopyItem::IsResidentIn(const Address& aAddrTest)
{
   Addresser addr;
   Address aDummy;
   return addr.IsResidentIn(m_Address, aAddrTest, aDummy);
}

MetaTagType CopyItem::DetermineMetaTagType(string aszTagKey)
{
   MetaTagType mTagType = Public;
   if (aszTagKey.size() > 0 && aszTagKey[0] == '_')
   {
      mTagType = Ignored;
   }
   if (mTagType == Ignored && aszTagKey.size() > 1 && aszTagKey[1] == '_')
   {
      mTagType = Tracking;
   }
   return mTagType;
}

void CopyItem::SetMetaTag(string aszKey, string aszVal, MetaTagType atagType, bool bTimeChange)
{
   if (bTimeChange) { itemChanged(); }

   function<int(MetaTag, MetaTag)> fnComparer;
   function<string(MetaTag)> fnExtractor =
      [](MetaTag atag1)-> string { return atag1.GetKey(); };

   if (aszKey == "Address") { _setParent(aszVal); }

   int iFound = ListHelper::List_Find(aszKey, m_lstMetaTags, fnExtractor);
   if (iFound == -1)
   {
      fnComparer = [](MetaTag atag1, MetaTag atag2)-> int { return atag1.GetKey().compare(atag2.GetKey()); };

      MetaTag newMeta(aszKey, aszVal, atagType);
      ListHelper::List_Insert(newMeta, m_lstMetaTags, fnComparer);
   }
   else if (m_lstMetaTags[iFound].CanView(atagType))
   {
      m_lstMetaTags[iFound].SetVal(aszVal);
   }
   m_bNeedHash = true;
}

string CopyItem::GetMetaTag(string aszKey, MetaTagType atagType)
{
   function<string(MetaTag)> fnExtractor =
      [](MetaTag atag)-> string { return atag.GetKey(); };
   int iFound = ListHelper::List_Find(aszKey, m_lstMetaTags, fnExtractor);
   if (iFound != -1)
   {
      return m_lstMetaTags[iFound].GetVal(atagType);
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag> CopyItem::GetMetaTags(MetaTagType atagType)
{
   vector<Tag> lstRetVal;
   vector<MetaTag>::iterator iter_Tags = m_lstMetaTags.begin();
   for (; iter_Tags != m_lstMetaTags.end(); ++iter_Tags)
   {
      if (iter_Tags->CanView(atagType))
      {
         lstRetVal.push_back(make_pair(iter_Tags->GetKey(), iter_Tags->GetVal()));
      }
   }

   return lstRetVal;
}

bool CopyItem::SetIdentifyingAttribute(string aszKey, string aszValue, bool bTimeChange)
{
   if (bTimeChange) { itemChanged(); }

   function<string(TraitItem)> fnExtractor = [](TraitItem aTI)->string { return aTI.GetKeyName(); };
   int iIsAttr = ListHelper::List_Find(aszKey, *m_plstRestrictedTraits, fnExtractor);
   if (iIsAttr != -1)
   {
      TraitItem foundTrait = m_plstRestrictedTraits->at(iIsAttr);
      if (foundTrait.IsAllowedValue(aszValue))
      {
         m_lstIdentifyingTags[iIsAttr].second = aszValue;
         int iValueIndex = ListHelper::List_Find(aszValue, foundTrait.GetAllowedValues());
         setPairedAttributes(aszKey, iValueIndex);
         m_bNeedHash = true;
         return true;
      }
   }

   return false;
}

string CopyItem::GetIdentifyingAttribute(string aszKey)
{
   int iFound = ListHelper::List_Find(aszKey, m_lstIdentifyingTags, Config::Instance()->GetTagHelper(Key));
   if (iFound != -1)
   {
      return m_lstIdentifyingTags.at(iFound).second;
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag> CopyItem::GetIdentifyingAttributes()
{
   vector<Tag> lstRetVal;
   vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
   for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
   {
      lstRetVal.push_back(make_pair(iter_Tags->first, iter_Tags->second));
   }

   return lstRetVal;
}

function<string(MetaTag)> CopyItem::GetMetaTagValueViewer(MetaTagType atagType)
{
   return [atagType](MetaTag atag)->string { return atag.GetVal(atagType); };
}

function<string(MetaTag)> CopyItem::GetMetaTagKeyViewer()
{
   return [](MetaTag atag)->string { return atag.GetKey(); };
}

std::vector<Address*> CopyItem::getAddresses()
{
   std::vector<Address*> lstRetval;
   lstRetval.push_back(&m_Address);

   size_t iResis = m_lstResidentIn.size();
   for (size_t i = 0; i < iResis; i++)
   {
      lstRetval.push_back(&m_lstResidentIn.at(i));
   }

   return lstRetval;
}

void CopyItem::itemChanged()
{
   long lChangeTime = time(nullptr);
   stringstream ss;
   ss << hex << lChangeTime ;
   SetMetaTag("__Session", ss.str(), MetaTagType::Tracking, false);
}

void CopyItem::setChainID(string aszNewID)
{
   SetMetaTag("__ChainID", aszNewID, MetaTagType::Tracking);
}

void CopyItem::setParent(string aszNewParent)
{
   SetMetaTag("Address", aszNewParent, MetaTagType::Public);
}

void CopyItem::_setParent(string aszNewParent)
{
   Addresser addr;
   Address pID(aszNewParent);
   AddResident(pID);
}

void CopyItem::setPairedAttributes(string aszKey, int iVal)
{
   function<string(TraitItem)> fnExtractor = [](TraitItem aTI)->string { return aTI.GetKeyName(); };
   vector<string> lstPartners;
   vector<Tag> lstPairs = Config::Instance()->GetPairedKeysList();
   for each (Tag var in lstPairs)
   {
      if (var.first == aszKey && ListHelper::List_Find(var.second, lstPartners) == -1)
      {
         lstPartners.push_back(var.second);
      }
      else if (var.second == aszKey && ListHelper::List_Find(var.first, lstPartners) == -1)
      {
         lstPartners.push_back(var.first);
      }
   }

   for each (string szKey in lstPartners)
   {
      int iIsAttr = ListHelper::List_Find(szKey, *m_plstRestrictedTraits, fnExtractor);
      if (iIsAttr != -1)
      {
         TraitItem foundTrait = m_plstRestrictedTraits->at(iIsAttr);
         m_lstIdentifyingTags[iIsAttr].second = foundTrait.GetAllowedValues().at(iVal);
      }
   }
}