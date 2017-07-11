#include "Collection.h"



Collection::Collection(std::string aszName, CollectionSource* aoSource, std::string aszFileCollection, std::string aszParentCollectionName)
{
	m_szName = aszName;
	m_szFileName = aszFileCollection;
	m_ptrCollectionSource = aoSource;
	m_szParentName = aszParentCollectionName;
	m_bRecordChanges = true;
}


Collection::~Collection()
{
}

std::string Collection::GetName()
{
	return m_szName;
}

void Collection::AddItem(std::string aszName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags,
	bool abCloseTransaction)
{
	// Verify the card name entered is valid
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem != -1)
	{
		CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);

		// This is needed for removal
		std::string szHash = item->GetHash(m_szName, alstAttrs, alstMetaTags);

		std::function<void()> fnDo;
		fnDo = std::bind(&Collection::addItem, this, aszName, alstAttrs, alstMetaTags);

		std::function<void()> fnUndo;
		fnUndo = std::bind(&Collection::removeItem, this, aszName, szHash);

		Action action(fnDo, fnUndo);

		std::string szIdentifier = "+ " + CollectionItem::ToCardLine(m_szName, aszName, alstAttrs, alstMetaTags);
		action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

		Transaction* transaction = getOpenTransaction();
		transaction->AddAction(action);

		if (abCloseTransaction)
		{
			finalizeTransaction();
		}

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
	fnDo = std::bind(&Collection::removeItem, this, aszName, aszIdentifyingHash);

	std::vector<Tag> lstAttrs = copy->GetIdentifyingAttributes();
	std::vector<Tag> lstMetas = copy->GetMetaTags(Visible);
	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::addItem, this, aszName, lstAttrs, lstMetas);

	Action action(fnDo, fnUndo);

	std::string szIdentifier = "- " + CollectionItem::ToCardLine(m_szName, aszName, lstAttrs, lstMetas);
	action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void Collection::ChangeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges, bool abCloseTransaction)
{
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem != -1) { return; }

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iValidItem);
	CopyItem* copy = item->FindCopyItem(aszIdentifyingHash);
	if (copy == nullptr) { return; }

	std::function<void()> fnDo;
	fnDo = std::bind(&Collection::changeItem, this, aszName, aszIdentifyingHash, alstChanges);

	std::vector<Tag> lstUndoChanges;
	std::vector<Tag>::iterator iter_Changes = alstChanges.begin();
	for (; iter_Changes != alstChanges.end(); ++iter_Changes)
	{
		std::string szVal = copy->GetIdentifyingAttribute(iter_Changes->first);
		if (szVal != Config::NotFoundString)
		{
			lstUndoChanges.push_back(std::make_pair(iter_Changes->first, szVal));
			continue;
		}

		szVal = copy->GetMetaTag(iter_Changes->first, MetaTagType::Any);
		if (szVal != Config::NotFoundString)
		{
			lstUndoChanges.push_back(std::make_pair(iter_Changes->first, szVal));
		}
	}

	std::function<void()> fnUndo;
	fnUndo = std::bind(&Collection::changeItem, this, aszName, aszIdentifyingHash, lstUndoChanges);

	Action action(fnDo, fnUndo);

	std::string szIdentifier = "% " + CollectionItem::ToCardLine(m_szName, aszName, alstChanges);
	action.SetIdentifier(szIdentifier); //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;

	Transaction* transaction = getOpenTransaction();
	transaction->AddAction(action);

	if (abCloseTransaction)
	{
		finalizeTransaction();
	}
}

void  Collection::SaveCollection()
{
	saveHistory();
	
	saveMeta();

	saveCollection();
}

void Collection::LoadCollection(std::string aszFileName)
{
	CollectionIO loader;
	m_bRecordChanges = false;

	std::vector<std::string> lstLines = loader.GetFileLines(aszFileName);

	std::vector<std::string> lstPreprocessLines;
	std::vector<std::string> lstCardLines = loader.GetPreprocessLines(lstLines, lstPreprocessLines);

	loadPreprocessingLines(lstPreprocessLines);

	LoadChanges(lstCardLines);

	// Also need to load metatags...
	loadMetaTagFile();

	m_bRecordChanges = true;
	IsLoaded = (m_szName != Config::NotFoundString);
}

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
	std::vector<std::pair<std::string,int>> lstSeenHashes;
	std::vector<int> lstCol = getCollection();
	std::vector<int>::iterator iter_Items = lstCol.begin();
	for (; iter_Items != lstCol.end(); ++iter_Items)
	{
		CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(*iter_Items);
		std::vector<CopyItem*> lstCopies = item->GetCopiesForCollection(m_szName, All);

		std::vector<CopyItem*>::iterator iter_Copy = lstCopies.begin();
		for (; iter_Copy != lstCopies.end(); ++iter_Copy)
		{
			std::string szHash = (*iter_Copy)->GetHash();
			int iCounted = ListHelper::List_Find(szHash, lstSeenHashes, fnExtractor);
			if (!aiCollapsed || (iCounted == -1))
			{
				std::string szRep = item->GetCardString(*iter_Copy, atagType);
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

std::vector<int> Collection::getCollection()
{
	if (m_ptrCollectionSource->IsSyncNeeded(m_szName))
	{
		m_lstItemCacheIndexes = m_ptrCollectionSource->GetCollectionCache(m_szName, false);
	}
	return m_lstItemCacheIndexes;
}

void Collection::addItem(std::string aszName, std::vector<Tag> alstAttrs, std::vector<Tag> alstMetaTags)
{
	int iCache = m_ptrCollectionSource->LoadCard(aszName);

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
	item->AddCopyItem(m_szName, alstAttrs, alstMetaTags);

	registerItem(iCache);
}

void Collection::removeItem(std::string aszName, std::string aszIdentifyingHash)
{
	int iCache = m_ptrCollectionSource->LoadCard(aszName);

	CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iCache);
	item->RemoveCopyItem(aszIdentifyingHash);

	if (item->GetCopiesForCollection(m_szName, All).size() == 0)
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
}

void Collection::changeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges)
{

}

void Collection::registerItem(int aiCacheIndex)
{
	int iFound = ListHelper::List_Find(aiCacheIndex, m_lstItemCacheIndexes);
	if (iFound == -1)
	{
		m_lstItemCacheIndexes.push_back(iFound);
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
	std::vector<std::string> lstMetaLines = ioHelper.GetFileLines(Config::Instance()->GetMetaFolderName() + "\\" + ioHelper.GetMetaFile(m_szFileName));

	for (size_t i = 0; i < lstMetaLines.size(); i++)
	{
		CollectionItem::PseudoIdentifier sudoItem;
		CollectionItem::ParseCardLine(lstMetaLines[i], sudoItem);

		std::vector<Tag> lstMetaTags = sudoItem.MetaTags;

		// Clear the meta so the hash may be obtained.
		sudoItem.MetaString = "";
		sudoItem.MetaTags.clear();

		int iRealCard = m_ptrCollectionSource->LoadCard(sudoItem.Name);
		if (iRealCard != -1)
		{
			CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(iRealCard);
			std::string szPlainHash = item->GetHash(m_szName, sudoItem.Identifiers);

			CopyItem* matchingCopy = item->FindCopyItem(szPlainHash);
			if (matchingCopy != nullptr)
			{
				for (size_t t = 0; t < lstMetaTags.size(); t++)
				{
					matchingCopy->SetMetaTag(lstMetaTags[i].first, lstMetaTags[i].second, MetaTagType::Public);
				}
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
	else if (szKey == "Parent")
	{
		m_szParentName = szValue;
	}
}

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

	AddItem(sudoItem.Name, sudoItem.Identifiers, sudoItem.MetaTags);
}

// This needs "Card Name : { __hash="hashval" }" All other values are irrelevant.
void Collection::loadRemoveLine(std::string aszLine)
{
	CollectionItem::PseudoIdentifier sudoItem;
	CollectionItem::ParseCardLine(aszLine, sudoItem);

	std::string szHash;
	int iHash = ListHelper::List_Find(std::string(Config::HashKey), sudoItem.MetaTags, Config::Instance()->GetTagHelper());
	if (iHash != -1)
	{
		szHash = sudoItem.MetaTags[iHash].second;
		RemoveItem(sudoItem.Name, szHash);
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
	int iHash = ListHelper::List_Find(std::string(Config::HashKey), sudoOldItem.MetaTags, Config::Instance()->GetTagHelper());
	if (iHash != -1)
	{
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
		oHistFile.open(Config::Instance()->GetHistoryFolderName() + "\\" + ioHelper.GetHistoryFile(m_szFileName), std::ios_base::app);

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

	if (lstMetaLines.size() > 0)
	{
		CollectionIO ioHelper;
		std::ofstream oMetaFile;
		oMetaFile.open(Config::Instance()->GetMetaFolderName() + "\\"+ioHelper.GetMetaFile(m_szFileName));

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
}

void Collection::saveCollection()
{
	std::vector<std::string> lstLines = GetCollectionList(None);

	if (lstLines.size() > 0)
	{
		CollectionIO ioHelper;
		std::ofstream oColFile;
		oColFile.open(ioHelper.GetCollectionFile(m_szFileName));

		oColFile << Config::CollectionDefinitionKey << " Name=\"" << m_szName<< "\"" << std::endl;
		if (m_szParentName != "")
		{
			oColFile << Config::CollectionDefinitionKey << " Parent=\"" << m_szParentName << "\"" << std::endl;
		}

		std::vector<std::string>::iterator iter_Line = lstLines.begin();
		for (; iter_Line != lstLines.end(); ++iter_Line)
		{
			oColFile << *iter_Line << std::endl;
		}

		oColFile.close();
	}
}
