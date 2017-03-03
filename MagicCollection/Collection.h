#pragma once
#pragma message ("Starting Collection.h")

#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <ctime>

#include "ICollection.h"
#include "ICollectionObject.h"
#include "CollectionObject.h"
// Just use magic card object for now
#include "MagicCardObject.h"
#include "CollectionSource.h"


class Collection : public ICollection
{
	struct CardTags
	{
		std::vector<std::pair<std::string, std::string>> Attrs; //Attributes
		std::vector<std::pair<std::string, std::string>> Tags; //Metatags
	};

	class Action
	{
	public:
		std::string Identifier;
		std::function<void()> Do;
		std::function<void()> Undo;
	};

	class Transaction
	{
	public:
		Transaction(Collection* aoCol);
		~Transaction();

		Collection* operator-> ();
		void AddAction(Action& aoAct);
		void RemoveAction(int i);
		void Finalize(bool abRecordable = true);
		void Rollback();

		bool IsOpen = true;
		bool Recordable;
		std::vector<Action> Actions;

	private:
		Collection* m_Col;
	};
public:
	Collection(std::string aszName, CollectionSource* aoSource, std::vector<std::string>* alstLoadedCollections);
	~Collection();

	// Meta
	std::string GetName();

	void AddItem(std::string aszNewItem,
		bool bFinal = true,
		std::vector<std::pair<std::string, std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>(),
		std::vector<std::pair<std::string, std::string>> alstMetaTags = std::vector<std::pair<std::string, std::string>>()) override;
	void AddItem(std::string aszNewItemLongName,
		std::vector<std::pair<std::string, std::string>> alstMetaTags);

	void RemoveItem(std::string aszNewItem,
		bool bFinal = true,
		std::vector<std::pair<std::string, std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>(),
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>()) override;
	void RemoveItem(std::string aszRemoveItemLongName, std::vector<std::pair<std::string, std::string>> alstMeta);

	void RemoveMetaTag(std::string aszLongName, std::string aszKey,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
	void AddMetaTag(std::string aszLongName, std::string aszKey, std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
	void AddMetaTags(std::string aszLongName, std::vector<std::pair<std::string,std::string>> alstKeyVals,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());

	void SetNonUniqueAttribute(std::string aszLongName, std::string aszKey, std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
	void SetNonUniqueAttributes(std::string aszLongName, std::vector<std::pair<std::string, std::string>> alstKeyVals,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
	// Returns the list of restions if restrictions exist. * if none exist.
	std::vector<std::string> GetNonUniqueAttributeRestrictions(std::string aszLongName, std::string aszKey);

	std::vector < std::vector<std::pair<std::string, std::string>>> GetMetaTags(std::string aszLongName);
	void RollbackTransaction();

	void LoadCollection(std::string aszCollectionFile, std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges);
	void SaveCollection(std::string aszCollectionFileName);
	// Clears the history file, then writes the baseline.
	void CreateBaselineHistory();
	void RecordForcedTransaction(std::string aszTransactionString);
	std::vector<std::string> GetCollectionList();
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
		GetCollectionListWithMeta();

	static std::vector<std::pair<std::string, std::string>> ParseAttrs(std::string aszAttrs);
	static bool ParseCardLine(std::string aszLine, int& riCount, std::string& rszName, std::string& rszDetails);


	void PrintList();

	bool TransactionIntercept;

private:
	CollectionSource* m_ColSource;

	// Collections just know that they have AT LEAST a copy of a card. It has to access
	//  the Col object to see how many copies.
	std::vector<int> m_lstCollection;
	std::string m_szName;
	std::vector<Transaction> m_lstTransactions;
	std::vector<std::string> m_lstUnreversibleChanges;
	std::string m_szHistoryFileName;
	std::string m_szMetaTagFileName;
	std::vector<std::string>* m_lstLoadedCollectionsBuffer;

	void setName(std::string aszName);

	void addItem(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	CopyObject* forceAdd(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs);
	// Only adds the collection object cache locations
	void registerItem(int aiItem);
	void removeItem(
		std::string aszItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	void changeItemAttribute(std::string aszCardname, CopyObject* aoCO, std::string aszKey, std::string aszNewVal, bool bFinal = true);
	std::string changeItemAttribute_string(std::string aszCardname, CopyObject* aoCO, std::string aszKey, std::string aszNewVal, bool bIsParentCol = true);
	void changeItemAttrs(CopyObject* aoCO, std::string aszKey, std::string aszNewVal);

	Transaction* openTransaction();
	void finalizeTransaction(bool abRecord = true);


	std::string cardToString(int aiCardFlyweight, std::pair<CopyObject*, int>* aoCopy, bool bFullDets = false);
	std::vector<std::string> splitIntoLines(std::string aszString);
};

#pragma message ("Finish Collection.h")