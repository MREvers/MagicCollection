#pragma once
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <stdlib.h>

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
				   std::string aszID = "");
	~Collection();

	void SetName(std::string aszNewName);
	std::string GetName();
	std::string GetIdentifier();

	void AddItem(std::string aszName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>(),
		bool abCloseTransaction = true);
	void AddItem(std::string aszName, std::string aszHash, std::string aszResidentIn = "", bool abCloseTransaction = true);
	void RemoveItem(std::string aszName, std::string aszIdentifyingHash, bool abCloseTransaction = true);
	void ChangeItem(std::string aszName, std::string aszIdentifyingHash, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction = true);
	void ReplaceItem(std::string aszName, std::string aszIdentifyingHash, std::string aszNewName, std::vector<Tag> alstIdChanges, std::vector<Tag> alstMetaChanges, bool abCloseTransaction = true);

	std::vector<std::string> GetMetaData();

	std::vector<std::pair<std::string, Tag>> GetTags();
	void TagItem(std::string aszHash, Tag atag);
	void UntagItem(std::string aszHash, std::string aszTagKey);

	// Used to track the number of children in of this collection to avoid name clashes.
	void ChildAdded();
	int ChildCount();

	void SaveCollection();
	void LoadCollection(std::string aszFileName, CollectionFactory* aoFactory);
	void LoadChanges(std::vector<std::string> aszLines);

	std::vector<std::string> GetCollectionList(MetaTagType atagType = Visible, bool aiCollapsed = false);

	bool IsLoaded = false;

private:
	std::string m_szName;
	unsigned int m_iChildrenCount; // Needed to avoid name clashes.
	std::string m_szID;
	std::string m_szFileName;
	CollectionSource* m_ptrCollectionSource;

	void setID(std::string aszIDString);

	bool m_bRecordChanges;
	std::vector<Transaction> m_lstTransactions;

	// Contains information such as "Sideboard" or if the card is an "Icon" of the collection.
	// Hash, Tag
	std::vector<std::pair<std::string, Tag>> m_lstTaggedItems;

	std::vector<int> m_lstItemCacheIndexes;

	std::vector<int> getCollection();

	// These all locate by name and hash for a second time so we dont risk dangling pointers.
	void addItem(std::string aszName, std::vector<Tag> alstAttrs, std::vector<Tag> alstMetaTags);
	void addExistingItem(std::string aszName, std::string aszHash, std::string aszResidentIn); // this is for an Item from another collection
	void removeItem(std::string aszName, std::string aszIdentifyingHash,std::string aszResidentIn);
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
};

