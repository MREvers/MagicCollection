#include "CopyItem.h"
#include <time.h>
#include <sstream>
#include <cstdlib>
#include "Addresser.h"
#include "CollectionItem.h"

using namespace std;

CopyItem::CopyItem( const Address& aAddrParentIdentifier )
{
   Config* config = Config::Instance();
   Addresser addr;

   SetParent(aAddrParentIdentifier);

   // Set the chain ID and session here. 
   // If one is set later, it will just overwrite this...
   setUID(config->GetHexID(addr.GetRandom()));
   itemChanged();

   m_bNeedHash = true;
}


CopyItem::~CopyItem()
{
   m_lstMetaTags.clear();
}

CopyItem::CopyItem( const Address& aAddrParentIdentifier, 
                    const std::vector<Tag>& alstMetaTags )
   : CopyItem(aAddrParentIdentifier)
{
   for ( auto attr : alstMetaTags )
   {
      SetMetaTag( attr.first, attr.second, 
                  DetermineMetaTagType(attr.first), 
                  false );
   }
}

string CopyItem::GetHash()
{
   function<string(const MetaTag&)> fnExtractor = GetMetaTagKeyViewer();
   int iMetaHash = ListHelper::List_Find(string(Config::HashKey), m_lstMetaTags, fnExtractor);

   if ( ( iMetaHash == -1 ) || ( m_bNeedHash ) )
   {
      string szHashString = m_Address.GetFullAddress();
      vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
      for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
      {
         // This requires that the tags have an ordering.
         // This ordering can be determined, by the order
         // of the MetaTag object in the collection object.
         szHashString += iter_Tags->second;
      }

      // Only iterate of public metatags
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

std::string CopyItem::GetUID() const
{
   return GetMetaTag( GetUIDKey(), Hidden );
}

string CopyItem::GetParent() const
{
   return m_Address.GetFullAddress();
}

Address CopyItem::GetAddress() const
{
   return m_Address;
}

bool CopyItem::IsParent(const Address& aAddrParent) const
{
   Addresser addr;
   Address aDummy;
   return addr.DoesLocationContain(aAddrParent, m_Address, aDummy);
}

void CopyItem::SetParent( const Address& aAddrTestAddress )
{
   string szParent = aAddrTestAddress.GetFullAddress();
   setParent( szParent );
   SetMetaTag( GetAddressKey(), szParent, Public );
   itemChanged();
}

// This will detect if the adding 'resident' is a subset of the parent,
// if so, it will adjust the parent address.
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

std::vector<Address> CopyItem::GetResidentIn() const
{
   return m_lstResidentIn;
}

bool CopyItem::IsResidentIn(const Address& aAddrTest) const
{
   Addresser addr;
   Address aDummy;
   return addr.IsResidentIn(m_Address, aAddrTest, aDummy);
}

CopyItem* CopyItem::CreateCopyItem( CollectionItem const* aoConstructor,
                                    const Address& aAddrParentIdentifier,
                                    const std::vector<Tag>& alstIDAttrs,
                                    const std::vector<Tag>& alstMetaTags )
{
   CopyItem* newCopy = new CopyItem( aAddrParentIdentifier, alstMetaTags );

   for( auto IDTag : alstIDAttrs )
   {
      aoConstructor->SetIdentifyingTrait( newCopy, IDTag.first, IDTag.second );
   }

   return newCopy;
}

MetaTagType CopyItem::DetermineMetaTagType(string aszTagKey)
{
   MetaTagType mTagType = Public;
   if( ( aszTagKey.size() > 0 ) && 
       ( aszTagKey[0] == *Config::IgnoredTagId ) )
   {
      mTagType = Ignored;
   }

   if( ( mTagType == Ignored ) &&
       ( aszTagKey.size() > 1 ) &&
       ( aszTagKey[1] == *Config::TrackingTagId ) )
   {
      mTagType = Tracking;
   }
   return mTagType;
}

std::string CopyItem::GetUIDKey()
{
   return Config::TrackingTagId + std::string("UID");
}

std::string CopyItem::GetSessionKey()
{
   return Config::TrackingTagId + std::string("Session");
}

std::string CopyItem::GetHashKey()
{
   return Config::TrackingTagId + std::string("hash");
}

std::string CopyItem::GetAddressKey()
{
   return "Address";
}

void 
CopyItem::SetMetaTag( const std::string& aszKey,
                      const std::string& aszVal,
                      MetaTagType atagType,
                      bool bTimeChange )
{
   if( aszKey == GetAddressKey() )
   {
      setParent( aszVal );
   }
   else
   {
      setMetaTag( aszKey, aszVal, atagType, bTimeChange );
   }
}

string CopyItem::GetMetaTag(const std::string& aszKey, MetaTagType atagType) const
{
   auto fnExtractor = GetMetaTagKeyViewer();
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

vector<Tag> CopyItem::GetMetaTags(MetaTagType atagType) const
{
   vector<Tag> lstRetVal;
   vector<MetaTag>::const_iterator iter_Tags = m_lstMetaTags.cbegin();
   for (; iter_Tags != m_lstMetaTags.cend(); ++iter_Tags)
   {
      if (iter_Tags->CanView(atagType))
      {
         lstRetVal.push_back(make_pair(iter_Tags->GetKey(), iter_Tags->GetVal()));
      }
   }

   return lstRetVal;
}

bool
CopyItem::SetIdentifyingAttribute( const std::string& aszKey,
                                   const std::string& aszValue,
                                   bool bTimeChange )
{
   if (bTimeChange) { itemChanged(); }
   int iFound = ListHelper::List_Find(aszKey, m_lstIdentifyingTags,
                                      Config::Instance()->GetTagHelper(Key));

   if( iFound == -1 )
   {
      m_lstIdentifyingTags.push_back( std::make_pair( aszKey, aszValue ) );
   }
   else
   {
      m_lstIdentifyingTags[iFound].second = aszValue;
   }

   return true;
}

string CopyItem::GetIdentifyingAttribute(string aszKey)
{
   int iFound = ListHelper::List_Find(aszKey, m_lstIdentifyingTags,
                                      Config::Instance()->GetTagHelper(Key));
   if (iFound != -1)
   {
      return m_lstIdentifyingTags.at(iFound).second;
   }
   else
   {
      return Config::NotFoundString;
   }
}

vector<Tag> CopyItem::GetIdentifyingAttributes() const
{
   return m_lstIdentifyingTags;
}

function<string(const MetaTag&)> CopyItem::GetMetaTagValueViewer(MetaTagType atagType)
{
   const static function<string(const MetaTag& )> fnValView = 
      [atagType](const MetaTag& atag )->string { return atag.GetVal( atagType ); };
   return fnValView;
}

function<string(const MetaTag&)> CopyItem::GetMetaTagKeyViewer()
{
   const static function<string( const MetaTag& )> fnKeyView = 
      []( const MetaTag& atag )->string { return atag.GetKey(); };
   return fnKeyView;
}

void CopyItem::itemChanged()
{
   long lChangeTime = time(nullptr);
   stringstream ss;
   ss << hex << lChangeTime ;
   SetMetaTag(GetSessionKey(), ss.str(), MetaTagType::Tracking, false);
}

void CopyItem::setUID(string aszNewID)
{
   SetMetaTag(GetUIDKey(), aszNewID, MetaTagType::Tracking);
}

void CopyItem::setParent(string aszNewParent)
{
   Address newAddress( aszNewParent );

   m_Address = newAddress;
}

void 
CopyItem::setMetaTag( const std::string& aszKey,
                      const std::string& aszVal,
                      MetaTagType atagType,
                      bool bTimeChange )
{
   const static function<int(const MetaTag&, MetaTag&)> fnComparer = 
      [](const MetaTag& atag1, MetaTag& atag2)-> int { return atag1.GetKey().compare(atag2.GetKey()); };
   function<string( const MetaTag& )> fnExtractor = GetMetaTagKeyViewer();

   if (bTimeChange) { itemChanged(); }

   int iFound = ListHelper::List_Find(aszKey, m_lstMetaTags, fnExtractor);
   if (iFound == -1)
   {
      MetaTag newMeta(aszKey, aszVal, atagType);
      ListHelper::List_Insert(newMeta, m_lstMetaTags, fnComparer);
   }
   else if (m_lstMetaTags[iFound].CanView(atagType))
   {
      m_lstMetaTags[iFound].SetVal(aszVal);
   }
   m_bNeedHash = true;
}