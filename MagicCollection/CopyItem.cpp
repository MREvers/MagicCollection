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

// Returns the hash. Hashes on parent, PUBLIC (so not the parent TAG) metatags, and the idattrs.
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

std::string CopyItem::GetSession() const
{
   return GetMetaTag( GetSessionKey(), Tracking );
}

std::string CopyItem::GetUID() const
{
   return GetMetaTag( GetUIDKey(), Tracking );
}

string CopyItem::GetParent() const
{
   return m_Address.GetFullAddress();
}

Address CopyItem::GetAddress() const
{
   return m_Address;
}

// Returns true if this copy item resides DIRECTLY within the input collection.
// i.e. A-2 is a parent of this object if this has an address of A-6.
bool CopyItem::IsParent(const Address& aAddrParent) const
{
   return m_Address.IsResidentIn(aAddrParent);
}

// Sets the parent address AND adds it to residents
void CopyItem::SetParent( const Address& aAddrTestAddress )
{
   string szParent = aAddrTestAddress.GetFullAddress();
   setParent( szParent );
   setMetaTag( GetAddressKey(), szParent, Public );
   
   if( !IsReferencedBy( aAddrTestAddress ) )
   {
      AddResident(aAddrTestAddress);
   }

   itemChanged();
}

// This will detect if the adding 'resident' is a subset of the parent,
// if so, it will adjust the parent address.
void CopyItem::AddResident(const Address& aAddrAddress)
{
   Addresser addr;
   // First check the main address, then try any child address
   addr.InceptLocation( m_Address, aAddrAddress );

   bool AddedToRef = false;
   for( int i = 0; i < m_lstResidentIn.size(); i++ )
   {
      AddedToRef |= addr.InceptLocation( m_lstResidentIn[i], aAddrAddress );
   }

   if( !AddedToRef )
   {
      m_lstResidentIn.push_back(aAddrAddress);
   }
}

// Returns the number of collection chains this copy is referenced in.
// Main collection counts as one.
int CopyItem::RemoveResident(const Address& aAddrAddress)
{
   Addresser addr;
   bool RemovedFromChildCol = addr.PitheLocation(m_Address, aAddrAddress);

   vector<int> lstRemoveAddrs;
   for( int i = 0; i < m_lstResidentIn.size(); i++ )
   {
      if( addr.PitheLocation( m_lstResidentIn.at( i ), aAddrAddress ) )
      {
         if( m_lstResidentIn.at( i ).IsEmpty() )
         {
            lstRemoveAddrs.push_back(i);
         }
         break;
      }
   }

   for( auto iLoc : lstRemoveAddrs )
   {
      m_lstResidentIn.erase( m_lstResidentIn.begin() + iLoc );
   }
 

   return ( m_Address.IsEmpty() ? 0 : 1 ) + m_lstResidentIn.size();
}

std::vector<Address> CopyItem::GetResidentIn() const
{
   return m_lstResidentIn;
}

// Returns true if this card is located in aAddrTest or if
// it is referenced in aAddrTest.
bool CopyItem::IsResidentIn(const Address& aAddrTest) const
{
   Addresser addr;
   Address aDummy;
   bool isResident = addr.DoesAddressIncludeLocation(m_Address, aAddrTest, aDummy);

   if( !isResident )
   {
      isResident |= IsReferencedBy(aAddrTest);
   }

   return isResident;
}

// Returns true if any resident references the input location.
bool CopyItem::IsReferencedBy(const Address& aAddrTest) const
{
   Addresser addr;
   Address aDummy;
   bool isResident = false;

   for( auto resident : m_lstResidentIn )
   {
      isResident |= addr.DoesAddressIncludeLocation( resident, aAddrTest, aDummy );
      if( isResident ){ break; }
   }

   return isResident;
}


CopyItem* CopyItem::CreateCopyItem( CollectionItem const* aoConstructor,
                                    const Address& aAddrParentIdentifier,
                                    const std::vector<Tag>& alstIDAttrs,
                                    const std::vector<Tag>& alstMetaTags )
{
   CopyItem* newCopy = new CopyItem( aAddrParentIdentifier, alstMetaTags );

   aoConstructor->SetIdentifyingTraitDefaults(newCopy);

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

std::string 
CopyItem::MakeIgnoredTag( const std::string aszTag )
{
   return Config::IgnoredTagId + aszTag;
}

std::string 
CopyItem::MakeTrackingTag( const std::string aszTag )
{
   return Config::TrackingTagId + aszTag;
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
      // Delete the tag if the value is empty.
      if( aszVal == "" )
      {
         m_lstMetaTags.erase(m_lstMetaTags.begin() + iFound);
      }
      else
      {
         m_lstMetaTags[iFound].SetVal(aszVal);
      }
   }
   m_bNeedHash = true;
}