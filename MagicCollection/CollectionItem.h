#pragma once
#include <string>
#include "CopyItem.h"
#include "Config.h"
#include "TraitItem.h"
#include "StringHelper.h"
#include "Addresser.h"

enum CollectionItemType : int
{
   Local = 0x1,
   Borrowed = 0x2,
   Real = 0x3,
   Virtual = 0x4,
   All = 0xF
};

class CollectionItem
{
public:
   class PseudoIdentifier
   {
   public:
      PseudoIdentifier();
      PseudoIdentifier(unsigned int aiCount, std::string aszName, std::string aszDetails, std::string aszMeta);
      ~PseudoIdentifier();

      unsigned int Count;
      std::string Name;
      std::string DetailString;
      std::string MetaString;
      std::vector<Tag> Identifiers;
      std::vector<Tag> MetaTags;
   };

   CollectionItem(std::string aszItemName, std::vector<Tag> alstCommon, std::vector<TraitItem> alstRestrictions);
   ~CollectionItem();

   std::string GetName();

   CopyItem* AddCopyItem( const Address& aAddrColID,
                          std::vector<Tag> alstAttrs = std::vector<Tag>(),
                          std::vector<Tag> alstMetaTags = std::vector<Tag>() );

   CopyItem* GenerateCopy( const Address& aAddrColID,
                           std::vector<Tag> alstAttrs = std::vector<Tag>(),
                           std::vector<Tag> alstMetaTags = std::vector<Tag>() );

   void RemoveCopyItem(const Address& aAddrColID, std::string aszHash);
   void RemoveResidentFromItem(CopyItem* acItem, const Address& aAddrColID);

   // Finds First
   CopyItem* FindCopyItem(std::string aszHash, const Address& aPairResi = Address());
   std::vector<CopyItem*> FindAllCopyItems(std::string aszHash, const Address& aPairResi = Address());
   void Erase(CopyItem* ociRemove);

   std::vector<CopyItem*> GetCopiesForCollection(const Address& aAddrCollectionIdentifier, CollectionItemType aItemType);

   std::string GetHash(const Address& aAddrIdentifier,
      std::vector<Tag> alstAttrs = std::vector<Tag>(),
      std::vector<Tag> alstMetaTags = std::vector<Tag>());

   std::string GetCardString(CopyItem* aItem, MetaTagType aTagType = Visible, const Address& aAddrCompareID = Address());
   std::string GetProtoTypeString();
   std::vector<Tag> GetCommonTraits();

   static bool ParseCardLine(std::string aszLine, PseudoIdentifier& rPIdentifier);
   static bool ParseTagString(std::string aszDetails, std::vector<Tag>& rlstTags);
   static std::string ToCardLine( const Address& aAddrParentID, 
                                  std::string aszName,
                                  std::vector<Tag> alstAttrs = std::vector<Tag>(),
                                  std::vector<Tag> alstMetaTags = std::vector<Tag>(), 
                                  const Address& aAddrCompareID = Address() );

private:
   std::string m_szName;

   std::vector<CopyItem*> m_lstCopies;

   // We are guaranteed that identifying traits are not themselves lists.
   // If a list is not an identifying trait. It is kept as "thing1::thing2"
   std::vector<Tag> m_lstCommonTraits;
   std::vector<TraitItem> m_lstIdentifyingTraits;
};

