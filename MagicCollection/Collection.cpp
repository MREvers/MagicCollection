#include "Collection.h"
#include "Addresser.h"

using namespace std;

// An ID will be given to the collection if there is a parent.
Collection::Collection(string aszName, 
	CollectionSource* aoSource,
	string aszFileCollection, 
	string aszID)
{
   m_szName = aszName;
   m_szFileName = aszFileCollection;
   m_ptrCollectionSource = aoSource;
   m_iChildrenCount = 0;
   m_ulTimeStamp = time(nullptr);

   m_bRecordChanges = true;
}


Collection::~Collection()
{
}

void Collection::SetName(string aszNewName)
{
   m_szName = aszNewName;
}

string Collection::GetName()
{
   return m_szName;
}

Address Collection::GetIdentifier()
{
   return m_Address;
}

void Collection::AddItem(string aszName,
   vector<Tag> alstAttrs,
   vector<Tag> alstMetaTags,
   bool abCloseTransaction)
{
   // Verify the card name entered is valid
   int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
   if (iValidItem == -1) { return; }

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);

   // This is needed for removal
   string szHash = item->GetHash(GetIdentifier(), alstAttrs, alstMetaTags);

   function<void()> fnDo;
   fnDo = bind(
	   &Collection::addItem, 
	   this, 
	   aszName, 
	   alstAttrs, 
	   alstMetaTags);

   function<void()> fnUndo;
   fnUndo = bind(
	   &Collection::removeItem, 
	   this, 
	   aszName, 
	   szHash, 
	   GetIdentifier());

   Action action(fnDo, fnUndo);

   // Use a generated copy to ensure that default values are set.
   CopyItem* newCopy = item->
	   GenerateCopy(GetIdentifier(), alstAttrs, alstMetaTags);
   
   string szIdentifier = "+ " + CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   newCopy->GetIdentifyingAttributes(), 
	   newCopy->GetMetaTags(MetaTagType::Visible), 
	   GetIdentifier());

   delete newCopy;
   
   action.SetIdentifier(szIdentifier); 

   Transaction* transaction = getOpenTransaction();
   transaction->AddAction(action);

   if (abCloseTransaction)
   {
      finalizeTransaction();
   }
}

void Collection::AddItem(string aszName,
	string aszHash, 
	string aszResidentIn, 
	bool abCloseTransaction)
{
   // Verify the card name entered is valid
   int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
   if (iValidItem == -1) { return; }

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);

   // This is needed for removal
   CopyItem* cItem = item->FindCopyItem(aszHash);
   if (cItem == nullptr) { return; }

   function<void()> fnDo;
   fnDo = bind(
	   &Collection::addExistingItem, 
	   this, 
	   aszName, 
	   aszHash, 
	   aszResidentIn);

   function<void()> fnUndo;
   fnUndo = bind(
	   &Collection::removeItem, 
	   this, 
	   aszName, 
	   aszHash, 
	   aszResidentIn);

   Action action(fnDo, fnUndo);

   // Use a generated copy to ensure that default values are set.
   string szIdentifier = "+ " + CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   cItem->GetIdentifyingAttributes(), 
	   cItem->GetMetaTags(MetaTagType::Visible), 
	   GetIdentifier());
   
   //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
   action.SetIdentifier(szIdentifier); 

   Transaction* transaction = getOpenTransaction();
   transaction->AddAction(action);

   if (abCloseTransaction)
   {
      finalizeTransaction();
   }
}

void Collection::RemoveItem(string aszName, 
	string aszIdentifyingHash, 
	bool abCloseTransaction)
{
   int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
   if (iValidItem == -1) { return; }

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
   CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
   if (copy == nullptr) { return; }

   function<void()> fnDo;
   fnDo = bind(
	   &Collection::removeItem, 
	   this, 
	   aszName, 
	   aszIdentifyingHash, 
	   GetIdentifier());

   vector<Tag> lstAttrs = copy->GetIdentifyingAttributes();
   vector<Tag> lstMetas = copy->GetMetaTags(Visible);
   function<void()> fnUndo;
   fnUndo = bind(&Collection::addItem, this, aszName, lstAttrs, lstMetas);

   Action action(fnDo, fnUndo);

   string szIdentifier = "- " + CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   lstAttrs, 
	   lstMetas, 
	   GetIdentifier());

   //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;
   action.SetIdentifier(szIdentifier); 

   Transaction* transaction = getOpenTransaction();
   transaction->AddAction(action);

   if (abCloseTransaction)
   {
      finalizeTransaction();
   }
}

void Collection::ChangeItem(string aszName, 
	string aszIdentifyingHash, 
	vector<Tag> alstChanges, 
	vector<Tag> alstMetaChanges, 
	bool abCloseTransaction)
{
   int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
   if (iValidItem == -1) { return; }

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
   CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
   if (copy == nullptr) { return; }

   function<void()> fnDo;
   fnDo = bind(
	   &Collection::changeItem, 
	   this, 
	   aszName, 
	   aszIdentifyingHash, 
	   alstChanges, 
	   alstMetaChanges);

   // Simulate the change so we can determine the hash and the exact traits when change is complete.
   CopyItem* falseCopy = new CopyItem(*copy);
   modifyItem(falseCopy, alstChanges, alstMetaChanges);
   vector<Tag> lstFutureIds = falseCopy->GetIdentifyingAttributes();
   vector<Tag> lstFutureMeta = falseCopy->GetMetaTags(MetaTagType::Visible);
   delete falseCopy;

   vector<Tag> lstOldIds = copy->GetIdentifyingAttributes();
   vector<Tag> lstOldMeta = copy->GetMetaTags(MetaTagType::Visible);

   // This is the hash that the itme will have after the properties are changed. So we need this to undo this change.
   string szPostHash = item->
	   GetHash(GetIdentifier(), lstFutureIds, lstFutureMeta);

   function<void()> fnUndo;
   fnUndo = bind(
	   &Collection::changeItem, 
	   this, 
	   aszName, 
	   szPostHash, 
	   lstOldIds, 
	   lstOldMeta);

   Action action(fnDo, fnUndo);

   string szIdentifier = "% " + CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   lstOldIds, 
	   lstOldMeta, 
	   GetIdentifier()) + "->";

   szIdentifier += CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   lstFutureIds, 
	   lstFutureMeta, 
	   GetIdentifier());
   action.SetIdentifier(szIdentifier);

   Transaction* transaction = getOpenTransaction();
   transaction->AddAction(action);

   if (abCloseTransaction)
   {
      finalizeTransaction();
   }
}

void Collection::ReplaceItem(
	string aszName, 
	string aszIdentifyingHash, 
	string aszNewName, 
	vector<Tag> alstIdChanges, 
	vector<Tag> alstMetaChanges, 
	bool abCloseTransaction)
{
   int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
   int iValidNewItem = m_ptrCollectionSource->LoadCard(aszNewName);
   if (iValidItem == -1 || iValidNewItem == -1) { return; }

   CollectionItem* item = m_ptrCollectionSource->
	   GetCardPrototype(iValidItem);
   CollectionItem* nItem = m_ptrCollectionSource->
	   GetCardPrototype(iValidNewItem);

   CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
   if (copy == nullptr) { return; }

   function<void()> fnDo;
   fnDo = bind(
	   &Collection::replaceItem, 
	   this, 
	   aszName, 
	   aszIdentifyingHash, 
	   aszNewName, 
	   alstIdChanges, 
	   alstMetaChanges);

   vector<Tag> lstOldIds = copy->GetIdentifyingAttributes();
   vector<Tag> lstOldMeta = copy->GetMetaTags(MetaTagType::Visible);

   // Generate a temp copy to see what the actual result is.
   CopyItem* falseCopy = nItem->
	   GenerateCopy(GetIdentifier(), alstIdChanges, alstMetaChanges);

   vector<Tag> lstFutureIds = falseCopy->GetIdentifyingAttributes();
   vector<Tag> lstFutureMeta = falseCopy->GetMetaTags(MetaTagType::Visible);
   delete falseCopy;

   // This is the hash that the itme will have after the properties are
   // changed. So we need this to undo this change.
   string szPostHash = nItem->
	   GetHash(GetIdentifier(), alstIdChanges, alstMetaChanges);
   function<void()> fnUndo;
   fnUndo = bind(
	   &Collection::replaceItem, 
	   this, 
	   aszNewName, 
	   szPostHash, 
	   aszName, 
	   lstOldIds, 
	   lstOldMeta);

   Action action(fnDo, fnUndo);

   string szIdentifier = "% " + CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszName, 
	   lstOldIds, 
	   lstOldMeta, 
	   GetIdentifier()) + "->";
   szIdentifier += CollectionItem::ToCardLine(
	   GetIdentifier(), 
	   aszNewName, 
	   alstIdChanges, 
	   alstMetaChanges, 
	   GetIdentifier());
   action.SetIdentifier(szIdentifier);

   Transaction* transaction = getOpenTransaction();
   transaction->AddAction(action);

   if (abCloseTransaction)
   {
      finalizeTransaction();
   }
}

vector<string> Collection::GetMetaData()
{
   vector<string> lstRetval;
   lstRetval.push_back("Name=\"" + m_szName + "\"");

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

void  Collection::ChildAdded()
{
   m_iChildrenCount++;
}

int  Collection::ChildCount()
{
   return m_iChildrenCount;
}

long Collection::GetTimeStamp()
{
   return m_ulTimeStamp;
}

void  Collection::SaveCollection()
{
   saveHistory();

   saveMeta();

   saveCollection();
}

void Collection::LoadCollection(
	string aszFileName, 
	CollectionFactory* aoFactory)
{
   vector<string> lstPreprocessLines;
   vector<string> lstCardLines;
   map<int, list<CopyItem*>> mapNewlyAddedItems;
   map<int, list<CopyItem*>> mapExistingItems;
   vector<string> lstFileLines;
   CollectionIO loader;

   m_bRecordChanges = false;

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
      GetTimeStamp(),
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

   m_bRecordChanges = true;
   IsLoaded = (GetIdentifier().Main != "");
}

// Returns all the copies impacted by this function.
void Collection::LoadChanges(vector<string> lstLines)
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
      CollectionItem* item = m_ptrCollectionSource->
		  GetCardPrototype(*iter_Items);
      vector<CopyItem*> lstCopies = item->
		  GetCopiesForCollection(GetIdentifier(), All);

      vector<CopyItem*>::iterator iter_Copy = lstCopies.begin();
      for (; iter_Copy != lstCopies.end(); ++iter_Copy)
      {
         string szHash = (*iter_Copy)->GetHash();
         int iCounted = ListHelper::List_Find(szHash, lstSeenHashes, fnExtractor);
         if (!aiCollapsed || (iCounted == -1))
         {
            string szRep = item->GetCardString(*iter_Copy, atagType, GetIdentifier());
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

void Collection::captureChanges()
{
   bool bChangesCaptured = false;

   map<int, vector<CopyItem*>> mapNewCollection;
   for each (int index in m_lstItemCacheIndexes)
   {
      CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(index);
      vector<CopyItem*> lstCopies = item->
		  GetCopiesForCollection(m_szName, CollectionItemType::All);
      mapNewCollection[index] = lstCopies;
   }

   for each (pair<int, vector<CopyItem*>> itemGroup in m_mapCollectionTracker)
   {
      map<int, vector<CopyItem*>>::iterator iter_oldItems = 
		  mapNewCollection.find(itemGroup.first);
      if (iter_oldItems != mapNewCollection.end())
      {
         // Calculate differences
         mapNewCollection.erase(iter_oldItems);
      }
      else
      {
         // all of these items were removed.
      }
   }

   for each (pair<int, vector<CopyItem*>> itemGroup in mapNewCollection)
   {
      // These are all additions.
   }

   if (bChangesCaptured)
   {
      // Time since epoch
      time_t result = time(nullptr);
      m_ulTimeStamp = result;
   }
}

vector<int> Collection::getCollection()
{
   if (m_ptrCollectionSource->IsSyncNeeded(GetIdentifier()))
   {
      m_lstItemCacheIndexes = m_ptrCollectionSource->
                              GetCollectionCache(GetIdentifier());
   }

   return m_lstItemCacheIndexes;
}

void Collection::addItem(
	string aszName, 
	vector<Tag> alstAttrs, 
	vector<Tag> alstMetaTags)
{
   CollectionItem* item; CopyItem* cItem; string szHash;
   
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   item = m_ptrCollectionSource->GetCardPrototype(iCache);

   cItem = item->AddCopyItem(m_Address, alstAttrs, alstMetaTags);
   szHash = cItem->GetHash();

   registerItem(iCache);

   // Notify other collections they may need to sync since this may have
   //  been borrowed by other collections.
   // There should never be other copies with that hash not in resident. 
   // They are removed at load time.
   m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void Collection::addExistingItem(string aszName, string aszHash, string aszResidentIn)
{

}

void 
Collection::removeItem( string aszName, 
                        string aszIdentifyingHash, 
                        Address aAddrResidentIn )
{
   // The copy is already verified to exist at this point
   int iCache = m_ptrCollectionSource->LoadCard(aszName);

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);

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

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
   CopyItem* cItem = item->FindCopyItem(aszIdentifyingHash);
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

   CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
   CollectionItem* newItem = m_ptrCollectionSource->GetCardPrototype(iNewCache);
   CopyItem* cItem = item->FindCopyItem(aszIdentifyingHash);
   if (cItem == nullptr) { return; }

   removeItem(item->GetName(), cItem->GetHash(), GetIdentifier());
   addItem(newItem->GetName(), alstIdChanges, alstMetaChanges);
}

void Collection::registerItem(int aiCacheIndex)
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

Transaction* Collection::getOpenTransaction()
{
   if (m_lstTransactions.size() == 0 ||
      !m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen())
   {
      m_lstTransactions.push_back(Transaction(this));
   }

   return &m_lstTransactions.at(m_lstTransactions.size() - 1);
}

void Collection::finalizeTransaction()
{
   Transaction* transaction = getOpenTransaction();
   if (transaction->IsOpen())
   {
      transaction->Finalize(m_bRecordChanges);
   }
}

void Collection::loadMetaTagFile()
{
   // This should only be called during initial loading.
   CollectionIO ioHelper;
   string szFileName = ioHelper.GetMetaFile(m_szFileName);
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

      CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iRealCard);
      string szPlainHash = item->GetHash(GetIdentifier(), sudoItem.Identifiers);

      // Gets the first matching item resident in this collection.
      CopyItem* matchingCopy = item->FindCopyItem(szPlainHash, GetIdentifier());
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
      m_szName = szValue;
   }
   else if (szKey == "ID")
   {
      m_Address = Address(szValue);
   }
   else if (szKey == "CC")
   {
      m_iChildrenCount = stoi(szValue);
   }
   else if (szKey == "Session")
   {
      tm tm{};
      istringstream str_stream(szValue);
      str_stream >> get_time(&tm, "%Y-%m-%d_%T");
      time_t time = mktime(&tm);
      m_ulTimeStamp = time;
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
   CollectionItem::PseudoIdentifier sudoItem;
   CollectionItem::ParseCardLine(aszLine, sudoItem);

   for (size_t i = 0; i < sudoItem.Count; i++)
   {
      AddItem(sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags);
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
      string szHash; CollectionItem* itemOld; CopyItem* cItem;
      
      szHash = sudoOldItem.MetaTags[iHash].second;
      itemOld = m_ptrCollectionSource->GetCardPrototype(iCache);
      cItem = itemOld->FindCopyItem(szHash);

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
            CollectionItem* itemNew = m_ptrCollectionSource->
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
   vector<string> lstHistoryLines;
   for (size_t i = 0; i < m_lstTransactions.size(); i++)
   {
      vector<string> lstTransLines = m_lstTransactions[i].GetDescriptions();
      for (size_t t = 0; t < lstTransLines.size(); t++)
      {
         lstHistoryLines.push_back(lstTransLines[t]);
      }
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
      string szHistFile = ioHelper.GetHistoryFile(m_szFileName);
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
   oMetaFile.open(ioHelper.GetMetaFile(m_szFileName));

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

   time_t time = m_ulTimeStamp;
   tm otm;
   localtime_s(&otm, &time);

   CollectionIO ioHelper;
   ofstream oColFile;
   oColFile.open(ioHelper.GetCollectionFile(m_szFileName));

   oColFile << Config::CollectionDefinitionKey
	        << " Name=\"" << m_szName << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey
	        << " ID=\"" << GetIdentifier().GetFullAddress() << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey 
	        << " CC=\"" << m_iChildrenCount << "\"" << endl;

   oColFile << Config::CollectionDefinitionKey 
	        << " Session=\"" << put_time(&otm, "%F_%T") << "\"" << endl;

   vector<string>::iterator iter_Line = lstLines.begin();
   for (; iter_Line != lstLines.end(); ++iter_Line)
   {
      oColFile << *iter_Line << endl;
   }

   oColFile.close();
}
