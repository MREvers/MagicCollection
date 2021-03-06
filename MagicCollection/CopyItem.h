#pragma once
#include <string>
#include <vector>
#include <functional>

#include "MetaTag.h"
#include "Config.h"
#include "StringHelper.h"
#include "TraitItem.h"
#include "Addresser.h"

class CollectionItem;

// Each Copy Item has a Unique ID and a Class Hash ID and its last modified session date.
// UID is a 3 digit hex number, this is also called the Chain ID.
// Hash ID is an MD5 has of various traits.
// Last modified session date is an integer.
class CopyItem
{
public:
   enum RemoveAddressType : int
   {
      Individual = 0x0,
      Family = 0x1
   };

   enum HashType : int
   {
      Ids = 0x2,
      Meta = 0x4,
      Default = 0x7
   };

private:
   // Use static constructor to build this class.
   CopyItem( const Identifier& aAddrParentIdentifier );
   CopyItem( const Identifier& aAddrParentIdentifier, 
             const std::vector<Tag>& alstMetaTags );

public:
   ~CopyItem();

   std::string GetHash(HashType aiHashType = Default);
   std::string GetSession() const;
   Address GetAddress() const;
   std::string GetUID() const;

   bool IsVirtual() const;

   void SetParent( const Identifier& aAddrTestAddress );
   bool IsParent( const Location& aAddrTestAddress ) const;
   std::string GetParent() const;

   void AddResident(const Identifier& aAddrAddress);
   int RemoveResident( const Identifier& aAddrAddress,
                       RemoveAddressType aiRemoveType = Individual );
   bool IsResidentIn( const Location& aAddrAddress ) const;
   bool IsReferencedBy( const Location& aAddrAddress ) const;
   std::vector<Address> GetResidentIn() const;

   void SetMetaTag( const std::string& aszKey,
                    const std::string& aszVal,
                    MetaTagType atagType,
                    bool bTimeChange = true );
   std::string GetMetaTag(const std::string& aszKey, MetaTagType atagType) const;
   std::vector<Tag> GetMetaTags(MetaTagType atagType) const;

   // This expects that the input is valid. Does not verify that
   // the value is an allowed value.
   bool SetIdentifyingAttribute( const std::string& aszKey,
                                 const std::string& aszValue,
                                 bool bTimeChange = true );
   std::string GetIdentifyingAttribute(std::string aszKey);
   std::vector<Tag> GetIdentifyingAttributes() const;
private:
   bool m_bNeedHash;

   // Includes <Address>-<SubAddress1>,<SubAddress2>...
   // SubAddressX's smallest prime factor is the xth prime.
   Address m_Address;
   std::vector<Address> m_lstResidentIn;

   void itemChanged();
   void setUID(std::string aszNewID);
   void setParent(std::string aszNewParent);
   void setMetaTag( const std::string& aszKey,
                    const std::string& aszVal,
                    MetaTagType atagType,
                    bool bTimeChange = true );
   int findFamilyMember(const Identifier& aId) const;

   std::vector<MetaTag> m_lstMetaTags;
   std::vector<Tag> m_lstIdentifyingTags;
public:
   static CopyItem* CreateCopyItem( CollectionItem const* aoConstructor,
                                    const Identifier& aAddrParentIdentifier,
                                    const std::vector<Tag>& alstIDAttrs,
                                    const std::vector<Tag>& alstMetaTags );

   static MetaTagType DetermineMetaTagType(std::string aszTagKey);

   static std::string GetUIDKey();
   static std::string GetSessionKey();
   static std::string GetHashKey();
   static std::string GetAddressKey();
   static std::string MakeIgnoredTag(const std::string aszTag);
   static std::string MakeTrackingTag(const std::string aszTag);

   static std::function<std::string(const MetaTag&)> GetMetaTagValueViewer(MetaTagType atagType);
   static std::function<std::string(const MetaTag&)> GetMetaTagKeyViewer();
};

