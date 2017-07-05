#include "Collection.h"



Collection::Collection(std::string aszName, CollectionSource* aoSource, std::string aszParentCollectionName)
{
	m_szName = aszName;
	m_ptrCollectionSource = aoSource;
	m_szParentName = aszParentCollectionName;
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
			transaction->Finalize();
		}

	}
}

void Collection::RemoveItem(std::string aszName, std::string aszIdentifyingHash, bool abCloseTransaction)
{
	int iValidItem = m_ptrCollectionSource->LoadCard(aszName);
	if (iValidItem != -1) { return; }

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
		transaction->Finalize();
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
		transaction->Finalize();
	}
}

void Collection::LoadCollection(std::string aszFileName)
{
	CollectionIO loader;

	std::vector<std::string> lstLines = loader.LoadLines(aszFileName);

	std::vector<std::string> lstPreprocessLines;
	std::vector<std::string> lstCardLines = loader.GetPreprocessLines(lstLines, lstPreprocessLines);

	loadPreprocessingLines(lstPreprocessLines);

	LoadChanges(lstCardLines);

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

std::vector<std::string> Collection::GetCollectionList()
{
	std::vector<std::string> lstRetVal;
	std::vector<int> lstCol = getCollection();
	std::vector<int>::iterator iter_Items = lstCol.begin();
	for (; iter_Items != lstCol.end(); ++iter_Items)
	{
		CollectionItem* item = m_ptrCollectionSource->GetCardPrototype(*iter_Items);
		std::vector<CopyItem*> lstCopies = item->GetCopiesForCollection(m_szName, All);

		std::vector<CopyItem*>::iterator iter_Copy = lstCopies.begin();
		for (; iter_Copy != lstCopies.end(); ++iter_Copy)
		{
			std::string szRep = item->GetCardString(*iter_Copy);
			lstRetVal.push_back(szRep);
		}
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

}

void Collection::changeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges)
{

}

void Collection::registerItem(int aiCacheIndex)
{
	int iFound = ListHelper::Instance()->List_Find(aiCacheIndex, m_lstItemCacheIndexes);
	if (iFound == -1)
	{
		m_lstItemCacheIndexes.push_back(iFound);
	}
}

Transaction* Collection::getOpenTransaction()
{
	if (m_lstTransactions.size() == 0 ||
		!m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
	{
		m_lstTransactions.push_back(Transaction(this));
	}

	return &m_lstTransactions.at(m_lstTransactions.size() - 1);
}

void Collection::finalizeTransaction()
{
	Transaction* transaction = getOpenTransaction();
	if (transaction->IsOpen)
	{
		transaction->Finalize();
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
	// Sylvan Card Name[{ set = "val" color = "val2" }][: { metatag1 = "val" metatag2 = "val2" }]
	std::string szIdentifierString;
	std::string szMetaTagString;

	std::vector<std::string> lstSplitLine = StringHelper::Str_Split(aszLine, ":");

	szIdentifierString = lstSplitLine[0];
	if (lstSplitLine.size() > 1)
	{
		szMetaTagString = lstSplitLine[1];
	}

	CollectionItem::PseudoIdentifier sudoItem;
	CollectionItem::ParseCardLine(szIdentifierString, sudoItem);

	std::vector<Tag> lstMetaTags;
	CollectionItem::ParseTagString(szMetaTagString, lstMetaTags);

	std::vector<Tag> lstIdentifiers = sudoItem.Identifiers;

	AddItem(sudoItem.Name, lstIdentifiers, lstMetaTags);
}

void Collection::loadRemoveLine(std::string aszLine)
{

}

void Collection::loadDeltaLine(std::string aszLine)
{

}