#pragma once
#include <vector>
#include <string>
#include <winapifamily.h>

#include "CollectionIO.h"
#include "CollectionSource.h"
#include "CollectionItem.h"
#include "StringHelper.h"
#include "Transaction.h"

class Collection
{
public:
	Collection(std::string aszName,
				   CollectionSource* aoSource,
				   std::string aszParentCollectionName = "");
	~Collection();

	std::string GetName();

	void AddItem(std::string aszName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>(),
		bool abCloseTransaction = true);
	void RemoveItem(std::string aszName, std::string aszIdentifyingHash, bool abCloseTransaction = true);
	void ChangeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges, bool abCloseTransaction = true);

	void SaveCollection();
	void LoadCollection(std::string aszFileName);
	void LoadChanges(std::vector<std::string> aszLines);

	std::vector<std::string> GetCollectionList(MetaTagType atagType = Visible, bool aiCollapsed = false);

	bool IsLoaded = false;

private:
	enum ChangeType: int
	{
		Addition = 0x0,
		Removal = 0x1,
		Change = 0x2
	};

	std::string m_szName;
	std::string m_szParentName;
	CollectionSource* m_ptrCollectionSource;

	bool m_bRecordChanges;
	std::vector<Transaction> m_lstTransactions;
	std::vector<std::string> m_lstHistory;

	std::vector<int> m_lstItemCacheIndexes;

	std::vector<int> getCollection();

	void addItem(std::string aszName, std::vector<Tag> alstAttrs, std::vector<Tag> alstMetaTags);
	void removeItem(std::string aszName, std::string aszIdentifyingHash);
	void changeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstChanges);
	void registerItem(int aiCacheIndex);

	Transaction* getOpenTransaction();
	void finalizeTransaction();

	void loadPreprocessingLines(std::vector<std::string> alstLine);
	void loadPreprocessingLine(std::string aszLine);

	void loadInterfaceLine(std::string aszLine);

	void loadAdditionLine(std::string aszLine);
	void loadRemoveLine(std::string aszLine);
	void loadDeltaLine(std::string aszLine);

	void saveHistory();
	void saveMeta();
	void saveCollection();
};

