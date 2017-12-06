#include "CollectionItem.h"
#include "Addresser.h"
#include "StringInterface.h"

using namespace std;

CollectionItem::
PseudoIdentifier::PseudoIdentifier()
{
}

CollectionItem::
PseudoIdentifier::PseudoIdentifier(unsigned int aiCount,
                                   string aszName, 
                                   string aszDetails, 
                                   string aszMeta)
{
   Count = aiCount;
   Name = aszName;
   DetailString = aszDetails;
   MetaString = aszMeta;

   CollectionItem::ParseTagString(aszDetails, Identifiers);
   CollectionItem::ParseTagString(aszMeta, MetaTags);
}

CollectionItem::
PseudoIdentifier::~PseudoIdentifier()
{
}

CollectionItem::CollectionItem( const string& aszItemName, 
                                const vector<Tag>& alstCommon,
                                const vector<TraitItem>& alstRestrictions )
{
   m_szName               = aszItemName;
   m_lstCommonTraits      = alstCommon;
   m_lstIdentifyingTraits = alstRestrictions;
}


CollectionItem::~CollectionItem()
{
   m_lstCopies.clear();
}

shared_ptr<CopyItem>
CollectionItem::AddCopy( const Location& aAddrColID,
                         const vector<Tag>& alstAttrTags,
                         const vector<Tag>& alstMetaTags )
{
   CopyItem* newCopy = createCopy(aAddrColID, alstAttrTags, alstMetaTags);
   m_lstCopies.push_back(shared_ptr<CopyItem>(newCopy));
   return m_lstCopies.back();
}

bool 
CollectionItem::RemoveCopy( const Location& aAddrColID,
                            const string aszUniqueID )
{
   shared_ptr<CopyItem> ptCopy;
   auto nCopy = FindCopy( aszUniqueID );

   if( nCopy.Good() )
   {
      ptCopy = *nCopy.Value();
      int iRefCnt = ptCopy->RemoveResident( (const Location)aAddrColID );

      if( iRefCnt == 0 )
      {
         DeleteCopy( ptCopy.get() );
      }
      return true;
   }
   return false;
}

string 
CollectionItem::GenerateHash( const Identifier& aAddrIdentifier,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags ) const
{
   CopyItem* hashCopy = createCopy( aAddrIdentifier, alstAttrs, alstMetaTags );
   string szHash = hashCopy->GetHash();
   delete hashCopy;

   return szHash;
}

void 
CollectionItem::DeleteCopy( const string& aszUniqueHash )
{
   auto copy = FindCopy( aszUniqueHash );
   if( copy.Good() )
   {
      DeleteCopy( copy.Value()->get() );
   }
}

void 
CollectionItem::DeleteCopy(CopyItem* ociRemove)
{
   const static function<CopyItem* (const shared_ptr<CopyItem>&)> fnExtractor =
      [](const shared_ptr<CopyItem>& aptr)->CopyItem* { return aptr.get(); };

   int iFound = ListHelper::List_Find(ociRemove, m_lstCopies, fnExtractor);
   if (iFound > -1)
   {
      m_lstCopies.erase(m_lstCopies.begin() + iFound);
   }
}

string 
CollectionItem::CopyToString( CopyItem const* aptItem,
                              const MetaTagType& aAccessType,
                              const Identifier& aAddrCompareID ) const
{
   return ToCardLine( aptItem->GetAddress(), GetName(),
                      aptItem->GetIdentifyingAttributes(),
                      aptItem->GetMetaTags(aAccessType),
                      aAddrCompareID );
}

TryGetCopy<shared_ptr<CopyItem>>
CollectionItem::FindCopy( const string& aszUID, 
                          FindType aiType  ) const
{
   TryGetCopy<shared_ptr<CopyItem>> oRetval;
   bool match = false;
   for( auto ptCopy : m_lstCopies )
   {
      match |= (aiType & UID)  > 0 && ptCopy->GetUID()  == aszUID;
      match |= (aiType & Hash) > 0 && ptCopy->GetHash() == aszUID;
      if( match )
      {
         oRetval.Set( shared_ptr<CopyItem>(ptCopy) );
         break;
      }
   }

   return oRetval;
}

// Returns each copy
vector<shared_ptr<CopyItem>> 
CollectionItem::FindCopies( const Identifier& aCollection,
                            CollectionItemType aSearchType ) const
{
   vector<shared_ptr<CopyItem>> copies;
   for( auto iSub : aCollection.GetAddresses() )
   {
      Location location = Location(aCollection.GetMain(), iSub);

      auto newCopies = FindCopies(location, aSearchType);
      for( auto copy : newCopies )
      {
         bool bAlreadyHave = false;
         for( auto alreadyCopy : copies )
         {
            bAlreadyHave |= copy.get() == alreadyCopy.get();
         }

         if( !bAlreadyHave )
         {
            copies.push_back(copy);
         }
      }
   }

   return copies;
}

vector<shared_ptr<CopyItem>> 
CollectionItem::FindCopies( const Location& aCollection,
                            CollectionItemType aSearchType ) const
{
   vector<shared_ptr<CopyItem>> lstRetVal;
   for( auto copy : m_lstCopies )
   {
      if( ( aSearchType & Local )         &&
          ( copy->IsParent(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
      else if( ( aSearchType & Borrowed )        &&
               ( !copy->IsParent(aCollection) )  && 
               ( copy->IsResidentIn(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
      else if( ( aSearchType & Virtual )         &&
               ( copy->IsVirtual() )             &&
               ( copy->IsResidentIn(aCollection) ) )
      {
         lstRetVal.push_back(copy);
      }
   }

   return lstRetVal;
}


string 
CollectionItem::GetName() const
{
   return m_szName;
}

string 
CollectionItem::GetProtoType() const
{
   StringInterface strIface;
   string szResult;
   
   // Start with static common traits
   vector<Tag> lstAllCommonTraits(m_lstCommonTraits);

   // Include identifying traits
   // These appear as <Key>, *Val1::Val2...
   for each (TraitItem trait in m_lstIdentifyingTraits)
   {
      auto vals = trait.GetAllowedValues();
      strIface.ListToDelimStr(vals.cbegin(), vals.cend(), szResult);
      
      lstAllCommonTraits.push_back(make_pair(trait.GetKeyName(), szResult));
   }

   return CollectionItem::ToCardLine(Address(), "", lstAllCommonTraits);
}

vector<TraitItem> 
CollectionItem::GetIdentifyingTraits()
{
   return m_lstIdentifyingTraits;
}

// Returns the first trait key that has the input value
bool 
CollectionItem::MatchIdentifyingTrait( const std::string& aszValue, 
                                       std::string& rszKey )
{
   for( auto trait : m_lstIdentifyingTraits )
   {
      auto values = trait.GetAllowedValues();
      int iFoundVal = ListHelper::List_Find(aszValue, values);
      if( iFoundVal != -1 )
      {
         rszKey = trait.GetKeyName();
         return true;
      }
   }

   return false;
}

bool 
CollectionItem::SetIdentifyingTrait( CopyItem* aptItem,
                                     const string& aszTraitKey,
                                     const string& aszTraitValue,
                                     bool bSession ) const
{
   const static function<string(const TraitItem& )> fnTraitExtractor =
      [](const TraitItem& item )->string { return item.GetKeyName(); };

   int iFound;
   iFound = ListHelper::List_Find( aszTraitKey, m_lstIdentifyingTraits,
                                   fnTraitExtractor );
   if( iFound == -1 ) { return false; }

   TraitItem trait = m_lstIdentifyingTraits[iFound];
   iFound = ListHelper::List_Find( aszTraitValue, trait.GetAllowedValues() );
   if( iFound == -1 ) { return false; }

   // Set the trait
   aptItem->SetIdentifyingAttribute( aszTraitKey, aszTraitValue, bSession );
   setCopyPairAttrs( aptItem, aszTraitKey, iFound );
}

// Sets all the ident traits to their defaults.
void 
CollectionItem::SetIdentifyingTraitDefaults( CopyItem* aptItem ) const
{
   // Include default values for IDAttrs NOT specified.
   for( auto IDAttrs : m_lstIdentifyingTraits )
   {
      SetIdentifyingTrait( aptItem, IDAttrs.GetKeyName(), IDAttrs.GetDefaultValue(), false );
   }
}

CopyItem* 
CollectionItem::createCopy( const Identifier& aAddrColID,
                            const vector<Tag>& alstAttrs,
                            const vector<Tag>& alstMetaTags ) const
{
   return CopyItem::CreateCopyItem( this, aAddrColID, alstAttrs, alstMetaTags );
}

// Does not update session.
void 
CollectionItem::setCopyPairAttrs( CopyItem* aptItem, const string& aszKey, int iVal ) const
{
   const static function<string(const TraitItem&)> fnExtractor = 
      [](const TraitItem& aTI)->string { return aTI.GetKeyName(); };

   vector<string> lstPartners;

   // Find any traits paied with the key.
   vector<Tag> lstPairs = Config::Instance()->GetPairedKeysList();
   for each (Tag var in lstPairs)
   {
      if( ( var.first == aszKey ) &&
          ( ListHelper::List_Find(var.second, lstPartners) == -1 ) )
      {
         lstPartners.push_back(var.second);
      }
      else if( ( var.second == aszKey ) &&
               ( ListHelper::List_Find(var.first, lstPartners) == -1 ) )
      {
         lstPartners.push_back(var.first);
      }
   }

   // Search for the trait and asign it.
   for each (string szKey in lstPartners)
   {
      // Verify the trait is an identifying trait.
      int iIsAttr = ListHelper::List_Find(szKey, m_lstIdentifyingTraits, fnExtractor);
      if (iIsAttr != -1)
      {
         TraitItem foundTrait = m_lstIdentifyingTraits.at(iIsAttr);
         aptItem->SetIdentifyingAttribute( szKey, foundTrait.GetAllowedValues().at( iVal ), false );
      }
   }
}

bool
CollectionItem::ParseCardLine( const string& aszLine,
                               PseudoIdentifier& rPIdentifier )
{
   StringInterface parser;
   string szName, szDetails, szMeta;
   unsigned int iCount;

   bool bGoodParse = parser.ParseCardLine( aszLine, iCount, szName,
                                           szDetails, szMeta );
   if( bGoodParse )
   {
      // Output the details
      rPIdentifier = PseudoIdentifier(iCount, szName, szDetails, szMeta);
   }
   else
   {
      return false;
   }

   return true;
}

bool CollectionItem::ParseTagString( const string& aszDetails, 
                                     vector<Tag>& rlstTags )
{
   StringInterface parser;
   return parser.ParseTagString(aszDetails, rlstTags);
}

// This function is agnostic of the card object. It assumes that the 
// parent is the input parent ID. If the parent is also specified in the 
// metatags, they should be the same.
string 
CollectionItem::ToCardLine( const Identifier& aAddrParentID,
                            const string& aszName,
                            const vector<Tag>& alstAttrs,   
                            const vector<Tag>& alstMetaTags,
                            const Identifier& aAddrCompareID )
{
   StringInterface parser;
   Config* config = Config::Instance();
   string szAddressKey = CopyItem::GetAddressKey();
   vector<Tag> lstMetaTags(alstMetaTags);
   
   // Don't specify the parent if the parent is requesting the
   // the string.
   bool bExcludeParent = true;

   // If the parent is empty and the compare is default(empty) then the
   // parent will not be included.
   bExcludeParent = aAddrCompareID.IsEmpty() || aAddrCompareID == aAddrParentID;

   // Check if the parent is specified in the metatags.
   int iFound = ListHelper::List_Find( szAddressKey,
                                       lstMetaTags,
                                       config->GetTagHelper() );

   // Remove the parent from meta tags if there is one.
   // Otherwise, add the parent.
   if( bExcludeParent )
   {
      if( iFound != -1 )
      {
         lstMetaTags.erase(lstMetaTags.begin() + iFound);
      }
   }
   else if( iFound == -1 )
   {
      // Add the parent if it wasn't already found in the metatags.
      auto pairParent = make_pair(szAddressKey, aAddrParentID.GetFullAddress());
      lstMetaTags.push_back(pairParent);
   }

   return parser.ToCardLine(aszName, alstAttrs, lstMetaTags);
}

