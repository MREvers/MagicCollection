#pragma once
#include <vector>
#include <string>

#include "CollectionIO.h"
#include "CollectionSource.h"
#include "CollectionItem.h"
#include "StringHelper.h"
#include "Transaction.h"
#include "CollectionFactory.h"

class CollectionFactory;

class Collection
{
public:
	Collection(std::string aszName,
				   CollectionSource* aoSource,
				   std::string aszFileCollection,
				   std::string aszParentCollectionName = "");
	~Collection();

	std::string GetName();

	void AddItem(std::string aszName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>(),
		bool abCloseTransaction = true);
	void RemoveItem(std::string aszName, std::string aszIdentifyingHash, bool abCloseTransaction = true);
	void ChangeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction = true);
	void ReplaceItem(std::string aszName, std::string aszIdentifyingHash, std::string aszNewName, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction = true);

	void SaveCollection();
	void LoadCollection(std::string aszFileName, CollectionFactory* aoFactory);
	void LoadChanges(std::vector<std::string> aszLines);

	std::vector<std::string> GetCollectionList(MetaTagType atagType = Visible, bool aiCollapsed = false);

	std::vector<std::string> GetCollectionAnalysis(std::string aszAnalysisCmd);

	bool IsLoaded = false;

private:
	std::string m_szName;
	std::string m_szParentName;
	std::string m_szFileName;
	CollectionSource* m_ptrCollectionSource;

	bool m_bRecordChanges;
	std::vector<Transaction> m_lstTransactions;

	// Contains information such as "Sideboard" or if the card is an "Icon" of the collection.
	std::vector<std::pair<std::string, Tag>> m_lstTaggedItems;

	std::vector<int> m_lstItemCacheIndexes;

	std::vector<int> getCollection();

	// These all locate by name and hash for a second time so we dont risk dangling pointers.
	void addItem(std::string aszName, std::vector<Tag> alstAttrs, std::vector<Tag> alstMetaTags);
	void removeItem(std::string aszName, std::string aszIdentifyingHash);
	void changeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges, std::vector<Tag> alstMetaChanges);
	void replaceItem(std::string aszName, std::string aszIdentifyingHash, std::string aszNewName, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges);
	void registerItem(int aiCacheIndex);

	void modifyItem(CopyItem* aptCopy, std::vector<Tag> alstChanges, std::vector<Tag> alstMetaChanges);

	Transaction* getOpenTransaction();
	void finalizeTransaction();

	void loadMetaTagFile();

	void loadPreprocessingLines(std::vector<std::string> alstLine);
	void loadPreprocessingLine(std::string aszLine);

	void loadInterfaceLine(std::string aszLine);

	void loadAdditionLine(std::string aszLine);
	void loadRemoveLine(std::string aszLine);
	void loadDeltaLine(std::string aszLine);

	void saveHistory();
	void saveMeta();
	void saveCollection();

	// Specific to MtG. This type of stuff can be abstracted out but I will leave it here for now.
	std::string analyseMana();
};

