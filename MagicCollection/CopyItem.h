#pragma once
#include <string>
#include <vector>
#include <functional>

#include "MetaTag.h"
#include "Config.h"
#include "StringHelper.h"
#include "TraitItem.h"
#include "Addresser.h"

class CopyItem
{
public:
   CopyItem(std::vector<TraitItem>* alstTraits,
      Address aAddrParentIdentifier);
   CopyItem(std::vector<TraitItem>* alstTraits,
      Address aAddrParentIdentifier,
      std::vector<Tag> alstAttrs,
      std::vector<Tag> alstMetaTags = std::vector<Tag>());
   ~CopyItem();

   std::string GetHash();

   std::string GetParent();
   Address GetAddress();
   bool IsParent(Address aAddrTestAddress);

   void AddResident(Address aAddrAddress);
   void RemoveResident(Address aAddrAddress);
   std::vector<Address> GetResidentIn();
   bool IsResidentIn(Address aAddrAddress);

   void SetMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType, bool bTimeChange = true);
   std::string GetMetaTag(std::string aszKey, MetaTagType atagType);
   std::vector<Tag> GetMetaTags(MetaTagType atagType);

   bool SetIdentifyingAttribute(std::string aszKey, std::string aszValue, bool bTimeChange = true);
   std::string GetIdentifyingAttribute(std::string aszKey);
   std::vector<Tag> GetIdentifyingAttributes();

   std::function<std::string(MetaTag)> GetMetaTagValueViewer(MetaTagType atagType);
   std::function<std::string(MetaTag)> GetMetaTagKeyViewer();

   static MetaTagType DetermineMetaTagType(std::string aszTagKey);
private:
   bool m_bNeedHash;

   // Includes <Address>-<SubAddress1>,<SubAddress2>...
   // SubAddressX's smallest prime factor is the xth prime.
   Address m_Address;
   std::vector<Address> m_lstResidentIn;
   std::vector<Address*> getAddresses();

   void itemChanged();
   void setChainID(std::string aszNewID);
   void setParent(std::string aszNewParent);
   void _setParent(std::string aszNewParent);

   // Metatags are visible by all collections. They 'may' be used to identify the card.
   // In general, meta-tags are attached to some physical meaning related to the card such as
   // aquisition date, price, etc...
   std::vector<MetaTag> m_lstMetaTags;

   // Note that these will correspond 1-1 to each traititem in m_plstRest...
   std::vector<Tag> m_lstIdentifyingTags;
   std::vector<TraitItem>* m_plstRestrictedTraits;
   void setPairedAttributes(std::string aszKey, int iVal);
};

