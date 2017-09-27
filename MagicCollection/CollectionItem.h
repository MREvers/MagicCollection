#pragma once
#include <string>
#include <memory>
#include "CopyItem.h"
#include "Config.h"
#include "TraitItem.h"
#include "StringHelper.h"
#include "Addresser.h"
#include "TryGet.h"

enum CollectionItemType : int
{
   Local = 0x1,
   Borrowed = 0x2,
   Real = 0x3,
   Virtual = 0x4,
   All = 0xF
};

enum FindType : int
{
   UID = 0x1,
   Hash = 0x2
};

class CollectionItem
{
public:
   // Contains all the information required to create a copy item.
   class PseudoIdentifier
   {
   public:
      PseudoIdentifier();
      PseudoIdentifier( unsigned int aiCount, std::string aszName,
                        std::string aszDetails, std::string aszMeta );
      ~PseudoIdentifier();

      unsigned int Count;
      std::string Name;
      std::string DetailString;
      std::string MetaString;
      std::vector<Tag> Identifiers;
      std::vector<Tag> MetaTags;
   };

// Collection Item Interface
//
public:
   CollectionItem( const std::string& aszItemName, 
                   const std::vector<Tag>& alstCommon,
                   const std::vector<TraitItem>& alstRestrictions );
   ~CollectionItem();

   // Adds an item to the list of copies.
   std::shared_ptr<CopyItem> AddCopy( const Location& aAddrColID,
                                      const std::vector<Tag>& alstAttrTags,
                                      const std::vector<Tag>& alstMetaTags );

   bool RemoveCopy( const Location& aAddrColID,
                    const std::string aszUniqueID );

   std::string GenerateHash( const Identifier& aAddrIdentifier,
                             const std::vector<Tag>& alstAttrs,
                             const std::vector<Tag>& alstMetaTags ) const;

   // Deletes an item from the list of copies.
   void DeleteCopy( const std::string& aszUniqueHash );

   TryGetCopy<std::shared_ptr<CopyItem>> FindCopy( const std::string& aszUniqueHash, 
                                                   FindType aiType = UID ) const;
   std::vector<std::shared_ptr<CopyItem>> FindCopies( const Identifier& aCollection,
                                                      CollectionItemType aSearchType ) const;
   std::vector<std::shared_ptr<CopyItem>> FindCopies( const Location& aCollection,
                                                      CollectionItemType aSearchType ) const;

   std::string GetName() const;
   std::string GetProtoType() const;

private:
   std::string m_szName;
   std::vector<Tag> m_lstCommonTraits;
   std::vector<TraitItem> m_lstIdentifyingTraits;

   std::vector<std::shared_ptr<CopyItem>> m_lstCopies;

// Copy Item Interface
//
public:
   // Attempts to set an identifying trait on a copy item.
   // Returns true if the value is valid.
   bool SetIdentifyingTrait( CopyItem* aptItem,
                             const std::string& aszTraitKey,
                             const std::string& aszTraitValue ) const;
   void SetIdentifyingTraitDefaults( CopyItem* aptItem ) const;

   void DeleteCopy( CopyItem* aptItem );
   std::string CopyToString( CopyItem const* aptItem, 
                             const MetaTagType& aAccessType = MetaTagType::Public,
                             const Identifier& aAddrCompareID  = Location() ) const;
   

private:
   CopyItem* createCopy( const Identifier& aAddrColID,
                         const std::vector<Tag>& alstAttrs    = std::vector<Tag>(),
                         const std::vector<Tag>& alstMetaTags = std::vector<Tag>() ) const;

   void setCopyPairAttrs( CopyItem* aptItem, const std::string& aszKey, int iVal ) const;

// Static interface
//
public:
   static bool ParseCardLine( const std::string& aszLine, PseudoIdentifier& rPIdentifier );

   static bool ParseTagString( const std::string& aszDetails, std::vector<Tag>& rlstTags );

   static std::string ToCardLine( const Identifier& aAddrParentID, 
                                  const std::string& aszName,
                                  const std::vector<Tag>& alstAttrs    = std::vector<Tag>(),
                                  const std::vector<Tag>& alstMetaTags = std::vector<Tag>(), 
                                  const Identifier& aAddrCompareID        = Location() );

private:
   static bool isNameChar( const char& c );
};

