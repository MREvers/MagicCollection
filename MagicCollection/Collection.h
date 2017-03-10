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
#include "CollectionSource.h"


class Collection : public ICollection
{
   /* Friend Class Action
    * Used to wrap an action that changes the collection with an action
    * that can undo it. Allows undo button.
    * Also maintains a string description of the action so that it can
    * be recorded in the .history.txt file.
    */
	class Action
	{
	public:
		std::string Identifier;
		std::function<void()> Do;
		std::function<void()> Undo;
	};

   /* Friend Class Transaction
   * Wraps a list of actions. Provides interface for executing associated
   * actions and rolling back those actions.
   */
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
   /* Collection
   * @Param aszName Name of the collection
   * @Param aoSource Pointer to a collection source.
   * @Param alstLoadedCollections Pointer to a list of loaded collections. (Used in producing debug information)
   */
	Collection(std::string aszName, CollectionSource* aoSource, std::vector<std::string>* alstLoadedCollections);
	~Collection();

	/* GetName()
   * @Return The name of this collection.
   */
	std::string GetName();

   /* Additem (Version 1) (Transaction)
   * Adds the item specified.
   * @Param aszNewItem String Name of item to search for in the Collection source.
   * @Param alstAttrs List of attributes that the added item will have. None by default.
   * @Param alstMetaTags List of Meta-Tags that the added Item will have. None by default.
   */
	void AddItem(std::string aszNewItem,
		bool bFinal = true,
		std::vector<std::pair<std::string, std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>(),
		std::vector<std::pair<std::string, std::string>> alstMetaTags = std::vector<std::pair<std::string, std::string>>()) override;
	
   /* Additem (Version 2) (Transaction)
   * Uses the NonUniqueAttrs in the long name to add a card to the collection.
   * @Param aszNewItemLongName Long name of the item to be added. Contains the NonUniqueAttributes.
   * @Param alstMetaTags List of Meta-Tags that the added Item will have.
   */
   void AddItem(std::string aszNewItemLongName,
		std::vector<std::pair<std::string, std::string>> alstMetaTags);

   /* RemoveItem (Version 1) (Transaction)
   * @Param aszRemoveItem Short name of item to be removed.
   * @Param bFinal True indicate that the transaction will be closed within this function call.
   * @Param alstAttrs List of NonUnique Attributes identifying the copy to be removed.
   * @Param alstMeta List of Meta-Tags identifying the copy to be removed.
   */
	void RemoveItem(std::string aszRemoveItem,
		bool bFinal = true,
		std::vector<std::pair<std::string, std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>(),
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>()) override;
   
   /* RemoveItem (Version 2) (Transaction)
   * @Param aszRemoveItemLongName Long name of the item to be removed. Contains the identifying NonUniqueAttributes.
   * @Param alstMetaTags List of Meta-Tags identifying the copy to be removed.
   */
   void RemoveItem(std::string aszRemoveItemLongName, std::vector<std::pair<std::string, std::string>> alstMeta);

   /* RemoveMetaTag (Transaction)
   * The Copy to be modified is identified here, then is stored off for when the operation is to be completed.
   * @Param aszLongName Long Name of item to remove tag from. Contains the identifying NonUniqueAttributes.
   * @Param aszKey Key of Meta-Tag to be removed.
   * @Param alstMatchMeta Meta-Tags Identifying copy to remove tag from.
   */
	void RemoveMetaTag(std::string aszLongName, std::string aszKey,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
	
   /* SetMetaTag  (Transaction)
   * Sets the single Meta-Tag of identified copy.
   * The Copy to be modified is identified here, then is stored off for when the operation is to be completed.
   * @Param aszLongName Long Name of copy to set the tag on. Contains the identifying NonUniqueAttributes.
   * @Param aszKey New Meta-Tag Key to add.
   * @Param aszValue New Meta-Tag Value to be added.
   * @Param alstMatchMeta Meta-Tags Identify the copy to be modified.
   */
   void SetMetaTag(std::string aszLongName, std::string aszKey, std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());
   // 

   /* SetMetaTags  (Transaction)
   * Sets many tags of the identified copy.
   * Used to set OR REMOVE multiple tags at once. "!NULL" in the value position will indicate removal.
   * The Copy to be modified is identified here, then is stored off for when the operation is to be completed.
   * @Param aszLongName Long Name of copy to set the tag on. Contains the identifying NonUniqueAttributes.
   * @Param alstKeyVals List of new Meta-Tag Key Value Pairs to be added to the identified copy.
   * @Param alstMatchMeta Meta-Tags Identify the copy to be modified.
   */
   void SetMetaTags(std::string aszLongName,
      std::vector<std::pair<std::string, std::string>> alstKeyVals = std::vector<std::pair<std::string, std::string>>(),
      std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());

   /* SetNonUniqueAttribute (Transaction)
   * The Copy to be modified is identified here, then is stored off for when the operation is to be completed.
   * @Param aszLongName Long Name of copy to set the attribute on. Contains the identifying NonUniqueAttributes.
   * @Param aszKey String key of attribute to set.
   * @Param aszValue Value of attribute.
   * @Param alstMatchMeta Meta-Tags Identify the copy to be modified.
   */
	void SetNonUniqueAttribute(std::string aszLongName, std::string aszKey, std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>());

   void SetFeatures(
      std::string aszLongName,
      std::vector<std::pair<std::string, std::string>> alstNewMeta,
      std::vector<std::pair<std::string, std::string>> alstNewAttrs,
      std::vector<std::pair<std::string, std::string>> alstMatchMeta);


	// Returns the list of restions if restrictions exist. * if none exist.
   /* GetNonUniqueAttributeRestrictions
   * Gets the possible values of the specified nonunique trait. For example, Thraben inspecter is only in SOI.
   * @Param aszLongName Long Name of card type to get restrictions from. Identifying info does not matter.
   * @Param aszKey Key of NonUniqueAttribute to get restrictions for.
   */
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

	void addItem(std::string aszNewItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	void removeItem(
		std::string aszItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	void setItemAttr(
		CopyObject* aoCO, 
		std::string aszKey,
		std::string aszNewVal);
   void setItemAttrs(
      CopyObject* aoCO,
      std::vector<std::pair<std::string, std::string>> alstAttrs);

	void setMetaTag(
		CopyObject* aoCO,
		std::string aszKey,
		std::string aszNewTag);
   // Change multiple tags on one tag. If not using this, the identity of the
   //  card will change before adding the next.
   void setMetaTags(
      CopyObject* aoCO,
      std::vector<std::pair<std::string,std::string>> alstKeyVals);
	void removeMetaTag(
		CopyObject* aoCO,
		std::string aszKey);

   void setFeatures(CopyObject* aoCO,
      std::vector<std::pair<std::string, std::string>> alstNewMeta,
      std::vector<std::pair<std::string, std::string>> alstNewAttrs);

   // Produces a string representation of the attribute strain
   std::string changeItemAttribute_string(
      std::string aszCardname,
      CopyObject* aoCO,
      std::string aszKey,
      std::string aszNewVal,
      bool bIsParentCol = true);

	Transaction* openTransaction();
	void finalizeTransaction(bool abRecord = true);

	CopyObject* forceAdd(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs);
	// Only adds the collection object cache locations
	void registerItem(int aiItem);

	std::string cardToString(int aiCardFlyweight, std::pair<CopyObject*, int>* aoCopy, bool bFullDets = false);
	std::vector<std::string> splitIntoLines(std::string aszString);
};

#pragma message ("Finish Collection.h")