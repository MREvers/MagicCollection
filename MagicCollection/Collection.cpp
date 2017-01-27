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
 std::vector<std::pair<std::string, std::string>> alstAttrs)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {
      // Construct a string for history.
      std::string szCard = "";
      int iCardProto = m_ColSource->LoadCard(aszNewItem);
      CopyObject oCO = m_ColSource->GetCardPrototype(iCardProto)->GenerateCopy(m_szName);
      CollectionObject::ConstructCopy(oCO, alstAttrs);
      szCard += cardToString(iCardProto, &std::make_pair(&oCO, 1));

      // Store the action and how to undo it here.
      Collection::Action oAction;
      oAction.Identifier = "+ " + szCard;
      oAction.Do = std::bind(&Collection::addItem, this, aszNewItem, alstAttrs);
      oAction.Undo = std::bind(&Collection::removeItem, this, aszNewItem, alstAttrs);

      // Store the arguments
      Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
      oTrans->AddAction(oAction);

      TransactionIntercept = false;
      return;
   }

   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->AddItem(aszNewItem, true, alstAttrs);

   if (bFinal)
   {
      oTrans->Finalize();
   }

}

void Collection::RemoveItem(std::string aszRemoveItem,
 bool bFinal,
 std::vector<std::pair<std::string, std::string>> alstAttrs)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {
      
      int iCardProto = m_ColSource->LoadCard(aszRemoveItem);

      CopyObject oCO;
      if (m_ColSource->GetCardPrototype(iCardProto)->GetCopy(m_szName, alstAttrs, oCO))
      {
         std::string szCard = cardToString(iCardProto, &std::make_pair(&oCO, 1));

         Collection::Action oAction;
         oAction.Identifier = "- " + szCard;
         oAction.Do = std::bind(&Collection::removeItem, this, aszRemoveItem, alstAttrs);
         oAction.Undo = std::bind(&Collection::addItem, this, aszRemoveItem, alstAttrs);

         // Store the arguments
         Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
         oTrans->AddAction(oAction);

         TransactionIntercept = false;
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

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::addItem(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
   int iCard = m_ColSource->LoadCard(aszNewItem);
   if (iCard != -1)
   {
      // Make sure we don't already 'have' this card.
      bool bNewCard = true;
      for (int i = 0; i < m_lstCollection.size(); i++)
      {
         bNewCard &= m_lstCollection.at(i) != iCard;
      }

      if (bNewCard)
      {
         m_lstCollection.push_back(iCard);
      }

      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card. Save the reference if we need to add unique traits.
      CopyObject* oCO = oCard->AddCopy(m_szName);

      CollectionObject::ConstructCopy(*oCO, alstAttrs);
   }

}

void Collection::removeItem(std::string aszRemoveItem, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
   int iCard = m_ColSource->LoadCard(aszRemoveItem);
   if (iCard != -1)
   {
      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card.
      oCard->RemoveCopy(m_szName);
   }
}

void Collection::LoadCollection(std::string aszFileName)
{
   std::ifstream in(aszFileName);
   std::stringstream buffer;
   buffer << in.rdbuf();
   std::string contents(buffer.str());

   std::vector<std::string> lstLines;
   std::string szLine = "";
   for (int i = 0; i < contents.size(); i++)
   {

      if (contents[i] == '\n')
      {
         lstLines.push_back(szLine);
         szLine = "";
      }
      else
      {
         szLine = szLine + contents[i];
         if (i == contents.size() - 1)
         {
            lstLines.push_back(szLine);
            szLine = "";
         }
      }

   }

   for (std::vector<std::string>::iterator iter = lstLines.begin(); iter != lstLines.end(); ++iter)
   {
      int i = 0;
      std::string szNum = "";
      while (i < iter->size() && (*iter).at(i) < '9' && (*iter).at(i) > '0')
      {
         szNum = szNum + iter->at(i);
         i++;
      }

      if (i >= iter->size())
      {
         return;
      }

      if (iter->at(i) == 'x')
      {
         i++;
      }

      if (i >= iter->size())
      {
         return;
      }

      std::string szName = "";
      int iter_size = iter->size();
      while (i < iter_size &&
         ((iter->at(i) >= 'a' && iter->at(i) <= 'z') ||
         (iter->at(i) >= 'A' && iter->at(i) <= 'Z') ||
            (iter->at(i) == ',' || iter->at(i) == ' ')))
      {
         szName = szName + iter->at(i);
         i++;
      }
      
      while (i < iter_size && (iter->at(i) == ' ' || iter->at(i) != '{'))
      {
         i++;
      }

      bool hasDets = false;
      if (i < iter_size)
      {
         if (iter->at(i) == '{')
         {
            hasDets = true;
         }
      }

      std::vector<std::pair<std::string,std::string>> lstKeyVals;
      std::string szParentAttr;
      if (i < iter_size && hasDets)
      {
         std::string szDetails = "";
         while (i < iter_size)
         {
            szDetails += iter->at(i);
            i++;
         }

         std::vector<std::string> lstDetails = SourceObject::Str_Split(szDetails," ");
         for (std::vector<std::string>::iterator iter_attrs = lstDetails.begin(); iter_attrs != lstDetails.end(); ++iter_attrs)
         {
            std::vector<std::string> lstPairs = SourceObject::Str_Split(*iter_attrs, "=");
            if (lstPairs.size() > 0 && lstPairs[0] == "Parent")
            {
               std::vector<std::string> lstVal = SourceObject::Str_Split(lstPairs[1], "\"");
               szParentAttr = lstVal[1];
               lstKeyVals.push_back(std::make_pair("Parent", szParentAttr));
            }
            else
            {
               //iterate through the pairs.
               //lstKeyVals.push_back(std::make_pair(lstPairs[0], szParentAttr));
            }
         }
      }

      // Check if cards with identical exist.
      try
      {
         int iNum = std::stoi(szNum);
         szName.erase(0, szName.find_first_not_of(' '));
         szName.erase(szName.find_last_not_of(' ') + 1);

         std::vector<CopyObject*> lstUsedCopies;
         for (int i = 0; i < iNum; i++)
         {
            // Create a temporary copy.
            CopyObject oCopy = CollectionObject::GenerateCopy(m_szName, lstKeyVals);

            // Check if a copy already exists.

            // If so, is it used already.

            // If not, use it.

            // Otherwise, keep going checking.
            bool bNeedNewCopy = true;
            int iCardIndex = m_ColSource->LoadCard(szName);
            if (iCardIndex != -1)
            {
              std::vector<CopyObject*> lstCopies = m_ColSource->GetCardPrototype(iCardIndex)->GetLocalCopies(oCopy.ParentCollection);
               std::vector<CopyObject*>::iterator iter_possible_dups = lstCopies.begin();
               for (; iter_possible_dups != lstCopies.end(); ++iter_possible_dups)
               {
                  if (CollectionObject::IsSameIdentity(&oCopy, *iter_possible_dups))
                  {
                     // Check if it is used already
                     bool bIsAlreadyUsed = false;
                     std::vector<CopyObject*>::iterator iter_already_used = lstUsedCopies.begin();
                     for (; iter_already_used != lstUsedCopies.end(); ++iter_already_used)
                     {
                        if (*iter_already_used == *iter_possible_dups)
                        {
                           bIsAlreadyUsed = true;
                           break;
                        }
                     }

                     // Use this copy, register this col with the copy.
                     if (!bIsAlreadyUsed)
                     {
                        (*iter_possible_dups)->ResidentCollections.push_back(m_szName);
                        lstUsedCopies.push_back(*iter_possible_dups);
                        bNeedNewCopy = false;
                        break;
                     }
                  }
               }
            }

            bool bCollectionExists = false;
            // Check if a collection exists
            std::vector<std::string>::iterator iter_collections = m_lstLoadedCollectionsBuffer->begin();
            for (; iter_collections != m_lstLoadedCollectionsBuffer->end(); ++iter_collections)
            {
               if (*iter_collections == oCopy.ParentCollection)
               {
                  bCollectionExists = true;
                  break;
               }
            }
            
            // If card copy does not exist, create a new one.
            if (bNeedNewCopy && !bCollectionExists)
            {
               AddItem(szName, false, lstKeyVals);
            }
            else if (bNeedNewCopy && bCollectionExists)
            {
               // Set the parent collection = "" because that card no longer exists in that collection
               std::vector<std::pair<std::string,std::string>>::iterator iter_keyvals = lstKeyVals.begin();
               for (; iter_keyvals != lstKeyVals.end(); ++iter_keyvals)
               {
                  if (iter_keyvals->first == "Parent")
                  {
                     iter_keyvals->second = "";
                     break;
                  }
               }

               AddItem(szName, false, lstKeyVals);
            }
         }

         // Now that the collection is loaded, if there are cards with parent collection == this but
         // this is not in their residents, then that copy no longer exists.
         std::vector<CopyObject*> lstFullCol = m_ColSource->GetCollection(m_szName);
         std::vector<CopyObject*>::iterator iter_ColOs = lstFullCol.begin();
         for (; iter_ColOs != lstFullCol.end(); ++iter_ColOs)
         {
            std::vector<std::string> lstResis = (*iter_ColOs)->ResidentCollections;
            std::vector<std::string>::iterator iter_resi = lstResis.begin();
            bool bFoundParent = false;
            for (; iter_resi != lstResis.end(); ++iter_resi)
            {
               if (*iter_resi == m_szName)
               {
                  bFoundParent = true;
                  break;
               }
            }

            if (!bFoundParent)
            {
               (*iter_ColOs)->ParentCollection = "";
            }
         }
      }
      catch (...)
      {

      }

   }

   finalizeTransaction(false);
}

void Collection::SaveCollection(std::string aszFileName)
{
   std::vector<std::string> lstLines = getCollectionString();

   std::string szTimeString = "";
   time_t now = time(0);
   struct tm timeinfo;
   localtime_s(&timeinfo, &now);
   char str[26];
   asctime_s(str, sizeof str, &timeinfo);
   str[strlen(str) - 1] = 0;

   std::ofstream oHistFile;
   oHistFile.open(m_szHistoryFileName + ".txt", std::ios_base::app);

   oHistFile << "[" << str << "] " << std::endl;

   std::vector<Transaction>::iterator iter_transactions = m_lstTransactions.begin();
   for (; iter_transactions != m_lstTransactions.end(); ++iter_transactions)
   {
      if (iter_transactions->Recordable)
      {
         std::vector<Action>::iterator iter_actions = iter_transactions->Actions.begin();
         for (; iter_actions != iter_transactions->Actions.end(); ++iter_actions)
         {
            oHistFile << iter_actions->Identifier << std::endl;
         }
      }
   }

   oHistFile.close();

   std::ofstream oColFile;
   oColFile.open(aszFileName + ".col");

   for (int i = 0; i < lstLines.size(); i++)
   {
      std::cout << lstLines[i] << std::endl;
      oColFile << lstLines[i] << "\n";
   }

   oColFile.close();
}

void Collection::CreateBaselineHistory()
{
   std::vector<std::string> lstLines = getCollectionString();

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
   if (m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
   {
      m_lstTransactions.at(m_lstTransactions.size() - 1).Finalize(abRecord);
   }
}

std::vector<std::string> Collection::getCollectionString()
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

void Collection::RollbackTransaction()
{
   if (!m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
   {
      m_lstTransactions.at(m_lstTransactions.size() - 1).Rollback();
      m_lstTransactions.pop_back();
   }
}


std::string Collection::cardToString(int aiCardProto, std::pair<CopyObject*, int>* aoCopy)
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
      aoCopy->first->ParentCollection != m_szName)
   {
      szLine += "{ ";
      if (aoCopy->first->ParentCollection != m_szName)
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