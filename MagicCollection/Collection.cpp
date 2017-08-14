#include "Collection.h"
#include "Addresser.h"
#include "AddAction.h"
#include "RemoveAction.h"
#include "ChangeAction.h"
#include "ReplaceAction.h"
#include "AddFromAction.h"

using namespace std;

// An ID will be given to the collection if there is a parent.
Collection::Collection(string aszName, 
	CollectionSource* aoSource,
	string aszFileCollection, 
	string aszID)
{
   m_ptrCollectionTracker = new CollectionTracker(this);
   m_ptrCollectionDetails = new CollectionDetails();
   m_ptrTransactionManager = new TransactionManager(this);

   m_ptrCollectionDetails->SetName(aszName);
   m_ptrCollectionDetails->SetFileName(aszFileCollection);
   m_ptrCollectionDetails->SetChildrenCount(0);
   m_ptrCollectionDetails->AssignAddress(aszID);

   m_ptrCollectionSource = aoSource;

   m_ptrCollectionTracker->Track();
}


Collection::~Collection()
{
   delete m_ptrCollectionTracker;
   delete m_ptrCollectionDetails;
   delete m_ptrTransactionManager;
}

string Collection::GetName()
{
   return m_ptrCollectionDetails->GetName();
}

Address Collection::GetIdentifier()
{
   return Address(*m_ptrCollectionDetails->GetAddress());
}

unsigned int 
Collection::GetChildCount()
{
   return m_ptrCollectionDetails->GetChildrenCount();
}

void 
Collection::ChildAdded()
{
   m_ptrCollectionDetails->IncrementChildCount();
}

void
Collection::AddItem(string aszName,
   vector<Tag> alstAttrs,
   vector<Tag> alstMetaTags,
   bool abCloseTransaction)
{
   AddAction addAction;
   addAction.SetIDs(alstAttrs);
   addAction.SetMeta(alstMetaTags);
   addAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(addAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void
Collection::AddItemFrom(
   std::string aszName,
   std::string aszIdentifyingHash,
   const Address& aAddress,
   bool abCloseTransaction)
{
   AddFromAction afAction;
   afAction.SetHash(aszIdentifyingHash);
   afAction.SetName(aszName);
   afAction.SetResi(aAddress);

   m_ptrTransactionManager->IncludeAction(afAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction);
}

void 
Collection::RemoveItem(string aszName, 
	string aszIdentifyingHash, 
	bool abCloseTransaction)
{
   RemoveAction rmAction;
   rmAction.SetResi(GetIdentifier());
   rmAction.SetHash(aszIdentifyingHash);
   rmAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(rmAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction); 
}

void 
Collection::ChangeItem(string aszName, 
	string aszIdentifyingHash, 
	vector<Tag> alstChanges, 
	vector<Tag> alstMetaChanges, 
	bool abCloseTransaction)
{
   ChangeAction chAction;
   chAction.SetIDs(alstChanges);
   chAction.SetMeta(alstMetaChanges);
   chAction.SetHash(aszIdentifyingHash);
   chAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(chAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction); 
}

void 
Collection::ReplaceItem(
	string aszName, 
	string aszIdentifyingHash, 
	string aszNewName, 
	vector<Tag> alstIdChanges, 
	vector<Tag> alstMetaChanges, 
	bool abCloseTransaction)
{
   ReplaceAction rpAction;
   rpAction.SetIDs(alstIdChanges);
   rpAction.SetMeta(alstMetaChanges);
   rpAction.SetNewCard(aszNewName);
   rpAction.SetHash(aszIdentifyingHash);
   rpAction.SetName(aszName);

   m_ptrTransactionManager->IncludeAction(rpAction);

   m_ptrTransactionManager->FinalizeTransaction(abCloseTransaction); 
}

vector<string> 
Collection::GetMetaData()
{
   vector<string> lstRetval;
   lstRetval.push_back("Name=\"" + GetName() + "\"");

   lstRetval.push_back("ID=\"" + GetIdentifier().GetFullAddress() + "\"");

   for each(auto metaData in m_lstTaggedItems)
   {
	  string szMetaData = metaData.first + ": {" +
					          metaData.second.first + "=\"" +
	                      metaData.second.second + "\" }";
      lstRetval.push_back(szMetaData);
   }

   return lstRetval;
}

void  
Collection::SaveCollection()
{
   saveHistory();

   saveMeta();

   saveCollection();
}

void 
Collection::LoadCollection(
	string aszFileName, 
	CollectionFactory* aoFactory)
{
   vector<string> lstPreprocessLines;
   vector<string> lstCardLines;
   map<int, list<CopyItem*>> mapNewlyAddedItems;
   map<int, list<CopyItem*>> mapExistingItems;
   vector<string> lstFileLines;
   CollectionIO loader;

   lstFileLines = loader.GetFileLines(aszFileName);

   loader.GetPreprocessLines(lstFileLines, lstCardLines, lstPreprocessLines);

   // This must be done first.
   loadPreprocessingLines(lstPreprocessLines);

   loader.CaptureUnlistedItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapExistingItems,
      mapNewlyAddedItems);

   LoadChanges(lstCardLines);
   loadMetaTagFile();

   loader.CaptureUnlistedItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapNewlyAddedItems,
      mapExistingItems);

   // Consolodate local items that match exactly.
   loader.ConsolodateLocalItems(GetIdentifier(),
      m_ptrCollectionSource,
      mapNewlyAddedItems,
      mapExistingItems);

   loader.RejoinAsyncedLocalItems(GetIdentifier(),
      m_ptrCollectionSource,
      m_ptrCollectionDetails->GetTimeStamp(),
      mapNewlyAddedItems,
      mapExistingItems);

   // Now Verify Borrowed Cards (i.e. Parent != this) that were just
   //  loaded exist. Two things can happen in this case. 
   // If the claimed collection exists, then try to find the referenced item
   //  and use that instead, if that fails, delete the item.
   // If the claimed collection does not exist, then try to find an identical
   //  copy that may have been created by another collection and use that.
   //  If that fails, use the one created.
   loader.ConsolodateBorrowedItems(GetIdentifier(),
      m_ptrCollectionSource,
      aoFactory);

   // Now this collection is COMPLETELY LOADED. Since other collections can
   //  reference this collection, without this collection being loaded,
   //  those other collections may have created copies of card in this 
   //  collection already; if that is the case, use those copies. Additionally,
   //  check that all the copies referenced by the other collections still
   //  exist, if not, delete those copies.
   loader.ReleaseUnfoundReferences(GetIdentifier(),
      m_ptrCollectionSource);

   IsLoaded = (GetIdentifier().Main != "");

   if (IsLoaded){ m_ptrCollectionTracker->Track(); }
}

// Returns all the copies impacted by this function.
void
Collection::LoadChanges(vector<string> lstLines)
{
   vector<string>::iterator iter_Lines = lstLines.begin();
   for (; iter_Lines != lstLines.end(); ++iter_Lines)
   {
      loadInterfaceLine(*iter_Lines);
   }
}

vector<string> 
Collection::GetCollectionList(MetaTagType atagType, bool aiCollapsed)
{
   function<string(pair<string, int>)> fnExtractor;
   fnExtractor = [](pair<string, int> pVal)
                   ->string { return pVal.first; };
   vector<string> lstRetVal;
   vector<pair<string, int>> lstSeenHashes;
   vector<int> lstCol = getCollection();
   vector<int>::iterator iter_Items = lstCol.begin();
   for (; iter_Items != lstCol.end(); ++iter_Items)
   {
      TryGet<CollectionItem> item = m_ptrCollectionSource->
		  GetCardPrototype(*iter_Items);
      vector<shared_ptr<CopyItem>> lstCopies = item->
		  GetCopiesForCollection(GetIdentifier(), All);

      vector<shared_ptr<CopyItem>>::iterator iter_Copy = lstCopies.begin();
      for (; iter_Copy != lstCopies.end(); ++iter_Copy)
      {
         string szHash = (*iter_Copy)->GetHash();
         int iCounted = ListHelper::List_Find(szHash, lstSeenHashes,
                                              fnExtractor);
         if (!aiCollapsed || (iCounted == -1))
         {
            string szRep = item->GetCardString(iter_Copy->get(), atagType, 
                                               GetIdentifier());
            lstRetVal.push_back(szRep);
            lstSeenHashes.push_back(make_pair(szHash, 1));
         }
         else if (iCounted != -1)
         {
            lstSeenHashes[iCounted].second++;
         }
      }
   }

   if (aiCollapsed)
   {
      vector<string> lstNewRetVal;
      for (size_t i = 0; i < lstRetVal.size(); i++)
      {
         int iCounted = lstSeenHashes[i].second;
         lstNewRetVal.push_back("x" + to_string(iCounted) + " " + lstRetVal[i]);
      }

      lstRetVal.clear();
      lstRetVal = lstNewRetVal;
   }

   return lstRetVal;
}

vector<int> 
Collection::getCollection()
{
   if (m_ptrCollectionSource->IsSyncNeeded(GetIdentifier()))
   {
      m_lstItemCacheIndexes = m_ptrCollectionSource->
                              GetCollectionCache(GetIdentifier());

      m_ptrTransactionManager->TransactionsAsynced();
   }

   return m_lstItemCacheIndexes;
}

void 
Collection::addItem(
	string aszName, 
	vector<Tag> alstAttrs, 
	vector<Tag> alstMetaTags)
{
   TryGet<CollectionItem> item; CopyItem* cItem; string szHash;
   
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   item = m_ptrCollectionSource->GetCardPrototype(iCache);

   cItem = item->AddCopyItem(GetIdentifier(), alstAttrs, alstMetaTags);
   szHash = cItem->GetHash();

   registerItem(iCache);

   // Notify other collections they may need to sync since this may have
   //  been borrowed by other collections.
   // There should never be other copies with that hash not in resident. 
   // They are removed at load time.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void 
Collection::addItemFrom(
   std::string aszName,
   std::string aszIdentifyingHash,
   const Address& aResiAddress)
{
   TryGet<CollectionItem> item;
   std::shared_ptr<CopyItem> cItem;

   int iCache = m_ptrCollectionSource->LoadCard(aszName);
   item = m_ptrCollectionSource->GetCardPrototype(iCache);
   if (!item.Good()) { return; }

   cItem = item->FindCopyItem(aszIdentifyingHash, aResiAddress);
   if (cItem.get() == nullptr) { return; }

   cItem->AddResident(GetIdentifier());

   registerItem(iCache);
}

void 
Collection::removeItem( string aszName, 
                        string aszIdentifyingHash, 
                        Address aAddrResidentIn )
{
   // The copy is already verified to exist at this point
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   TryGet<CollectionItem> item = m_ptrCollectionSource->GetCardPrototype(iCache);

   item->RemoveCopyItem(aAddrResidentIn, aszIdentifyingHash);

   // Remove any items from the cache that are no longer in this collection.
   if (item->GetCopiesForCollection(GetIdentifier(), All).size() == 0)
   {
      vector<int> lstNewCacheIndexes;
      for (size_t i = 0; i < m_lstItemCacheIndexes.size(); i++)
      {
         if (m_lstItemCacheIndexes[i] != iCache)
         {
            lstNewCacheIndexes.push_back(m_lstItemCacheIndexes[i]);
         }
      }
      m_lstItemCacheIndexes = lstNewCacheIndexes;
   }

   // Notify other collections they may need to sync since this may have been 
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void 
Collection::changeItem( string aszName, 
                        string aszIdentifyingHash, 
                        vector<Tag> alstChanges, 
                        vector<Tag> alstMetaChanges )
{
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   TryGet<CollectionItem> item = m_ptrCollectionSource->GetCardPrototype(iCache);
   CopyItem* cItem = item->FindCopyItem(aszIdentifyingHash).get();
   if (cItem == nullptr) { return; }

   modifyItem(cItem, alstChanges, alstMetaChanges);

   // Notify other collections they may need to sync since this may have been
   //  borrowed by other collections.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}


void 
Collection::replaceItem( string aszName,
                         string aszIdentifyingHash, 
                         string aszNewName, 
                         vector<Tag> alstIdChanges, 
                         vector<Tag> alstMetaChanges )
{
   int iCache = m_ptrCollectionSource->LoadCard(aszName);
   int iNewCache = m_ptrCollectionSource->LoadCard(aszNewName);

   TryGet<CollectionItem> item = m_ptrCollectionSource->GetCardPrototype(iCache);
   TryGet<CollectionItem> newItem = m_ptrCollectionSource->GetCardPrototype(iNewCache);
   CopyItem* cItem = item->FindCopyItem(aszIdentifyingHash).get();
   if (cItem == nullptr) { return; }

   removeItem(item->GetName(), cItem->GetHash(), GetIdentifier());
   addItem(newItem->GetName(), alstIdChanges, alstMetaChanges);
}

void 
Collection::registerItem(int aiCacheIndex)
{
   int iFound = ListHelper::List_Find(aiCacheIndex, m_lstItemCacheIndexes);

   if (iFound == -1)
   {
      m_lstItemCacheIndexes.push_back(aiCacheIndex);
   }
}

void 
Collection::modifyItem(
	CopyItem* aptCopy, 
	vector<Tag> alstChanges, 
	vector<Tag> alstMetaChanges)
{
   for (size_t i = 0; i < alstChanges.size(); i++)
   {
      aptCopy->
		  SetIdentifyingAttribute(alstChanges[i].first, alstChanges[i].second);
   }

   for (size_t i = 0; i < alstMetaChanges.size(); i++)
   {
      MetaTagType mTagType = CopyItem::DetermineMetaTagType(alstMetaChanges[i].first);
      aptCopy->
		  SetMetaTag(alstMetaChanges[i].first, alstMetaChanges[i].second, mTagType);
   }
}

void Collection::loadMetaTagFile()
{
   // This should only be called during initial loading.
   CollectionIO ioHelper;
   string szFileName = ioHelper.GetMetaFile(m_ptrCollectionDetails->GetFileName());
   vector<string> lstMetaLines = ioHelper.GetFileLines(szFileName);

   for (size_t i = 0; i < lstMetaLines.size(); i++)
   {
      CollectionItem::PseudoIdentifier sudoItem;
      CollectionItem::ParseCardLine(lstMetaLines[i], sudoItem);

      vector<Tag> lstMetaTags = sudoItem.MetaTags;

      // Clear the meta so the hash may be obtained.
      sudoItem.MetaString = "";
      sudoItem.MetaTags.clear();

      int iRealCard = m_ptrCollectionSource->LoadCard(sudoItem.Name);
      if (iRealCard == -1) { continue; }

      TryGet<CollectionItem> item = m_ptrCollectionSource->GetCardPrototype(iRealCard);
      string szPlainHash = item->GetHash(GetIdentifier(), sudoItem.Identifiers);

      // Gets the first matching item resident in this collection.
      CopyItem* matchingCopy = item->FindCopyItem(szPlainHash, GetIdentifier()).get();
      if (matchingCopy != nullptr)
      {
         for (size_t t = 0; t < lstMetaTags.size(); t++)
         {
            MetaTagType mTagType = CopyItem::DetermineMetaTagType(lstMetaTags[t].first);
            matchingCopy->
				SetMetaTag(lstMetaTags[t].first, lstMetaTags[t].second, mTagType, false);
         }
      }

   }
}

void Collection::loadPreprocessingLines(vector<string>  alstLines)
{
   vector<string>::iterator iter_Lines = alstLines.begin();
   for (; iter_Lines != alstLines.end(); ++iter_Lines)
   {
      loadPreprocessingLine(*iter_Lines);
   }
}

void Collection::loadPreprocessingLine(string aszLine)
{
   string szDefKey(Config::CollectionDefinitionKey);
   if (aszLine.size() < 2) { return; }
   if (aszLine.substr(0, szDefKey.size()) != szDefKey) { return; }

   string szBaseLine = aszLine.substr(2);
   vector<string> lstSplitLine = StringHelper::Str_Split(szBaseLine, "=");

   if (lstSplitLine.size() != 2) { return; }

   vector<string>::iterator iter_Lines = lstSplitLine.begin();
   for (; iter_Lines != lstSplitLine.end(); ++iter_Lines)
   {
      *iter_Lines = StringHelper::Str_Trim(*iter_Lines, ' ');
   }

   string szKey = lstSplitLine.at(0);
   string szValue = lstSplitLine.at(1);
   szValue = StringHelper::Str_Trim(szValue, '\"');

   if (szKey == "Name")
   {
      m_ptrCollectionDetails->SetName(szValue);
   }
   else if (szKey == "ID")
   {
      m_ptrCollectionDetails->AssignAddress(szValue);
   }
   else if (szKey == "CC")
   {
      m_ptrCollectionDetails->SetChildrenCount(stoi(szValue));
   }
   else if (szKey == "Session")
   {
      tm tm{};
      istringstream str_stream(szValue);
      str_stream >> get_time(&tm, "%Y-%m-%d_%T");
      time_t time = mktime(&tm);
      m_ptrCollectionDetails->SetTimeStamp(time);
   }
}

// May return null depending on input
void Collection::loadInterfaceLine(string aszLine)
{
   if (aszLine.size() <= 2) { return; }

   string szTrimmedLine = StringHelper::Str_Trim(aszLine, ' ');

   string szLoadDirective = szTrimmedLine.substr(0, 1);

   if (szLoadDirective == "-") // REMOVE
   {
      szTrimmedLine = szTrimmedLine.substr(1);
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadRemoveLine(szTrimmedLine);
   }
   else if (szLoadDirective == "%") // CHANGE
   {
      szTrimmedLine = szTrimmedLine.substr(1);
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }] ->
      //   Another Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadDeltaLine(szTrimmedLine);
   }
   else // ADD
   {
      if (szLoadDirective == "+")
      {
         szTrimmedLine = szTrimmedLine.substr(1);
      }
      // Of the form
      // Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
      loadAdditionLine(szTrimmedLine);
   }
}

void Collection::loadAdditionLine(string aszLine)
{
   Address aParentAddress;
   string szID = "";
   CollectionItem::PseudoIdentifier sudoItem;
   bool bThisIsParent = true;

   CollectionItem::ParseCardLine(aszLine, sudoItem);

   int iFoundAddress = ListHelper::List_Find( string("Parent"),
                                              sudoItem.MetaTags,
                                              Config::Instance()->GetTagHelper() );
   if (iFoundAddress != -1)
   {
      aParentAddress = Address(sudoItem.MetaTags.at(iFoundAddress).second);
      int iFoundHash = ListHelper::List_Find( string(Config::HashKey),
                                              sudoItem.MetaTags,
                                              Config::Instance()->GetTagHelper() );
      szID = sudoItem.MetaTags.at(iFoundHash).second;
      bThisIsParent = !(aParentAddress == GetIdentifier());
   }

   // If the ID is specified, then we assume the card already exists.
   if (!bThisIsParent && // This is not the parent
       szID != "")       // and the id was specified.
   {
      for (size_t i = 0; i < sudoItem.Count; i++)
      {
         AddItemFrom(sudoItem.Name, szID, aParentAddress);
      }
   }
   // If the parent was not specified, or this was designated the parent
   // without an ID, then add it as a new card.
   else
   {
      for (size_t i = 0; i < sudoItem.Count; i++)
      {
         AddItem(sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags);
      }

   }
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
void Collection::loadRemoveLine(string aszLine)
{
   CollectionItem::PseudoIdentifier sudoItem;
   CollectionItem::ParseCardLine(aszLine, sudoItem);

   for (size_t i = 0; i < sudoItem.Count; i++)
   {
      string szHash;
      int iHash = ListHelper::List_Find( string(Config::HashKey), 
                                         sudoItem.MetaTags, 
                                         Config::Instance()->GetTagHelper());
      if (iHash != -1)
      {
         szHash = sudoItem.MetaTags[iHash].second;
         RemoveItem(sudoItem.Name, szHash);
      }
      else { break; }
   }
}
void Collection::loadDeltaLine(string aszLine)
{
   vector<string> lstOldNew = StringHelper::Str_Split(aszLine, "->");

   CollectionItem::PseudoIdentifier sudoOldItem;
   CollectionItem::ParseCardLine(lstOldNew[0], sudoOldItem);

   CollectionItem::PseudoIdentifier sudoNewItem;
   CollectionItem::ParseCardLine(lstOldNew[1], sudoNewItem);

   string szHash;
   int iCache;
   int iHash = ListHelper::List_Find( string(Config::HashKey),
                                      sudoOldItem.MetaTags, 
                                      Config::Instance()->GetTagHelper() );
   if (iHash != -1 && 
      (iCache = m_ptrCollectionSource->LoadCard(sudoOldItem.Name)) != -1)
   {
      string szHash; TryGet<CollectionItem> itemOld; CopyItem* cItem;
      
      szHash = sudoOldItem.MetaTags[iHash].second;
      itemOld = m_ptrCollectionSource->GetCardPrototype(iCache);
      cItem = itemOld->FindCopyItem(szHash).get();

      for (size_t i = 0; i < sudoOldItem.Count; i++)
      {
         int iNewCache;
         if (sudoOldItem.Name == sudoNewItem.Name)
         {
            ChangeItem( sudoOldItem.Name,
                        szHash, 
                        sudoNewItem.Identifiers, 
                        sudoNewItem.MetaTags );
         }
         else if ((iNewCache = m_ptrCollectionSource->LoadCard(sudoNewItem.Name)) != -1)
         {
            TryGet<CollectionItem> itemNew = m_ptrCollectionSource->
                                      GetCardPrototype(iNewCache);
            ReplaceItem( sudoOldItem.Name, 
                         szHash, 
                         sudoNewItem.Name, 
                         sudoNewItem.Identifiers, 
                         sudoNewItem.MetaTags );
         }
      }

   }

}

void Collection::saveHistory()
{
   m_ptrCollectionTracker->Track();
   CollectionDeltaClass cdc = m_ptrCollectionTracker->CalculateChanges();

   vector<string> lstHistoryLines;
   for (size_t i = 0; i < cdc.Additions.size(); i++)
   {
      lstHistoryLines.push_back( cdc.Additions[i]);
   }

   for (size_t i = 0; i < cdc.Removals.size(); i++)
   {
      lstHistoryLines.push_back( cdc.Removals[i]);
   }

   for (size_t i = 0; i < cdc.Changes.size(); i++)
   {
      lstHistoryLines.push_back( cdc.Changes[i]);
   }

   if (lstHistoryLines.size() > 0)
   {
      string szTimeString = "";
      time_t now = time(0);
      struct tm timeinfo;
      localtime_s(&timeinfo, &now);
      char str[26];
      asctime_s(str, sizeof str, &timeinfo);
      str[strlen(str) - 1] = 0;

      CollectionIO ioHelper;
      ofstream oHistFile;
      string szHistFile = ioHelper.GetHistoryFile(m_ptrCollectionDetails->GetFileName());
      oHistFile.open(szHistFile, ios_base::app);

      oHistFile << "[" << str << "] " << endl;

      vector<string>::iterator iter_histLines = lstHistoryLines.begin();
      for (; iter_histLines != lstHistoryLines.end(); ++iter_histLines)
      {
         oHistFile << *iter_histLines << endl;
      }

      oHistFile.close();
   }
}

void Collection::saveMeta()
{
   vector<string> lstMetaLines = GetCollectionList(Persistent);

   CollectionIO ioHelper;
   ofstream oMetaFile;
   oMetaFile.open(ioHelper.GetMetaFile(m_ptrCollectionDetails->GetFileName()));

   vector<string>::iterator iter_MetaLine = lstMetaLines.begin();
   for (; iter_MetaLine != lstMetaLines.end(); ++iter_MetaLine)
   {
      if (iter_MetaLine->find_first_of(':') != string::npos)
      {
         oMetaFile << *iter_MetaLine << endl;
      }
   }

   oMetaFile.close();
}

void Collection::saveCollection()
{
   vector<string> lstLines = GetCollectionList(None);

   time_t time = m_ptrCollectionDetails->GetTimeStamp();
   tm otm;
   localtime_s(&otm, &time);

   CollectionIO ioHelper;
   ofstream oColFile;
   oColFile.open(ioHelper.GetCollectionFile(m_ptrCollectionDetails->GetFileName()));

   oColFile << Config::CollectionDefinitionKey
	        << " Name=\"" << m_ptrCollectionDetails->GetName() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
	        << " ID=\"" << GetIdentifier().GetFullAddress() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey 
	        << " CC=\"" << m_ptrCollectionDetails->GetChildrenCount() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey 
	        << " Session=\"" << put_time(&otm, "%F_%T") << "\"" << endl;

   vector<string>::iterator iter_Line = lstLines.begin();
   for (; iter_Line != lstLines.end(); ++iter_Line)
   {
      oColFile << *iter_Line << endl;
   }

   oColFile.close();
}
