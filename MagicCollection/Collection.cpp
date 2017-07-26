#include "Collection.h"


// An ID will be given to the collection if there is a parent.
Collection::Collection(std::string aszName, CollectionSource* aoSource, std::string aszFileCollection, std::string aszID)
{
	m_szName = aszName;
	m_szFileName = aszFileCollection;
	m_ptrCollectionSource = aoSource;
	m_iChildrenCount = 0;

	setID(aszID);

	m_bRecordChanges = true;
}


Collection::~Collection()
{
}

void Collection::SetName(std::string aszNewName)
{
	m_szName = aszNewName;
}

std::string Collection::GetName()
{
	return m_szName;
}

std::string Collection::GetIdentifier()
{
	return m_szID;
}

void Collection::AddItem(std::string aszName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags,
	bool abCloseTransaction)
{
	// Verify the card name entered is valid
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem == -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);

	// This is needed for removal
	std::string szHash = item->GetHash(GetIdentifier(), alstAttrs, alstMetaTags);

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::addItem, this, aszName, alstAttrs, alstMetaTags);

	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::removeItem, this, aszName, szHash, GetIdentifier());

	Action action(fnDo, fnUndo);

	// Use a generated copy to ensure that default values are set.
	CopyItem* newCopy = item->GenerateCopy(GetIdentifier(), alstAttrs, alstMetaTags);
	std::string szIdentifier = "+ " + CollectionItem::ToCardLine(GetIdentifier(), aszName, newCopy->GetIdentifyingAttributes(), newCopy->GetMetaTags(MetaTagType::Visible), GetIdentifier());
	delete newCopy;
	action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void Collection::AddItem(std::string aszName, std::string aszHash, std::string aszResidentIn, bool abCloseTransaction)
{
	// Verify the card name entered is valid
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem == -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);

	// This is needed for removal
	CopyItem* cItem = item->FindCopyItem(aszHash);
	if (cItem == nullptr) { return; }

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::addExistingItem, this, aszName, aszHash, aszResidentIn);

	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::removeItem, this, aszName, aszHash, aszResidentIn);

	Action action(fnDo, fnUndo);

	// Use a generated copy to ensure that default values are set.
	std::string szIdentifier = "+ " + CollectionItem::ToCardLine(GetIdentifier(), aszName, cItem->GetIdentifyingAttributes(), cItem->GetMetaTags(MetaTagType::Visible), GetIdentifier());
	action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void Collection::RemoveItem(std::string aszName, std::string aszIdentifyingHash, bool abCloseTransaction)
{
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem == -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
	CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
	if (copy == nullptr) { return; }

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::removeItem, this, aszName, aszIdentifyingHash, GetIdentifier());

	std::vector<Tag> lstAttrs = copy->GetIdentifyingAttributes();
	std::vector<Tag> lstMetas = copy->GetMetaTags(Visible);
	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::addItem, this, aszName, lstAttrs, lstMetas);

	Action action(fnDo, fnUndo);

	std::string szIdentifier = "- " + CollectionItem::ToCardLine(GetIdentifier(), aszName, lstAttrs, lstMetas, GetIdentifier());
	action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void Collection::ChangeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction)
{
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem == -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
	CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
	if (copy == nullptr) { return; }

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::changeItem, this, aszName, aszIdentifyingHash, alstChanges, alstMetaChanges);

	// Simulate the change so we can determine the hash and the exact traits when change is complete.
	CopyItem* falseCopy = new CopyItem(*copy);
	modifyItem(falseCopy, alstChanges, alstMetaChanges);
	std::vector<Tag> lstFutureIds = falseCopy->GetIdentifyingAttributes();
	std::vector<Tag> lstFutureMeta = falseCopy->GetMetaTags(MetaTagType::Visible);
	delete falseCopy;

	std::vector<Tag> lstOldIds = copy->GetIdentifyingAttributes();
	std::vector<Tag> lstOldMeta = copy->GetMetaTags(MetaTagType::Visible);

	// This is the hash that the itme will have after the properties are changed. So we need this to undo this change.
	std::string szPostHash = item->GetHash(GetIdentifier(), lstFutureIds, lstFutureMeta);
	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::changeItem, this, aszName, szPostHash, lstOldIds, lstOldMeta);

	Action action(fnDo, fnUndo);

	std::string szIdentifier = "% " + CollectionItem::ToCardLine(GetIdentifier(), aszName, lstOldIds, lstOldMeta, GetIdentifier()) + "->";
	szIdentifier += CollectionItem::ToCardLine(GetIdentifier(), aszName, lstFutureIds, lstFutureMeta, GetIdentifier());
	action.SetIdentifier(szIdentifier);

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void Collection::ReplaceItem(std::string aszName, std::string aszIdentifyingHash, std::string aszNewName, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction)
{
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	int iValidNewItem = m_ptrCollectionSource->LoadCard(aszNewName);
	if (iValidItem == -1 || iValidNewItem == -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
	CollectionItem* newItem = m_ptrCollectionSource->GetCardPrototype(iValidNewItem);
	CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
	if (copy == nullptr) { return; }

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::replaceItem, this, aszName, aszIdentifyingHash, aszNewName, alstIdChanges, alstMetaChanges);

	std::vector<Tag> lstOldIds = copy->GetIdentifyingAttributes();
	std::vector<Tag> lstOldMeta = copy->GetMetaTags(MetaTagType::Visible);

	// Generate a temp copy to see what the actual result is.
	CopyItem* falseCopy = newItem->GenerateCopy(GetIdentifier(), alstIdChanges, alstMetaChanges);
	std::vector<Tag> lstFutureIds = falseCopy->GetIdentifyingAttributes();
	std::vector<Tag> lstFutureMeta = falseCopy->GetMetaTags(MetaTagType::Visible);
	delete falseCopy;

	// This is the hash that the itme will have after the properties are changed. So we need this to undo this change.
	std::string szPostHash = newItem->GetHash(GetIdentifier(), alstIdChanges, alstMetaChanges);
	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::replaceItem, this, aszNewName, szPostHash, aszName, lstOldIds, lstOldMeta);

	Action action(fnDo, fnUndo);

	std::string szIdentifier = "% " + CollectionItem::ToCardLine(GetIdentifier(), aszName, lstOldIds, lstOldMeta, GetIdentifier()) + "->";
	szIdentifier += CollectionItem::ToCardLine(GetIdentifier(), aszNewName, alstIdChanges, alstMetaChanges, GetIdentifier());
	action.SetIdentifier(szIdentifier);

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

std::vector<std::string> Collection::GetMetaData()
{
	std::vector<std::string> lstRetval;
	lstRetval.push_back("Name=\"" + m_szName + "\"");

	lstRetval.push_back("ID=\"" + m_szID + "\"");

	for each(auto metaData in m_lstTaggedItems)
	{
		lstRetval.push_back(metaData.first + ": {" + metaData.second.first + "=\"" + metaData.second.second + "\" }");
	}

	return lstRetval;
}

std::vector<std::pair<std::string, Tag>> Collection::GetTags()
{
	return m_lstTaggedItems;
}

void Collection::TagItem(std::string aszHash, Tag atag)
{
	std::pair<std::string, Tag> pairItem = std::make_pair(aszHash, atag);
	m_lstTaggedItems.push_back(pairItem);
}

void Collection::UntagItem(std::string aszHash, std::string aszTagKey)
{
	std::function<std::string(std::pair<std::string, Tag>)> fnExtractor = [aszTagKey](std::pair<std::string, Tag> itemTag)->std::string { return itemTag.first; };

	int iFound = ListHelper::List_Find(aszHash, m_lstTaggedItems, fnExtractor);
	if (iFound != -1)
	{
		m_lstTaggedItems.erase(m_lstTaggedItems.begin() + iFound);
	}
}

void  Collection::ChildAdded()
{
	m_iChildrenCount++;
}

int  Collection::ChildCount()
{
	return m_iChildrenCount;
}

void  Collection::SaveCollection()
{
	saveHistory();

	saveMeta();

	saveCollection();
}

void Collection::LoadCollection(std::string aszFileName, CollectionFactory* aoFactory)
{
	std::vector<std::string> lstPreprocessLines;
	std::vector<std::string> lstCardLines;
	std::map<int, std::list<CopyItem*>> mapNewlyAddedItems;
	std::map<int, std::list<CopyItem*>> mapExistingItems;
	std::vector<std::string> lstFileLines;
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

	loader.ConsolodateLocalItems(GetIdentifier(),
		m_ptrCollectionSource,
		mapNewlyAddedItems,
		mapExistingItems);

	// Now Verify Borrowed Cards (i.e. Parent != this) that were just loaded exist.
	// Two things can happen in this case. 
	// If the claimed collection exists, then try to find the referenced item and use that instead, if that fails, delete the item.
	// If the claimed collection does not exist, then try to find an identical copy that may have been created by another collection and use that. If that fails, use the one created.
	loader.ConsolodateBorrowedItems(GetIdentifier(),
		m_ptrCollectionSource,
		aoFactory);

	// Now this collection is COMPLETELY LOADED. Since other collections can reference this collection, without this collection being loaded,
	// those other collections may have created copies of card in this collection already; if that is the case, use those copies. Additionally,
	// check that all the copies referenced by the other collections still exist, if not, delete those copies.
	loader.ReleaseUnfoundReferences(GetIdentifier(),
		m_ptrCollectionSource);

	m_bRecordChanges = true;
	IsLoaded = (GetIdentifier() != Config::NotFoundString);
}

// Returns all the copies impacted by this function.
void Collection::LoadChanges(std::vector<std::string> lstLines)
{
	std::vector<std::string>::iterator iter_Lines = lstLines.begin();
	for (; iter_Lines != lstLines.end(); ++iter_Lines)
	{
		loadInterfaceLine(*iter_Lines);
	}
}

std::vector<std::string> Collection::GetCollectionList(MetaTagType atagType, bool aiCollapsed)
{
	std::function<std::string(std::pair<std::string, int>)> fnExtractor = [](std::pair<std::string, int> pVal)->std::string { return pVal.first; };
	std::vector<std::string> lstRetVal;
	std::vector<std::pair<std::string, int>> lstSeenHashes;
	std::vector<int> lstCol = getCollection();
	std::vector<int>::iterator iter_Items = lstCol.begin();
	for (; iter_Items != lstCol.end(); ++iter_Items)
	{
		CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(*iter_Items);
		std::vector<CopyItem*> lstCopies = item->GetCopiesForCollection(GetIdentifier(), All);

		std::vector<CopyItem*>::iterator iter_Copy = lstCopies.begin();
		for (; iter_Copy != lstCopies.end(); ++iter_Copy)
		{
			std::string szHash = (*iter_Copy)->GetHash();
			int iCounted = ListHelper::List_Find(szHash, lstSeenHashes, fnExtractor);
			if (!aiCollapsed || (iCounted == -1))
			{
				std::string szRep = item->GetCardString(*iter_Copy, atagType, GetIdentifier());
				lstRetVal.push_back(szRep);
				lstSeenHashes.push_back(std::make_pair(szHash, 1));
			}
			else if (iCounted != -1)
			{
				lstSeenHashes[iCounted].second++;
			}
		}
	}

	if (aiCollapsed)
	{
		std::vector<std::string> lstNewRetVal;
		for (size_t i = 0; i < lstRetVal.size(); i++)
		{
			int iCounted = lstSeenHashes[i].second;
			lstNewRetVal.push_back("x" + std::to_string(iCounted) + " " + lstRetVal[i]);
		}

		lstRetVal.clear();
		lstRetVal = lstNewRetVal;
	}

	return lstRetVal;
}

void Collection::setID(std::string aszIDString)
{
	if (aszIDString == "")
	{
		m_szID = std::to_string(std::rand() % 1000000); // 1,000,000 - 6 digits
	}
	else
	{
		m_szID = aszIDString;
	}
}

std::vector<int> Collection::getCollection()
{
	if (m_ptrCollectionSource->IsSyncNeeded(GetIdentifier()))
	{
		m_lstItemCacheIndexes = m_ptrCollectionSource->GetCollectionCache(GetIdentifier());
	}
	return m_lstItemCacheIndexes;
}

void Collection::addItem(std::string aszName, std::vector<Tag> alstAttrs, std::vector<Tag> alstMetaTags)
{
	int iCache = m_ptrCollectionSource->LoadCard(aszName);

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);

	// If this is a super collection, it will just add the item.
	// If this is a child collection, it will add this to its resident and set the parent to this' parent.
	CopyItem* cItem = item->AddCopyItem(m_szID, alstAttrs, alstMetaTags);
	std::string szHash = cItem->GetHash();

	registerItem(iCache);

	// Notify other collections they may need to sync since this may have been borrowed by other collections.
	// There should never be other copies with that hash not in resident. They are removed at load time.
	m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void Collection::addExistingItem(std::string aszName, std::string aszHash, std::string aszResidentIn)
{

}

void Collection::removeItem(std::string aszName, std::string aszIdentifyingHash, std::string aszResidentIn)
{
	// The copy is already verified to exist at this point
	int iCache = m_ptrCollectionSource->LoadCard(aszName);

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
	std::string szHostToRemoveFrom = GetIdentifier();
	if (aszResidentIn != "")
	{
		szHostToRemoveFrom = aszResidentIn;
	}

	item->RemoveCopyItem(aszResidentIn, aszIdentifyingHash);

	// Remove any items from the cache that are no longer in this collection.
	if (item->GetCopiesForCollection(GetIdentifier(), All).size() == 0)
	{
		std::vector<int> lstNewCacheIndexes;
		for (size_t i = 0; i < m_lstItemCacheIndexes.size(); i++)
		{
			if (m_lstItemCacheIndexes[i] != iCache)
			{
				lstNewCacheIndexes.push_back(m_lstItemCacheIndexes[i]);
			}
		}
		m_lstItemCacheIndexes = lstNewCacheIndexes;
	}

	// Notify other collections they may need to sync since this may have been borrowed by other collections.
	m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}

void Collection::changeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges, std::vector<Tag> alstMetaChanges)
{
	int iCache = m_ptrCollectionSource->LoadCard(aszName);

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
	CopyItem* cItem = item->FindCopyItem(aszIdentifyingHash);
	if (cItem == nullptr) { return; }

	modifyItem(cItem, alstChanges, alstMetaChanges);

	// Notify other collections they may need to sync since this may have been borrowed by other collections.
	m_ptrCollectionSource->NotifyNeedToSync(GetIdentifier());
}


void Collection::replaceItem(std::string aszName, std::string aszIdentifyingHash, std::string aszNewName, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges)
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

void Collection::modifyItem(CopyItem* aptCopy, std::vector<Tag> alstChanges, std::vector<Tag> alstMetaChanges)
{
	for (size_t i = 0; i < alstChanges.size(); i++)
	{
		aptCopy->SetIdentifyingAttribute(alstChanges[i].first, alstChanges[i].second);
	}

	MetaTagType mtt = MetaTagType::Public;
	for (size_t i = 0; i < alstMetaChanges.size(); i++)
	{
		if (alstMetaChanges[i].first.size() > 0 && alstMetaChanges[i].first[0] == '_')
		{
			mtt == MetaTagType::Ignored;
		}
		aptCopy->SetMetaTag(alstMetaChanges[i].first, alstMetaChanges[i].second, mtt);
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
	std::string szFileName = ioHelper.GetMetaFile(m_szFileName);
	std::vector<std::string> lstMetaLines = ioHelper.GetFileLines(szFileName);

	for (size_t i = 0; i < lstMetaLines.size(); i++)
	{
		CollectionItem::PseudoIdentifier sudoItem;
		CollectionItem::ParseCardLine(lstMetaLines[i], sudoItem);

		std::vector<Tag> lstMetaTags = sudoItem.MetaTags;

		// Clear the meta so the hash may be obtained.
		sudoItem.MetaString = "";
		sudoItem.MetaTags.clear();

		int iRealCard = m_ptrCollectionSource->LoadCard(sudoItem.Name);
		if (iRealCard == -1) { continue; }

		CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iRealCard);
		std::string szPlainHash = item->GetHash(GetIdentifier(), sudoItem.Identifiers);

		// Gets the first matching item resident in this collection.
		CopyItem* matchingCopy = item->FindCopyItem(szPlainHash, GetIdentifier());
		if (matchingCopy != nullptr)
		{
			for (size_t t = 0; t < lstMetaTags.size(); t++)
			{
				matchingCopy->SetMetaTag(lstMetaTags[t].first, lstMetaTags[t].second, MetaTagType::Public);
			}
		}

	}
}

void Collection::loadPreprocessingLines(std::vector<std::string>  alstLines)
{
	std::vector<std::string>::iterator iter_Lines = alstLines.begin();
	for (; iter_Lines != alstLines.end(); ++iter_Lines)
	{
		loadPreprocessingLine(*iter_Lines);
	}
}

void Collection::loadPreprocessingLine(std::string aszLine)
{
	std::string szDefKey(Config::CollectionDefinitionKey);
	if (aszLine.size() < 2) { return; }
	if (aszLine.substr(0, szDefKey.size()) != szDefKey) { return; }

	std::string szBaseLine = aszLine.substr(2);
	std::vector<std::string> lstSplitLine = StringHelper::Str_Split(szBaseLine, "=");

	if (lstSplitLine.size() != 2) { return; }

	std::vector<std::string>::iterator iter_Lines = lstSplitLine.begin();
	for (; iter_Lines != lstSplitLine.end(); ++iter_Lines)
	{
		*iter_Lines = StringHelper::Str_Trim(*iter_Lines, ' ');
	}

	std::string szKey = lstSplitLine.at(0);
	std::string szValue = lstSplitLine.at(1);
	szValue = StringHelper::Str_Trim(szValue, '\"');

	if (szKey == "Name")
	{
		m_szName = szValue;
	}
	else if (szKey == "ID")
	{
		setID(szValue);
	}
	else if (szKey == "CC")
	{
		m_iChildrenCount = std::stoi(szValue);
	}
}

// May return null depending on input
void Collection::loadInterfaceLine(std::string aszLine)
{
	if (aszLine.size() <= 2) { return; }

	std::string szTrimmedLine = StringHelper::Str_Trim(aszLine, ' ');

	std::string szLoadDirective = szTrimmedLine.substr(0, 1);

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

void Collection::loadAdditionLine(std::string aszLine)
{
	CollectionItem::PseudoIdentifier sudoItem;
	CollectionItem::ParseCardLine(aszLine, sudoItem);

	for (size_t i = 0; i < sudoItem.Count; i++)
	{
		AddItem(sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags);
	}
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
void Collection::loadRemoveLine(std::string aszLine)
{
	CollectionItem::PseudoIdentifier sudoItem;
	CollectionItem::ParseCardLine(aszLine, sudoItem);

	for (size_t i = 0; i < sudoItem.Count; i++)
	{
		std::string szHash;
		int iHash = ListHelper::List_Find(std::string(Config::HashKey), sudoItem.MetaTags, Config::Instance()->GetTagHelper());
		if (iHash != -1)
		{
			szHash = sudoItem.MetaTags[iHash].second;
			RemoveItem(sudoItem.Name, szHash);
		}
		else { break; }
	}
}

// Sylvan Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }] ->
//   Another Card Name [{ set="val" color="val2" } ][: { metatag1="val" metatag2="val2" }]
void Collection::loadDeltaLine(std::string aszLine)
{
	std::vector<std::string> lstOldNew = StringHelper::Str_Split(aszLine, "->");

	CollectionItem::PseudoIdentifier sudoOldItem;
	CollectionItem::ParseCardLine(lstOldNew[0], sudoOldItem);

	CollectionItem::PseudoIdentifier sudoNewItem;
	CollectionItem::ParseCardLine(lstOldNew[1], sudoNewItem);

	std::string szHash;
	int iCache;
	int iHash = ListHelper::List_Find(std::string(Config::HashKey), sudoOldItem.MetaTags, Config::Instance()->GetTagHelper());
	if (iHash != -1 && (iCache = m_ptrCollectionSource->LoadCard(sudoOldItem.Name)) != -1)
	{
		std::string szHash = sudoOldItem.MetaTags[iHash].second;
		CollectionItem* itemOld = m_ptrCollectionSource->GetCardPrototype(iCache);
		CopyItem* cItem = itemOld->FindCopyItem(szHash);

		for (size_t i = 0; i < sudoOldItem.Count; i++)
		{
			int iNewCache;
			if (sudoOldItem.Name == sudoNewItem.Name)
			{
				ChangeItem(sudoOldItem.Name, szHash, sudoNewItem.Identifiers, sudoNewItem.MetaTags);
			}
			else if ((iNewCache = m_ptrCollectionSource->LoadCard(sudoNewItem.Name)) != -1)
			{
				CollectionItem* itemNew = m_ptrCollectionSource->GetCardPrototype(iNewCache);
				ReplaceItem(sudoOldItem.Name, szHash, sudoNewItem.Name, sudoNewItem.Identifiers, sudoNewItem.MetaTags);
			}
		}

	}

}

void Collection::saveHistory()
{
	std::vector<std::string> lstHistoryLines;
	for (size_t i = 0; i < m_lstTransactions.size(); i++)
	{
		std::vector<std::string> lstTransLines = m_lstTransactions[i].GetDescriptions();
		for (size_t t = 0; t < lstTransLines.size(); t++)
		{
			lstHistoryLines.push_back(lstTransLines[t]);
		}
	}

	if (lstHistoryLines.size() > 0)
	{
		std::string szTimeString = "";
		time_t now = time(0);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char str[26];
		asctime_s(str, sizeof str, &timeinfo);
		str[strlen(str) - 1] = 0;

		CollectionIO ioHelper;
		std::ofstream oHistFile;
		std::string szHistFile = ioHelper.GetHistoryFile(m_szFileName);
		oHistFile.open(szHistFile, std::ios_base::app);

		oHistFile << "[" << str << "] " << std::endl;

		std::vector<std::string>::iterator iter_histLines = lstHistoryLines.begin();
		for (; iter_histLines != lstHistoryLines.end(); ++iter_histLines)
		{
			oHistFile << *iter_histLines << std::endl;
		}

		oHistFile.close();
	}
}

void Collection::saveMeta()
{
	std::vector<std::string> lstMetaLines = GetCollectionList(Visible);

	CollectionIO ioHelper;
	std::ofstream oMetaFile;
	oMetaFile.open(ioHelper.GetMetaFile(m_szFileName));

	std::vector<std::string>::iterator iter_MetaLine = lstMetaLines.begin();
	for (; iter_MetaLine != lstMetaLines.end(); ++iter_MetaLine)
	{
		if (iter_MetaLine->find_first_of(':') != std::string::npos)
		{
			oMetaFile << *iter_MetaLine << std::endl;
		}
	}

	oMetaFile.close();
}

void Collection::saveCollection()
{
	std::vector<std::string> lstLines = GetCollectionList(None);

	CollectionIO ioHelper;
	std::ofstream oColFile;
	oColFile.open(ioHelper.GetCollectionFile(m_szFileName));

	oColFile << Config::CollectionDefinitionKey << " Name=\"" << m_szName << "\"" << std::endl;

	oColFile << Config::CollectionDefinitionKey << " ID=\"" << m_szID << "\"" << std::endl;

	oColFile << Config::CollectionDefinitionKey << " CC=\"" << m_iChildrenCount << "\"" << std::endl;

	std::vector<std::string>::iterator iter_Line = lstLines.begin();
	for (; iter_Line != lstLines.end(); ++iter_Line)
	{
		oColFile << *iter_Line << std::endl;
	}

	oColFile.close();
}
