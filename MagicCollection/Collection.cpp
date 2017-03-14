#include "Collection.h"


Collection::Transaction::Transaction(Collection* aoCol)
{
   m_Col = aoCol;
   Recordable = true;

}

Collection::Transaction::~Transaction()
{
   Actions.clear();
}


Collection* Collection::Transaction::operator->()
{
   m_Col->TransactionIntercept = true;
   return m_Col;
}

void Collection::Transaction::AddAction(Action& aoAct)
{
   if (IsOpen)
   {
      Actions.push_back(aoAct);
   }

}

void Collection::Transaction::RemoveAction(int i)
{
   if (IsOpen)
   {
      Actions.erase(Actions.begin() + i);
   }
}

void Collection::Transaction::Finalize(bool abRecordable)
{
   if (IsOpen)
   {
      IsOpen = false;
      int iSize = Actions.size();
      for (int i = 0; i < iSize; i++)
      {
         Actions.at(i).Do();
      }
      Recordable = abRecordable;
   }

}

void Collection::Transaction::Rollback()
{
   if (!IsOpen)
   {
      int iSize = Actions.size();
      for (int i = 0; i < iSize; i++)
      {
         Actions.at(i).Undo();
      }
   }
}



Collection::Collection(std::string aszName, CollectionSource* aoSource, std::vector<std::string>* alstLoadedCollections)
{
   m_ColSource = aoSource;
   m_szName = aszName;
   TransactionIntercept = false;
   m_szHistoryFileName = m_szName + ".history";
   m_szMetaTagFileName = m_szName + ".metatags";
   m_lstLoadedCollectionsBuffer = alstLoadedCollections;
}

Collection::~Collection()
{
}

std::string Collection::GetName()
{
   return m_szName;
}

void Collection::AddItem(std::string aszNewItem,
   bool bFinal,
   std::vector<std::pair<std::string, std::string>> alstAttrs,
   std::vector<std::pair<std::string, std::string>> alstMeta)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {
      // Construct a string for history.
      std::string szCard = "";
      int iCardProto = m_ColSource->LoadCard(aszNewItem);
      if (iCardProto != -1)
      {
         CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
         CopyObject oCO = oCardPrototype->GenerateCopy(m_szName, alstAttrs);
         szCard += cardToString(iCardProto, &std::make_pair(&oCO, 1));

         // Store the action and how to undo it here.
         Collection::Action oAction;
         oAction.Identifier = "+ " + szCard;
         oAction.Do = std::bind(&Collection::addItem, this, aszNewItem, alstAttrs, alstMeta);
         oAction.Undo = std::bind(&Collection::removeItem, this, aszNewItem, alstAttrs, alstMeta);

         // Store the arguments
         Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
         oTrans->AddAction(oAction);

         TransactionIntercept = false;
      }
      return;
   }

   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->AddItem(aszNewItem, true, alstAttrs, alstMeta);

   if (bFinal)
   {
      oTrans->Finalize();
   }
}

void Collection::AddItem(std::string aszNewItemLongName,
   std::vector<std::pair<std::string, std::string>> alstMetaTags)
{
   std::string szName;
   int iCount;
   std::string szDetails;
   if (Collection::ParseCardLine(aszNewItemLongName, iCount, szName, szDetails))
   {
      auto lstAttrs = Collection::ParseAttrs(szDetails);
      AddItem(szName, true, lstAttrs, alstMetaTags);
   }
}

void Collection::RemoveItem(std::string aszRemoveItem,
   bool bFinal,
   std::vector<std::pair<std::string, std::string>> alstAttrs,
   std::vector<std::pair<std::string, std::string>> alstMeta)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {
      int iCardProto = m_ColSource->LoadCard(aszRemoveItem);
      if (iCardProto != -1)
      {
         CollectionObject* CardPrototype = m_ColSource->GetCardPrototype(iCardProto);
         CopyObject* oCO;
         if (CardPrototype->GetCopy(m_szName, alstAttrs, oCO))
         {
            std::string szCard = cardToString(iCardProto, &std::make_pair(oCO, 1));

            Collection::Action oAction;
            oAction.Identifier = "- " + szCard;
            oAction.Do = std::bind(&Collection::removeItem, this, aszRemoveItem, alstAttrs, alstMeta);
            oAction.Undo = std::bind(&Collection::addItem, this, aszRemoveItem, alstAttrs, alstMeta);

            // Store the arguments
            Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
            oTrans->AddAction(oAction);

            TransactionIntercept = false;
         }
      }
      return;
   }

   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->RemoveItem(aszRemoveItem, true, alstAttrs);

   if (bFinal)
   {
      oTrans->Finalize();
   }
}

void
Collection::RemoveItem(std::string aszNewItemLongName,
   std::vector<std::pair<std::string, std::string>> alstMetaTags)
{
   std::string szName;
   int iCount;
   std::string szDetails;
   if (Collection::ParseCardLine(aszNewItemLongName, iCount, szName, szDetails))
   {
      auto lstAttrs = Collection::ParseAttrs(szDetails);
      RemoveItem(szName, true, lstAttrs, alstMetaTags);
   }
}

void
Collection::RemoveMetaTag(
   std::string aszLongName,
   std::string aszKey,
   std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
   if (TransactionIntercept)
   {
      std::string szCardName;
      int iAmount;
      std::string szDetails;
      if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
      {
         std::vector<std::pair<std::string, std::string>> lstAttrs = ParseAttrs(szDetails);
         int iCardProto = m_ColSource->LoadCard(szCardName);
         if (iCardProto != -1)
         {
            CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
            CopyObject* oCO;
            if (oCardPrototype->GetCopy(m_szName, lstAttrs, alstMatchMeta, oCO))
            {
               std::string szOldVal = oCO->GetMetaTag(m_szName, aszKey).second;
               if (szOldVal != "!NULL") // Key Doesnt exist
               {
                  // Store the action and how to undo it here.
                  Collection::Action oAction;
                  oAction.Identifier = "Removed Meta-Tag '" + aszKey + "' from " + aszLongName;// + szCard;
                  oAction.Do = std::bind(&Collection::removeMetaTag,
                     this, oCO, aszKey);

                  oAction.Undo = std::bind(&Collection::setMetaTag,
                     this, oCO, aszKey, szOldVal);

                  // Store the arguments
                  Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
                  oTrans->AddAction(oAction);
               }
            }


         }
      }
      TransactionIntercept = false;
      return;
   }


   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->RemoveMetaTag(aszLongName, aszKey, alstMatchMeta);
   oTrans->Finalize();
}

void Collection::SetMetaTag(
   std::string aszLongName,
   std::string aszKey,
   std::string aszValue,
   std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
   if (TransactionIntercept)
   {
      std::string szCardName;
      int iAmount;
      std::string szDetails;
      if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
      {
         std::vector<std::pair<std::string, std::string>> lstAttrs = ParseAttrs(szDetails);
         int iCardProto = m_ColSource->LoadCard(szCardName);
         if (iCardProto != -1)
         {
            CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
            CopyObject* oCO;
            if (oCardPrototype->GetCopy(m_szName, lstAttrs, alstMatchMeta, oCO))
            {
               // Store the action and how to undo it here.
               Collection::Action oAction;
               oAction.Identifier = "Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
               oAction.Do = std::bind(&Collection::setMetaTag, this, oCO, aszKey, aszValue);

               std::string szOldVal = oCO->GetMetaTag(m_szName, aszKey).second;
               if (szOldVal != "!NULL") // Key Doesnt exist
               {
                  oAction.Undo = std::bind(&Collection::setMetaTag,
                     this, oCO, aszKey, szOldVal);
               }
               else
               {
                  oAction.Undo = std::bind(&Collection::removeMetaTag,
                     this, oCO, aszKey);
               }

               // Store the arguments
               Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
               oTrans->AddAction(oAction);
            }


         }
      }
      TransactionIntercept = false;
      return;
   }


   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->SetMetaTag(aszLongName, aszKey, aszValue, alstMatchMeta);
   oTrans->Finalize();
}

void Collection::SetMetaTags(std::string aszLongName,
   std::vector<std::pair<std::string, std::string>> alstKeyVals,
   std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
   if (TransactionIntercept)
   {
      std::string szCardName;
      int iAmount;
      std::string szDetails;
      if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
      {
         std::vector<std::pair<std::string, std::string>> lstAttrs = ParseAttrs(szDetails);
         int iCardProto = m_ColSource->LoadCard(szCardName);
         if (iCardProto != -1)
         {
            CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
            CopyObject* oCO;
            if (oCardPrototype->GetCopy(m_szName, lstAttrs, alstMatchMeta, oCO))
            {
               // Store the action and how to undo it here.
               Collection::Action oAction;
               oAction.Identifier = "Set Meta-Tag '";// + szCard;
               oAction.Do = std::bind(&Collection::setMetaTags, this, oCO, alstKeyVals);

               std::vector<std::pair<std::string, std::string>> lstUndoList;
               std::vector<std::pair<std::string, std::string>> lstOldMetaTags = oCO->GetMetaTags(m_szName);
               std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags = lstOldMetaTags.begin();
               for (; iter_MetaTags != lstOldMetaTags.end(); ++iter_MetaTags)
               {
                  lstUndoList.push_back(std::make_pair(iter_MetaTags->first, iter_MetaTags->second));
               }
               oAction.Undo = std::bind(&Collection::setMetaTags,
                  this, oCO, lstUndoList);

               // Store the arguments
               Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
               oTrans->AddAction(oAction);
            }


         }
      }
      TransactionIntercept = false;
      return;
   }


   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->SetMetaTags(aszLongName, alstKeyVals, alstMatchMeta);
   oTrans->Finalize();
}

void Collection::SetNonUniqueAttribute(
   std::string aszLongName,
   std::string aszKey,
   std::string aszValue,
   std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
   if (TransactionIntercept)
   {
      std::string szCardName;
      int iAmount;
      std::string szDetails;
      if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
      {
         std::vector<std::pair<std::string, std::string>> lstAttrs = ParseAttrs(szDetails);
         int iCardProto = m_ColSource->LoadCard(szCardName);
         if (iCardProto != -1)
         {
            CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
            CopyObject* oCO;
            if (oCardPrototype->GetCopy(m_szName, lstAttrs, alstMatchMeta, oCO))
            {

               auto iter_UniqueTrait = oCO->NonUniqueTraits.find(aszKey);
               if (iter_UniqueTrait != oCO->NonUniqueTraits.end())
               {
                  // Store the action and how to undo it here.
                  Collection::Action oAction;
                  oAction.Identifier = "Setting attribute '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
                  oAction.Do = std::bind(&Collection::setItemAttr, this, oCO, aszKey, aszValue);

                  std::string szOldVal = iter_UniqueTrait->second;

                  oAction.Undo = std::bind(&Collection::setItemAttr, this, oCO, aszKey, szOldVal);

                  // Store the arguments
                  Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
                  oTrans->AddAction(oAction);
               }

            }


         }
      }
      TransactionIntercept = false;
      return;
   }


   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->SetMetaTag(aszLongName, aszKey, aszValue, alstMatchMeta);

   oTrans->Finalize();
}

void Collection::SetFeatures(
   std::string aszLongName,
   std::vector<std::pair<std::string, std::string>> alstNewMeta,
   std::vector<std::pair<std::string, std::string>> alstNewAttrs,
   std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
   if (TransactionIntercept)
   {
      std::string szCardName;
      int iAmount;
      std::string szDetails;
      if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
      {
         std::vector<std::pair<std::string, std::string>> lstAttrs = ParseAttrs(szDetails);
         int iCardProto = m_ColSource->LoadCard(szCardName);
         if (iCardProto != -1)
         {
            CollectionObject* oCardPrototype = m_ColSource->GetCardPrototype(iCardProto);
            CopyObject* oCO;
            if (oCardPrototype->GetCopy(m_szName, lstAttrs, alstMatchMeta, oCO))
            {
               // Store the action and how to undo it here.
               Collection::Action oAction;
               oAction.Identifier = "Set Features '";// + szCard;
               oAction.Do = std::bind(&Collection::setFeatures, this, oCO, alstNewMeta, alstNewAttrs);

               std::vector<std::pair<std::string, std::string>> lstUndoList;
               std::vector<std::pair<std::string, std::string>> lstOldMetaTags = oCO->GetMetaTags(m_szName);
               std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags = lstOldMetaTags.begin();
               for (; iter_MetaTags != lstOldMetaTags.end(); ++iter_MetaTags)
               {
                  lstUndoList.push_back(std::make_pair(iter_MetaTags->first, iter_MetaTags->second));
               }
               oAction.Undo = std::bind(&Collection::setFeatures, this, oCO, lstUndoList, lstAttrs);

               // Store the arguments
               Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
               oTrans->AddAction(oAction);
            }


         }
      }
      TransactionIntercept = false;
      return;
   }


   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->SetFeatures(aszLongName, alstNewMeta, alstNewAttrs, alstMatchMeta);

   oTrans->Finalize();
}

std::vector<std::string> Collection::GetNonUniqueAttributeRestrictions(std::string aszLongName, std::string aszKey)
{
   std::string szCardName;
   int iAmount;
   std::string szDetails;
   if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
   {
      bool bAlreadyTagged = false;
      int iCardCacheIndex = m_ColSource->LoadCard(szCardName);
      if (iCardCacheIndex != -1)
      {
         CollectionObject* oCardClass = m_ColSource->GetCardPrototype(iCardCacheIndex);
         auto mapRestrictions = oCardClass->GetNonUniqueAttributeRestrictions();
         if (mapRestrictions.find(aszKey) != mapRestrictions.end())
         {
            return mapRestrictions[aszKey];
         }
      }
   }
   std::vector<std::string> lstNullRetVal;
   lstNullRetVal.push_back("*");
   return lstNullRetVal;
}

std::vector<std::vector<std::pair<std::string, std::string>>> Collection::GetMetaTags(std::string aszLongName)
{
   std::vector<std::vector<std::pair<std::string, std::string>>> lstRetVal;
   std::string szCardName;
   int iAmount;
   std::string szDetails;
   if (Collection::ParseCardLine(aszLongName, iAmount, szCardName, szDetails))
   {
      int iCardCacheIndex = m_ColSource->LoadCard(szCardName);
      if (iCardCacheIndex != -1)
      {
         std::vector<CopyObject*> LstCopies = m_ColSource->GetCardPrototype(iCardCacheIndex)->GetCopies(m_szName);
         std::vector<CopyObject*>::iterator iter_Copies = LstCopies.begin();
         for (; iter_Copies != LstCopies.end(); ++iter_Copies)
         {
            if (CollectionObject::CompareKeyValPairList(
               CollectionObject::FilterOutUniqueTraits(ParseAttrs(szDetails)),
               CollectionObject::ConvertMapToList((*iter_Copies)->NonUniqueTraits)))
            {
               lstRetVal.push_back((*iter_Copies)->GetMetaTags(m_szName));
            }
         }
      }
   }

   return lstRetVal;
}


void Collection::setName(std::string aszName)
{
   m_szName = aszName;
   m_szHistoryFileName = aszName + ".history";
}

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::addItem(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs,
   std::vector<std::pair<std::string, std::string>> alstMeta)
{
   int iCard = m_ColSource->LoadCard(aszNewItem);
   if (iCard != -1)
   {
      registerItem(iCard);

      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card. Save the reference if we need to add unique traits.
      CopyObject* oCO = oCard->AddCopy(m_szName, alstAttrs);

      std::vector<std::pair<std::string, std::string>>::iterator iter_Meta = alstMeta.begin();
      for (; iter_Meta != alstMeta.end(); iter_Meta++)
      {
         oCO->SetMetaTag(m_szName, iter_Meta->first, iter_Meta->second);
      }
   }
}

CopyObject* Collection::forceAdd(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
   int iCard = m_ColSource->LoadCard(aszNewItem);
   if (iCard != -1)
   {
      registerItem(iCard);

      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card. Save the reference if we need to add unique traits.
      CopyObject* oCO = oCard->AddCopy(m_szName, alstAttrs);

      return oCO;
   }
}

void Collection::registerItem(int aiItem)
{
   // Make sure we don't already 'have' this card.
   bool bNewCard = true;
   for (int i = 0; i < m_lstCollection.size(); i++)
   {
      bNewCard &= m_lstCollection.at(i) != aiItem;
   }

   if (bNewCard)
   {
      m_lstCollection.push_back(aiItem);
   }
}

void Collection::removeItem(std::string aszRemoveItem,
   std::vector<std::pair<std::string, std::string>> alstAttrs,
   std::vector<std::pair<std::string, std::string>> alstMeta)
{
   int iCard = m_ColSource->LoadCard(aszRemoveItem);
   if (iCard != -1)
   {
      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card.
      oCard->RemoveCopy(m_szName, alstAttrs, alstMeta);
   }
}

std::string Collection::changeItemAttribute_string(
   std::string aszCardname,
   CopyObject* aoCO,
   std::string aszKey,
   std::string aszNewVal,
   bool bIsParentCol)
{
   int iCardProto = m_ColSource->LoadCard(aszCardname);

   std::string szCard = "";
   CopyObject oCO(*aoCO);
   szCard += cardToString(iCardProto, &std::make_pair(&oCO, 1), !bIsParentCol);

   std::string szAfter = "";
   setItemAttr(&oCO, aszKey, aszNewVal);
   szAfter = cardToString(iCardProto, &std::make_pair(&oCO, 1), !bIsParentCol);

   return "% " + szCard + " -> " + szAfter;
}

void Collection::loadCollectionFromFile(std::string aszFileName)
{
   // Load the collection
   std::ifstream in(aszFileName);
   std::stringstream buffer;
   buffer << in.rdbuf();
   std::string contents(buffer.str());

   std::vector<std::string> lstLines = splitIntoLines(contents);

   loadCollectionLines(lstLines);
}

void Collection::loadCollectionLines(std::vector<std::string>& alstLines)
{
   for (std::vector<std::string>::iterator iter_CardLine = alstLines.begin(); iter_CardLine != alstLines.end(); ++iter_CardLine)
   {
      this->loadCardLine(*iter_CardLine);
   }

   finalizeTransaction(false);
}

bool Collection::loadCardLine(std::string aszNewCardLine)
{
   int iNum;
   std::string szName;
   std::string szDetails;
   if (!ParseCardLine(aszNewCardLine, iNum, szName, szDetails))
   {
      m_lstUnreversibleChanges.push_back("Could Not Parse Line \"" + aszNewCardLine + "\"");
      return false;
   }
   std::vector<std::pair<std::string, std::string>> lstKeyVals = ParseAttrs(szDetails);

   // Add or find a copy for each count.
   for (int i = 0; i < iNum; i++)
   {
      bool bNeedNewCopy = true;
      int iCardIndex = m_ColSource->LoadCard(szName);
      if (iCardIndex != -1)
      {
         // Create a temporary copy that will be used in comparisons
         CollectionObject* ColPrototype = m_ColSource->GetCardPrototype(iCardIndex);

         // Keep in mind, that lstKeyVals may overwrite m_szName as the parent collection.
         CopyObject oTargetCopy = ColPrototype->GenerateCopy(m_szName, lstKeyVals);

         // Look for a copy that is identical to this copy and check whether it is
         // currently uncounted/unincluded in this collection. If a copy is found,
         // add this collection to its resident collection.

         // Check for all existing copies in the collection that the target copy claims it is in.
         bool bNeedNewCopy = true;
         std::vector<CopyObject*> lstExistingCopies = m_ColSource
            ->GetCardPrototype(iCardIndex)
            ->GetLocalCopies(oTargetCopy.ParentCollection);
         std::vector<CopyObject*>::iterator iter_ExistingCopy = lstExistingCopies.begin();
         for (; iter_ExistingCopy != lstExistingCopies.end(); ++iter_ExistingCopy)
         {
            // The only way a copy could possibly have this collection as a resi. is if this collection created it
            //  or used it. If that is the case, then we should not use it as the copy.
            if (SourceObject::List_Find(m_szName, (*iter_ExistingCopy)->ResidentCollections) == -1)
            {
               // This copy is not used by this collection. Check if this copy is identical to the target copy.
               if (CollectionObject::IsSameIdentity(&oTargetCopy, *iter_ExistingCopy))
               {
                  // Record that this item is in this collection.
                  // Normally this is done during the "AddItem" call, but
                  // if the copy was loaded by another collection, it must
                  // be done here.
                  registerItem(iCardIndex);

                  // Record this collection in this copy's residents.
                  (*iter_ExistingCopy)->ResidentCollections.push_back(m_szName);
                  bNeedNewCopy = false;
                  break;
               }
            }
         }

         // If we didn't find a copy that matches, generate a new one.
         if (bNeedNewCopy)
         {
            // Just add the copy if it is in this collection or
            // the collection is loaded.
            if (oTargetCopy.ParentCollection == m_szName ||
               (SourceObject::List_Find(oTargetCopy.ParentCollection, *m_lstLoadedCollectionsBuffer) == -1))
            {
               AddItem(szName, false, lstKeyVals);
            }
            else
            {
               // If the parent collection is loaded, and a copy was not found in that collection,
               // add a modified copy where the parent is ""
               std::vector<std::pair<std::string, std::string>> lstKeyValsFixed(lstKeyVals);
               std::vector<std::pair<std::string, std::string>>::iterator iter_keyvals = lstKeyValsFixed.begin();
               for (; iter_keyvals != lstKeyValsFixed.end(); ++iter_keyvals)
               {
                  if (iter_keyvals->first == "Parent")
                  {
                     iter_keyvals->second = "";

                     // Record somewhere that this change was made.
                     m_lstUnreversibleChanges.push_back(changeItemAttribute_string(szName, &oTargetCopy, "Parent", ""));
                     break;
                  }
               }

               AddItem(szName, false, lstKeyValsFixed);
            }

         } // End If need new copy

      } // End card name exists
      else
      {
         // Can't find the card in the source... PANIC
         m_lstUnreversibleChanges.push_back("Could Not Find Card \"" + szName + "\"");
      }

   } // End for Card name in file

   return false;
}

void Collection::refreshCopyLinks(std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges)
{
   // Now that the collection is loaded, if there are cards with parent collection == this but
   // this is not in their residents, then that copy no longer exists.
   std::vector<std::pair<std::string, CopyObject*>> lstFullCol = m_ColSource->GetCollection(m_szName);
   std::vector<std::pair<std::string, CopyObject*>> ::iterator iter_ColOs = lstFullCol.begin();
   for (; iter_ColOs != lstFullCol.end(); ++iter_ColOs)
   {
      if (SourceObject::List_Find(m_szName, (*iter_ColOs).second->ResidentCollections) == -1)
      {
         std::vector<std::string> lstAffectedCols = (*iter_ColOs).second->ResidentCollections;
         std::vector<std::string>::iterator iter_affected_col = lstAffectedCols.begin();
         for (; iter_affected_col != lstAffectedCols.end(); ++iter_affected_col)
         {
            alstOutsideForcedChanges.push_back(
               std::make_pair(
                  *iter_affected_col,
                  changeItemAttribute_string((*iter_ColOs).first, (*iter_ColOs).second, "Parent", "", false)
               ));
         }
         (*iter_ColOs).second->ParentCollection = "";
      }
   }
}

void Collection::loadMetaTagsFromFile()
{
   // Now load meta tags
   std::ifstream in(m_szMetaTagFileName + ".txt");
   if (in.good())
   {
      std::stringstream buffer;
      buffer << in.rdbuf();
      std::string contents(buffer.str());

      std::vector<std::string> lstLines = splitIntoLines(contents);

      loadMetaTagLines(lstLines);
   }
}

void Collection::loadMetaTagLines(std::vector<std::string>& alstLines)
{
   for (std::vector<std::string>::iterator iter = alstLines.begin(); iter != alstLines.end(); ++iter)
   {
      std::vector<std::string> LstSplitLine = SourceObject::Str_Split(*iter, ":");
      if (LstSplitLine.size() == 2)
      {
         std::string szTempName;
         std::string szTempDetails;
         int iTaggedCards;
         if (ParseCardLine(LstSplitLine[0], iTaggedCards, szTempName, szTempDetails))
         {
            std::string szKey = "";
            std::string szVal = "";
            std::vector<std::pair<std::string, std::string>> LstSplitMeta = ParseAttrs(LstSplitLine[1]);
            if (LstSplitMeta.size() > 0)
            {
               // Store the tags so that
               // There are no MatchMeta because no cards have meta yet. I.e. NO META TAGS
               for (int k = 0; k < iTaggedCards; k++)
               {
                  std::vector<std::pair<std::string, std::string>>::iterator iter_NewMeta = LstSplitMeta.begin();
                  for (; iter_NewMeta != LstSplitMeta.end(); ++iter_NewMeta)
                  {
                     SetMetaTag(LstSplitLine[0], iter_NewMeta->first, iter_NewMeta->second);
                  }
               } // End for number of copies
            }
         } // End if successful parse

      }
   }
}

void Collection::setItemAttr(
   CopyObject* aoCO,
   std::string aszKey,
   std::string aszNewVal)
{
   aoCO->SetNonUniqueAttr(aszKey, aszNewVal);
}

void Collection::setItemAttrs(
   CopyObject* aoCO,
   std::vector<std::pair<std::string, std::string>> alstKeyVals)
{
   std::vector<std::pair<std::string, std::string>>::iterator iter_KeyVals = alstKeyVals.begin();
   for (; iter_KeyVals != alstKeyVals.end(); ++iter_KeyVals)
   {
      aoCO->SetNonUniqueAttr(iter_KeyVals->first, iter_KeyVals->second);
   }
}


void Collection::setMetaTag(
   CopyObject* aoCO,
   std::string aszKey,
   std::string aszNewTag)
{
   aoCO->SetMetaTag(m_szName, aszKey, aszNewTag);
}

void Collection::setMetaTags(
   CopyObject* aoCO,
   std::vector<std::pair<std::string, std::string>> alstKeyVals)
{
   std::vector<std::pair<std::string, std::string>>::iterator iter_KeyVals = alstKeyVals.begin();
   for (; iter_KeyVals != alstKeyVals.end(); ++iter_KeyVals)
   {
      aoCO->SetMetaTag(m_szName, iter_KeyVals->first, iter_KeyVals->second);
   }
}

void  Collection::removeMetaTag(
   CopyObject* aoCO,
   std::string aszKey)
{
   aoCO->RemoveMetaTag(m_szName, aszKey);
}

void Collection::setFeatures(CopyObject* aoCO,
   std::vector<std::pair<std::string, std::string>> alstNewMeta,
   std::vector<std::pair<std::string, std::string>> alstNewAttrs)
{
   setMetaTags(aoCO, alstNewMeta);
   setItemAttrs(aoCO, alstNewAttrs);
}

void Collection::LoadCollection(std::string aszFileName, std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges)
{
   this->loadCollectionFromFile(aszFileName);

   this->refreshCopyLinks(alstOutsideForcedChanges);

   this->loadMetaTagsFromFile();
}

void Collection::SaveCollection(std::string aszFileName)
{
   std::vector<std::string> lstLines = GetCollectionList();

   if (m_lstUnreversibleChanges.size() > 0 || m_lstTransactions.size() > 0)
   {
      std::string szTimeString = "";
      time_t now = time(0);
      struct tm timeinfo;
      localtime_s(&timeinfo, &now);
      char str[26];
      asctime_s(str, sizeof str, &timeinfo);
      str[strlen(str) - 1] = 0;


      std::vector<std::string> lstHistoryLines;

      std::vector<std::string>::iterator iter_addit = m_lstUnreversibleChanges.begin();
      for (; iter_addit != m_lstUnreversibleChanges.end(); ++iter_addit)
      {
         lstHistoryLines.push_back(*iter_addit);
      }

      std::vector<Transaction>::iterator iter_transactions = m_lstTransactions.begin();
      for (; iter_transactions != m_lstTransactions.end(); ++iter_transactions)
      {
         if (iter_transactions->Recordable)
         {
            std::vector<Action>::iterator iter_actions = iter_transactions->Actions.begin();
            for (; iter_actions != iter_transactions->Actions.end(); ++iter_actions)
            {
               lstHistoryLines.push_back(iter_actions->Identifier);
            }
            iter_transactions->Recordable = false;
         }
      }

      if (lstHistoryLines.size() > 0)
      {
         std::ofstream oHistFile;
         oHistFile.open(m_szHistoryFileName + ".txt", std::ios_base::app);

         oHistFile << "[" << str << "] " << std::endl;

         std::vector<std::string>::iterator iter_histLines = lstHistoryLines.begin();
         for (; iter_histLines != lstHistoryLines.end(); ++iter_histLines)
         {
            oHistFile << *iter_histLines << std::endl;
         }

         oHistFile.close();
         m_lstUnreversibleChanges.clear();
      }

   }
   std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
      LstCardWithMeta = GetCollectionListWithMeta();
   std::ofstream oMetaFile;
   oMetaFile.open(m_szMetaTagFileName + ".txt");
   for (int i = 0; i < LstCardWithMeta.size(); i++)
   {
      if (LstCardWithMeta[i].second.size() > 0)
      {
         std::string szTagLine = ": { ";
         std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTag = LstCardWithMeta[i].second.begin();
         for (; iter_MetaTag != LstCardWithMeta[i].second.end(); ++iter_MetaTag)
         {
            szTagLine += iter_MetaTag->first + "=\"" + iter_MetaTag->second + "\" ";
         }
         szTagLine += "}";
         oMetaFile << LstCardWithMeta[i].first << szTagLine << "\n";
      }
   }

   oMetaFile.close();

   std::ofstream oColFile;
   oColFile.open(aszFileName);
   oColFile << ": Name=\"" + m_szName + "\" \n";
   for (int i = 0; i < lstLines.size(); i++)
   {
      std::cout << lstLines[i] << std::endl;
      oColFile << lstLines[i] << "\n";
   }

   oColFile.close();
}

void Collection::RecordForcedTransaction(std::string aszTransactionString)
{
   m_lstUnreversibleChanges.push_back(aszTransactionString);
}

void Collection::CreateBaselineHistory()
{
   std::vector<std::string> lstLines = GetCollectionList();

   std::string szTimeString = "";
   time_t now = time(0);
   struct tm timeinfo;
   localtime_s(&timeinfo, &now);
   char str[26];
   asctime_s(str, sizeof str, &timeinfo);
   str[strlen(str) - 1] = 0;

   std::ofstream oHistFile;
   oHistFile.open(m_szHistoryFileName + ".txt");

   oHistFile << "[" << str << "] " << std::endl;

   std::vector<std::string>::iterator iter_lines = lstLines.begin();
   for (; iter_lines != lstLines.end(); ++iter_lines)
   {
      oHistFile << "+ " << *iter_lines << "\n";
   }

   oHistFile.close();
}

void Collection::PrintList()
{
   for (int i = 0; i < m_lstCollection.size(); i++)
   {
      for (int t = 0; t < m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetCopies(m_szName).size(); t++)
      {
         std::cout << m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetName() << std::endl;
      }

   }
}

Collection::Transaction* Collection::openTransaction()
{
   // Currently, all i can think of is add/remove item, change/unchange attr of card.
   if ((m_lstTransactions.size() > 0 && !m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen) ||
      !(m_lstTransactions.size() > 0))
   {

      m_lstTransactions.push_back(Collection::Transaction(this));
   }
   return &m_lstTransactions.at(m_lstTransactions.size() - 1);
}

void Collection::finalizeTransaction(bool abRecord)
{
   if (m_lstTransactions.size() > 0 && m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
   {
      m_lstTransactions.at(m_lstTransactions.size() - 1).Finalize(abRecord);
   }
}

std::vector<std::string> Collection::GetCollectionList()
{
   // Now look for all cards with resident col == this
   std::vector<std::string> lstLines;
   std::vector<int>::iterator iter_ResiCards = m_lstCollection.begin();
   for (; iter_ResiCards != m_lstCollection.end(); ++iter_ResiCards)
   {
      int iCacheID = *iter_ResiCards;
      std::vector<CopyObject*> lstPossibleCopies = m_ColSource->GetCardPrototype(iCacheID)->GetCopies(m_szName);
      std::vector<CopyObject*>::iterator iter_PossibleCopies = lstPossibleCopies.begin();

      // Map all the copies that are identical to the same bucket
      std::vector<std::pair<CopyObject*, int>> lstBuckets;
      for (; iter_PossibleCopies != lstPossibleCopies.end(); ++iter_PossibleCopies)
      {
         CopyObject* oCurrentCard = *iter_PossibleCopies;
         bool bFound = false;

         // Check already processed copies
         std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
         for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
         {
            CopyObject* oCompareCard = iter_Bucket->first;
            if (bFound |= CollectionObject::IsSameIdentity(oCompareCard, oCurrentCard))
            {
               iter_Bucket->second = iter_Bucket->second + 1;
               break;
            }
         }

         // If not found, add it to the buckets.
         if (!bFound)
         {
            lstBuckets.push_back(std::make_pair(*iter_PossibleCopies, 1));
         }
         else
         {
            // It should have already been ticked up.
         }
      }

      std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
      for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
      {
         std::string szLine = cardToString(*iter_ResiCards, &(*iter_Bucket));
         lstLines.push_back(szLine);
      }
   }

   return lstLines;
}

std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
Collection::GetCollectionListWithMeta()
{
   std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> lstRetVal;
   std::vector<int>::iterator iter_ResiCards = m_lstCollection.begin();
   for (; iter_ResiCards != m_lstCollection.end(); ++iter_ResiCards)
   {
      int iCacheID = *iter_ResiCards;
      std::vector<CopyObject*> lstPossibleCopies = m_ColSource->GetCardPrototype(iCacheID)->GetCopies(m_szName);
      std::vector<CopyObject*>::iterator iter_PossibleCopies = lstPossibleCopies.begin();

      // Map all the copies that are identical to the same bucket
      std::vector<std::pair<CopyObject*, int>> lstBuckets;
      std::vector<std::vector<std::pair<std::string, std::string>>> lstTagBucket;
      for (; iter_PossibleCopies != lstPossibleCopies.end(); ++iter_PossibleCopies)
      {
         CopyObject* oCurrentCard = *iter_PossibleCopies;
         bool bFound = false;

         // Check already processed copies
         std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
         for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
         {
            CopyObject* oCompareCard = iter_Bucket->first;
            if (CollectionObject::IsSameIdentity(oCompareCard, oCurrentCard))
            {
               // The meta tags need also be the same
               if (bFound |= CopyObject::IsSameMetaTags(oCompareCard->GetMetaTags(m_szName), oCurrentCard->GetMetaTags(m_szName)))
               {
                  iter_Bucket->second = iter_Bucket->second + 1;
                  break;
               }

            }
         }

         // If not found, add it to the buckets.
         if (!bFound)
         {
            lstBuckets.push_back(std::make_pair(*iter_PossibleCopies, 1));
            lstTagBucket.push_back(oCurrentCard->GetMetaTags(m_szName));
         }
         else
         {
            // It should have already been ticked up.
         }
      }

      for (int i = 0; i < lstBuckets.size(); i++)
      {
         std::string szLine = cardToString(*iter_ResiCards, &(lstBuckets[i]));
         lstRetVal.push_back(std::make_pair(szLine, lstTagBucket[i]));
      }
   }

   return lstRetVal;
}



std::vector<std::pair<std::string, std::string>> Collection::ParseAttrs(std::string aszAttrs)
{
   std::vector<std::pair<std::string, std::string>> lstKeyVals;
   std::vector<std::string> lstDetails = SourceObject::Str_Split(aszAttrs, " ");
   for (std::vector<std::string>::iterator iter_attrs = lstDetails.begin(); iter_attrs != lstDetails.end(); ++iter_attrs)
   {
      std::vector<std::string> lstPairs = SourceObject::Str_Split(*iter_attrs, "=");
      if (lstPairs.size() > 1)
      {
         std::vector<std::string> lstVal = SourceObject::Str_Split(lstPairs[1], "\"");
         if (lstVal.size() == 3)
         {
            std::string szVal = lstVal[1];
            lstKeyVals.push_back(std::make_pair(lstPairs[0], szVal));
         }

      }


   }
   return lstKeyVals;
}

bool Collection::ParseCardLine(std::string aszLine, int& riCount, std::string& rszName, std::string& rszDetails)
{
   int i = 0;
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
      int iNum = std::stoi(szNum);
      riCount = iNum;
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

   std::string szName = "";
   int iter_size = aszLine.size();
   while (i < iter_size &&
      ((aszLine.at(i) >= 'a' && aszLine.at(i) <= 'z') ||
      (aszLine.at(i) >= 'A' && aszLine.at(i) <= 'Z') ||
         (aszLine.at(i) == ',' || aszLine.at(i) == ' ')))
   {
      szName = szName + aszLine.at(i);
      i++;
   }

   szName.erase(0, szName.find_first_not_of(' '));
   szName.erase(szName.find_last_not_of(' ') + 1);

   // Output the name
   rszName = szName;

   while (i < iter_size && (aszLine.at(i) == ' ' || aszLine.at(i) != '{'))
   {
      i++;
   }

   bool hasDets = false;
   if (i < iter_size)
   {
      if (aszLine.at(i) == '{')
      {
         hasDets = true;
      }
   }


   std::string szDetails = "";
   if (i < iter_size && hasDets)
   {

      while (i < iter_size)
      {
         szDetails += aszLine.at(i);
         i++;
      }
   }

   // Output the details
   rszDetails = szDetails;
}




void Collection::RollbackTransaction()
{
   if (!m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
   {
      m_lstTransactions.at(m_lstTransactions.size() - 1).Rollback();
      m_lstTransactions.pop_back();
   }
}


std::string Collection::cardToString(int aiCardProto, std::pair<CopyObject*, int>* aoCopy, bool bFullDets)
{
   std::string szLine = "";
   if (aoCopy->second > 1)
   {
      szLine += std::to_string(aoCopy->second);
      szLine += "x ";
   }

   szLine += m_ColSource->GetCardPrototype(aiCardProto)->GetName();
   szLine += " ";

   if (aoCopy->first->NonUniqueTraits.size() > 0 ||
      (aoCopy->first->ParentCollection != m_szName) || bFullDets)
   {
      szLine += "{ ";
      if (aoCopy->first->ParentCollection != m_szName || bFullDets)
      {
         szLine += "Parent=\"";
         szLine += aoCopy->first->ParentCollection;
         szLine += "\" ";
      }

      if (aoCopy->first->NonUniqueTraits.size() > 0)
      {
         std::map<std::string, std::string>::iterator iter_keyValPairs = aoCopy->first->NonUniqueTraits.begin();
         for (; iter_keyValPairs != aoCopy->first->NonUniqueTraits.end(); ++iter_keyValPairs)
         {
            szLine += iter_keyValPairs->first;
            szLine += "=\"";
            szLine += iter_keyValPairs->second;
            szLine += "\" ";
         }

      }
      szLine += "}";
   }

   return szLine;
}

std::vector<std::string> Collection::splitIntoLines(std::string aszString)
{
   std::vector<std::string> lstLines;
   std::string szLine = "";
   for (int i = 0; i < aszString.size(); i++)
   {

      if (aszString[i] == '\n')
      {
         lstLines.push_back(szLine);
         szLine = "";
      }
      else
      {
         szLine = szLine + aszString[i];
         if (i == aszString.size() - 1)
         {
            lstLines.push_back(szLine);
            szLine = "";
         }
      }

   }
   return lstLines;
}