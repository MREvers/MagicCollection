#include "CollectionItem.h"
#include "Addresser.h"

using namespace std;

CollectionItem::PseudoIdentifier::PseudoIdentifier()
{
}

CollectionItem::PseudoIdentifier::PseudoIdentifier(unsigned int aiCount,
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

CollectionItem::PseudoIdentifier::~PseudoIdentifier()
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
CollectionItem::AddCopy( const Address& aAddrColID,
                             const vector<Tag>& alstAttrTags,
                             const vector<Tag>& alstMetaTags )
{
   CopyItem* newCopy = createCopy(aAddrColID, alstAttrTags, alstMetaTags);
   m_lstCopies.push_back(shared_ptr<CopyItem>(newCopy));
   return m_lstCopies.back();
}

string 
CollectionItem::GenerateHash( const Address& aAddrIdentifier,
                              const vector<Tag>& alstAttrs,
                              const vector<Tag>& alstMetaTags )
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

std::string 
CollectionItem::CopyToString( CopyItem const* aptItem,
                              const MetaTagType& aAccessType,
                              const Address& aAddrCompareID ) const
{
   return ToCardLine( aptItem->GetAddress(), GetName(),
                      aptItem->GetIdentifyingAttributes(),
                      aptItem->GetMetaTags(aAccessType),
                      aAddrCompareID );
}

TryGet<shared_ptr<CopyItem>>
CollectionItem::FindCopy( const string& aszUID ) const
{
   TryGet<shared_ptr<CopyItem>> oRetval;

   for( auto ptCopy : m_lstCopies )
   {
      if( ptCopy->GetUID() == aszUID )
      {
         oRetval.Set( &ptCopy );
         break;
      }
   }

   return oRetval;
}

std::vector<std::shared_ptr<CopyItem>> 
CollectionItem::FindCopies( const Address& aCollection,
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
               ( copy->GetParent() == "" )       &&
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
   // Start with static common traits
   vector<Tag> lstAllCommonTraits(m_lstCommonTraits);

   // Include identifying traits
   // These appear as <Key>, *Val1::Val2...
   for each (TraitItem trait in m_lstIdentifyingTraits)
   {
      string szTraitVal = "";
      bool first = true;
      for each (string possibleTrait in trait.GetAllowedValues())
      {
         if (!first) { szTraitVal += "::"; }
         else { szTraitVal += "*"; } // This * will indicate it is an identifier.
         szTraitVal += possibleTrait;
         first = false;
      }
      lstAllCommonTraits.push_back(make_pair(trait.GetKeyName(), szTraitVal));
   }

   return CollectionItem::ToCardLine(Address(), "", lstAllCommonTraits);
}

bool 
CollectionItem::SetIdentifyingTrait( CopyItem* aptItem,
                                     const string& aszTraitKey,
                                     const string& aszTraitValue ) const
{
   const static function<string(const TraitItem& )> fnTraitExtractor =
      [](const TraitItem& item )->string { return item.GetKeyName(); };

   int iFound;
   iFound = ListHelper::List_Find( aszTraitKey, m_lstIdentifyingTraits, fnTraitExtractor );
   if( iFound == -1 ) { return false; }

   TraitItem trait = m_lstIdentifyingTraits[iFound];
   iFound = ListHelper::List_Find( aszTraitValue, trait.GetAllowedValues() );
   if( iFound == -1 ) { return false; }

   // Set the trait
   aptItem->SetIdentifyingAttribute( aszTraitKey, aszTraitValue );
   setCopyPairAttrs( aptItem, aszTraitKey, iFound );
}

CopyItem* 
CollectionItem::createCopy( const Address& aAddrColID,
                            const vector<Tag>& alstAttrs,
                            const vector<Tag>& alstMetaTags ) const
{
   vector<Tag> lstAttrs = alstAttrs;
   auto fnExtractor = Config::Instance()->GetTagHelper( Value );

   // Include default values for IDAttrs NOT specified.
   for( auto IDAttrs : m_lstIdentifyingTraits )
   {
      string szDefault = IDAttrs.GetDefaultValue();
      if( ListHelper::List_Find( szDefault, alstAttrs, fnExtractor ) == -1 )
      {
         lstAttrs.push_back( make_pair(IDAttrs.GetKeyName(), szDefault) );
      }
   }

   return CopyItem::CreateCopyItem( this, aAddrColID, alstAttrs, alstMetaTags );
}

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
      if (var.first == aszKey &&
          ListHelper::List_Find(var.second, lstPartners) == -1)
      {
         lstPartners.push_back(var.second);
      }
      else if (var.second == aszKey &&
               ListHelper::List_Find(var.first, lstPartners) == -1)
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
         aptItem->SetIdentifyingAttribute( szKey, foundTrait.GetAllowedValues().at( iVal ) );
      }
   }
}
/*
vector<shared_ptr<CopyItem>> 
CollectionItem::GetCopiesForCollection(const Address& aAddrCollectionIdentifier,
                                       CollectionItemType aItemType)
{
   vector<shared_ptr<CopyItem>> lstRetVal;
   vector<shared_ptr<CopyItem>> ::iterator iter_Copies = m_lstCopies.begin();

   for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
   {
      if ((aItemType & Local) > 0 &&
          (*iter_Copies)->IsParent(aAddrCollectionIdentifier))
      {
         lstRetVal.push_back(*iter_Copies);
      }
      else if ((aItemType & Borrowed) > 0 &&
         !(*iter_Copies)->IsParent(aAddrCollectionIdentifier) &&
         (*iter_Copies)->IsResidentIn(aAddrCollectionIdentifier))
      {
         lstRetVal.push_back(*iter_Copies);
      }
      else if ((aItemType & Virtual) > 0 &&
         (*iter_Copies)->GetParent() == "" &&
         (*iter_Copies)->IsResidentIn(aAddrCollectionIdentifier))
      {
         lstRetVal.push_back(*iter_Copies);
      }
   }

   return lstRetVal;
}
*/
bool
CollectionItem::ParseCardLine(const string& aszLine, PseudoIdentifier& rPIdentifier)
{
   string szLine;
   unsigned int iCount;
   string szMeta;
   string szDetails;
   string szName;
   szLine = StringHelper::Str_Trim(aszLine, ' ');

   unsigned int i = 0;
   if (szLine.size() > 0 && szLine[0] == 'x') { i++; }

   string szNum = "";
   while (i < szLine.size() && szLine.at(i) < '9' && szLine.at(i) > '0')
   {
      szNum = szNum + szLine.at(i);
      i++;
   }

   if (i >= szLine.size())
   {
      return false;
   }

   if (szNum == "")
   {
      szNum = "1";
   }

   try
   {
      iCount = stoi(szNum);
   }
   catch (...)
   {
      return false;
   }

   if (szLine.at(i) == 'x')
   {
      i++;
   }

   if (i >= szLine.size())
   {
      return false;
   }

   szName = "";
   unsigned int iter_size = szLine.size();
   while (i < iter_size &&
      ((szLine.at(i) >= 'a' && szLine.at(i) <= 'z') ||
      (szLine.at(i) >= 'A' && szLine.at(i) <= 'Z') ||
         (szLine.at(i) == ',' || szLine.at(i) == ' ' || szLine.at(i) == '-')))
   {
      szName = szName + szLine.at(i);
      i++;
   }

   szName.erase(0, szName.find_first_not_of(' '));
   szName.erase(szName.find_last_not_of(' ') + 1);

   while (i < iter_size && szLine.at(i) == ' ')
   {
      i++;
   }

   bool hasDets = false;
   bool hasMeta = false;
   if (i < iter_size)
   {
      hasDets = szLine.at(i) == '{';
      hasMeta = szLine.at(i) == ':';
   }

   szDetails = "";
   if (i < iter_size && hasDets)
   {
      while (i < iter_size && szLine.at(i) != '}')
      {
         szDetails += szLine.at(i);
         i++;
      }
      if (i < iter_size)
      {
         szDetails += szLine.at(i);
         i++;
      }
   }

   if (!hasMeta && hasDets)
   {
      while (i < iter_size && szLine.at(i) != ':')
      {
         i++;
      }
      hasMeta = (i < iter_size) && (szLine.at(i) == ':');
   }

   szMeta = "";
   if (i < iter_size && hasMeta)
   {
      i++;
      while (i < iter_size)
      {
         szMeta += szLine.at(i);
         i++;
      }
   }

   // Output the details
   rPIdentifier = PseudoIdentifier(iCount, szName, szDetails, szMeta);
   return true;
}

bool CollectionItem::ParseTagString(const string& aszDetails, vector<Tag>& rlstTags)
{
   vector<Tag> lstKeyVals;
   vector<string> lstPairs;
   vector<string> lstVal;

   vector<string> lstDetails = StringHelper::Str_Split(aszDetails, " ");

   vector<string>::iterator iter_attrs;
   for (iter_attrs = lstDetails.begin(); 
        iter_attrs != lstDetails.end(); 
        ++iter_attrs)
   {
      lstPairs = StringHelper::Str_Split(*iter_attrs, "=");
      if (lstPairs.size() > 1)
      {
         lstVal = StringHelper::Str_Split(lstPairs[1], "\"");
         if (lstVal.size() == 3)
         {
            string szVal = lstVal[1];
            lstKeyVals.push_back(make_pair(lstPairs[0], szVal));
         }
      }
   }
   rlstTags = lstKeyVals;
   return true;
}

string 
CollectionItem::ToCardLine( const Address& aAddrParentID,
                                   const std::string& aszName,
                                   const std::vector<Tag>& alstAttrs,   
                                   const std::vector<Tag>& alstMetaTags,
                                   const Address& aAddrCompareID )
{
   string szLine = aszName;
   szLine += " { ";

   vector<Tag>::const_iterator iter_keyValPairs;
   if (alstAttrs.size() > 0)
   {
      iter_keyValPairs = alstAttrs.cbegin();
      for (; iter_keyValPairs != alstAttrs.cend(); ++iter_keyValPairs)
      {
         szLine += iter_keyValPairs->first;
         szLine += "=\"";
         szLine += iter_keyValPairs->second;
         szLine += "\" ";
      }

   }
   szLine += "}";

   if (alstMetaTags.size() == 0)
   {
      return szLine;
   }

   szLine += " : { ";

   iter_keyValPairs = alstMetaTags.begin();
   unsigned int iDummy;
   for (; iter_keyValPairs != alstMetaTags.end(); ++iter_keyValPairs)
   {
      if (!(aAddrCompareID.Main == "") &&
         (iter_keyValPairs->first == "Address") &&
         (Address(iter_keyValPairs->second) == aAddrCompareID))
      {
         continue;
      }
      szLine += iter_keyValPairs->first;
      szLine += "=\"";
      szLine += iter_keyValPairs->second;
      szLine += "\" ";
   }
   szLine += "}";

   return szLine;
}

