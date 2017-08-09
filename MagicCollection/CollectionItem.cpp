#include "CollectionItem.h"
#include "Addresser.h"

CollectionItem::PseudoIdentifier::PseudoIdentifier()
{
}

CollectionItem::PseudoIdentifier::PseudoIdentifier(unsigned int aiCount,
                                                   std::string aszName, 
                                                   std::string aszDetails, 
                                                   std::string aszMeta)
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

CollectionItem::CollectionItem(std::string aszName,
                               std::vector<Tag> alstCommon, 
                               std::vector<TraitItem> alstRestrictions)
{
   m_szName = aszName;
   m_lstCommonTraits = alstCommon;
   m_lstIdentifyingTraits = alstRestrictions;
}


CollectionItem::~CollectionItem()
{
   m_lstCopies.clear();
}

std::string CollectionItem::GetName()
{
   return m_szName;
}

CopyItem* 
CollectionItem::AddCopyItem( const Address& aAddrColID,
                             std::vector<Tag> alstAttrs,
                             std::vector<Tag> alstMetaTags )
{
   CopyItem* newCopy = GenerateCopy(aAddrColID, alstAttrs, alstMetaTags);
   m_lstCopies.push_back(std::shared_ptr<CopyItem>(newCopy));
   return newCopy;
}

CopyItem* 
CollectionItem::GenerateCopy( const Address& aAddrColID,
                              std::vector<Tag> alstAttrs,
                              std::vector<Tag> alstMetaTags )
{
   CopyItem* newCopy = new CopyItem(&m_lstIdentifyingTraits, aAddrColID,
                                    alstAttrs, alstMetaTags);

   return newCopy;
}

void 
CollectionItem::RemoveCopyItem( const Address& aAddrColID,
                                std::string aszHash )
{
   std::vector<std::shared_ptr<CopyItem>>::iterator iter_Copies;

   for ( iter_Copies = m_lstCopies.begin();
         iter_Copies != m_lstCopies.end(); 
         ++iter_Copies)
   {
      CopyItem* cItem = iter_Copies->get();
      if (cItem->GetMetaTag(Config::HashKey, Hidden) == aszHash)
      {
         if (cItem->IsParent(aAddrColID) && 
             cItem->GetResidentIn().size() <= 1)
         {
            m_lstCopies.erase(iter_Copies);
         }
         else
         {
            cItem->RemoveResident(aAddrColID);
         }

         break;
      }
   }

}

void 
CollectionItem::RemoveResidentFromItem( CopyItem* acItem, 
                                        const Address& aAddrColID )
{
   std::function<CopyItem* (std::shared_ptr<CopyItem>)> fnExtractor;
   fnExtractor = [](std::shared_ptr<CopyItem> aptr)->
                                    CopyItem* { return aptr.get(); };
   int iFound = ListHelper::List_Find(acItem, m_lstCopies, fnExtractor);
   if (-1 != iFound)
   {
      acItem->RemoveResident(aAddrColID);
      if (acItem->GetParent() == "" &&
          acItem->GetResidentIn().size() == 0)
      {
         m_lstCopies.erase(m_lstCopies.begin() + iFound);
      }
   }
}

std::shared_ptr<CopyItem> 
CollectionItem::FindCopyItem( std::string aszHash,
                              const Address& aAddrResidentIn )
{
   Addresser addr;
   std::vector<std::shared_ptr<CopyItem>>::iterator iter_Copies;

   for ( iter_Copies  = m_lstCopies.begin(); 
         iter_Copies != m_lstCopies.end(); 
         ++iter_Copies )
   {
      CopyItem* cItem = iter_Copies->get();
      if ( cItem->GetHash() == aszHash &&
           (aAddrResidentIn.Main == "" || cItem->IsResidentIn(aAddrResidentIn)) )
      {
         return *iter_Copies;
      }
   }

   return std::shared_ptr<CopyItem>(nullptr);
}

std::vector<std::shared_ptr<CopyItem>>
CollectionItem::FindAllCopyItems(std::string aszHash, const Address& aptAddress)
{
   std::vector<std::shared_ptr<CopyItem>> lstRetval;
   std::vector<std::shared_ptr<CopyItem>>::iterator iter_Copies;

   for (iter_Copies = m_lstCopies.begin();
        iter_Copies != m_lstCopies.end();
        ++iter_Copies)
   {
      CopyItem* cItem = iter_Copies->get();
      if (cItem->GetMetaTag(Config::HashKey, Hidden) == aszHash &&
         (aptAddress.Main == "" || cItem->IsResidentIn(aptAddress)))
      {
         lstRetval.push_back(*iter_Copies);
      }
   }

   return lstRetval;
}

void CollectionItem::Erase(CopyItem* ociRemove)
{
   std::function<CopyItem* (std::shared_ptr<CopyItem>)> fnExtractor;
   fnExtractor = [](std::shared_ptr<CopyItem> aptr)->
                                    CopyItem* { return aptr.get(); };

   int iFound = ListHelper::List_Find(ociRemove, m_lstCopies, fnExtractor);
   if (iFound > -1)
   {
      m_lstCopies.erase(m_lstCopies.begin() + iFound);
   }
}

std::vector<std::shared_ptr<CopyItem>> 
CollectionItem::GetCopiesForCollection(const Address& aAddrCollectionIdentifier,
                                       CollectionItemType aItemType)
{
   std::vector<std::shared_ptr<CopyItem>> lstRetVal;
   std::vector<std::shared_ptr<CopyItem>> ::iterator iter_Copies = m_lstCopies.begin();

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

std::string 
CollectionItem::GetHash(const Address& aAddrIdentifier,
                        std::vector<Tag> alstAttrs,
                        std::vector<Tag> alstMetaTags)
{
   CopyItem copyToHash(&m_lstIdentifyingTraits, aAddrIdentifier, 
                       alstAttrs, alstMetaTags);

   return copyToHash.GetHash();
}

std::string 
CollectionItem::GetCardString(CopyItem* aItem, MetaTagType aTagType,
                              const Address& aAddrAddress)
{
   return CollectionItem::ToCardLine(aItem->GetParent(), m_szName, 
                                     aItem->GetIdentifyingAttributes(), 
                                     aItem->GetMetaTags(aTagType), aAddrAddress);
}

std::string 
CollectionItem::GetProtoTypeString()
{
   // Include multi traits in this list
   std::vector<Tag> lstAllCommonTraits(m_lstCommonTraits);
   for each (TraitItem trait in m_lstIdentifyingTraits)
   {
      std::string szTraitVal = "";
      bool first = true;
      for each (std::string possibleTrait in trait.GetAllowedValues())
      {
         if (!first) { szTraitVal += "::"; }
         else { szTraitVal += "*"; } // This * will indicate it is an identifier.
         szTraitVal += possibleTrait;
         first = false;
      }
      lstAllCommonTraits.push_back(std::make_pair(trait.GetKeyName(), szTraitVal));
   }

   return CollectionItem::ToCardLine(Address(), "", lstAllCommonTraits);
}

std::vector<Tag> CollectionItem::GetCommonTraits()
{
   return m_lstCommonTraits;
}

bool
CollectionItem::ParseCardLine(std::string aszLine, PseudoIdentifier& rPIdentifier)
{
   unsigned int iCount;
   std::string szMeta;
   std::string szDetails;
   std::string szName;
   aszLine = StringHelper::Str_Trim(aszLine, ' ');

   unsigned int i = 0;
   if (aszLine.size() > 0 && aszLine[0] == 'x') { i++; }

   std::string szNum = "";
   while (i < aszLine.size() && aszLine.at(i) < '9' && aszLine.at(i) > '0')
   {
      szNum = szNum + aszLine.at(i);
      i++;
   }

   if (i >= aszLine.size())
   {
      return false;
   }

   if (szNum == "")
   {
      szNum = "1";
   }

   try
   {
      iCount = std::stoi(szNum);
   }
   catch (...)
   {
      return false;
   }

   if (aszLine.at(i) == 'x')
   {
      i++;
   }

   if (i >= aszLine.size())
   {
      return false;
   }

   szName = "";
   unsigned int iter_size = aszLine.size();
   while (i < iter_size &&
      ((aszLine.at(i) >= 'a' && aszLine.at(i) <= 'z') ||
      (aszLine.at(i) >= 'A' && aszLine.at(i) <= 'Z') ||
         (aszLine.at(i) == ',' || aszLine.at(i) == ' ' || aszLine.at(i) == '-')))
   {
      szName = szName + aszLine.at(i);
      i++;
   }

   szName.erase(0, szName.find_first_not_of(' '));
   szName.erase(szName.find_last_not_of(' ') + 1);

   while (i < iter_size && aszLine.at(i) == ' ')
   {
      i++;
   }

   bool hasDets = false;
   bool hasMeta = false;
   if (i < iter_size)
   {
      hasDets = aszLine.at(i) == '{';
      hasMeta = aszLine.at(i) == ':';
   }

   szDetails = "";
   if (i < iter_size && hasDets)
   {
      while (i < iter_size && aszLine.at(i) != '}')
      {
         szDetails += aszLine.at(i);
         i++;
      }
      if (i < iter_size)
      {
         szDetails += aszLine.at(i);
         i++;
      }
   }

   if (!hasMeta && hasDets)
   {
      while (i < iter_size && aszLine.at(i) != ':')
      {
         i++;
      }
      hasMeta = (i < iter_size) && (aszLine.at(i) == ':');
   }

   szMeta = "";
   if (i < iter_size && hasMeta)
   {
      i++;
      while (i < iter_size)
      {
         szMeta += aszLine.at(i);
         i++;
      }
   }

   // Output the details
   rPIdentifier = PseudoIdentifier(iCount, szName, szDetails, szMeta);
   return true;
}

bool CollectionItem::ParseTagString(std::string aszDetails, std::vector<Tag>& rlstTags)
{
   std::vector<Tag> lstKeyVals;
   std::vector<std::string> lstPairs;
   std::vector<std::string> lstVal;

   std::vector<std::string> lstDetails = StringHelper::Str_Split(aszDetails, " ");

   std::vector<std::string>::iterator iter_attrs;
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
            std::string szVal = lstVal[1];
            lstKeyVals.push_back(std::make_pair(lstPairs[0], szVal));
         }
      }
   }
   rlstTags = lstKeyVals;
   return true;
}

std::string CollectionItem::ToCardLine(const Address& aAddrParentID,
   std::string aszName,
   std::vector<Tag> alstAttrs,
   std::vector<Tag> alstMetaTags,
   const Address& aAddrCompareID)
{
   std::string szLine = aszName;
   szLine += " { ";

   std::vector<Tag>::iterator iter_keyValPairs;
   if (alstAttrs.size() > 0)
   {
      iter_keyValPairs = alstAttrs.begin();
      for (; iter_keyValPairs != alstAttrs.end(); ++iter_keyValPairs)
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