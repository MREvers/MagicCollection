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

#include "CollectionObject.h"
// Just use magic card object for now
#include "CollectionSource.h"
#include "StringHelper.h"
#include "Config.h"


class Collection
{
	/* Friend Class PseudoCopy
	*  Used to store information needed to identify/construct a real copy.
	*/
	class PseudoCopy
	{
	public:
		int Count;
		std::string DetailsString;
		std::string Name;
		std::string LongName;
		std::vector<std::pair<std::string, std::string>> IdentifyingAttributes;
		std::vector<std::pair<std::string, std::string>> MetaList;
		CopyObject* RealCopy;
		CollectionObject* Prototype;
		int CacheIndex;
		bool Ok;
		bool FoundCardClass;
		bool FoundCardCopy;

		PseudoCopy(std::string aszCollectionName,
			CollectionSource* aoColSource,
			std::string aszLongName,
			std::vector<std::pair<std::string, std::string>> alstMeta);

		PseudoCopy(std::string aszCollectionName,
			CollectionSource* aoColSource,
			std::string aszName,
			std::vector<std::pair<std::string, std::string>> alstAttrs,
			std::vector<std::pair<std::string, std::string>> alstMeta);

		bool LoadCard();

		bool FindCopy();

		CopyObject GeneratePseudoCopy();

		std::string ToString(bool bFullDets = false, bool bIncludeMeta = false);

	private:
		CollectionSource* m_ColSource;
		std::string m_szCollectionName;

		// Private Constructor for common features of the common constructors
		PseudoCopy(std::string aszCollectionName,
			CollectionSource* aoColSource,
			std::vector<std::pair<std::string, std::string>> alstMeta);
	};

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
	/* Collection (Version 1)
	* @Param aszName Name of the collection
	* @Param aoSource Pointer to a collection source.
	* @Param alstLoadedCollections Pointer to a list of loaded collections. (Used in producing debug information)
	*/
	Collection(std::string aszName,
		CollectionSource* aoSource,
		std::vector<std::string>* alstLoadedCollections);

	/* Collection (Version 2)
	* @Param aszName Name of the collection
	* @Param aszParentName Name of the Parent collection
	* @Param aoSource Pointer to a collection source.
	* @Param alstLoadedCollections Pointer to a list of loaded collections. (Used in producing debug information)
	*/
	Collection(std::string aszName,
		std::string aszParentName,
		CollectionSource* aoSource,
		std::vector<std::string>* alstLoadedCollections);
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
		std::vector<std::pair<std::string, std::string>> alstMetaTags = std::vector<std::pair<std::string, std::string>>());

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
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>());

	/* RemoveItem (Version 2) (Transaction)
	* @Param aszRemoveItemLongName Long name of the item to be removed. Contains the identifying NonUniqueAttributes.
	* @Param alstMetaTags List of Meta-Tags identifying the copy to be removed.
	*/
	void RemoveItem(std::string aszRemoveItemLongName, std::vector<std::pair<std::string, std::string>> alstMeta);

	/* ReplaceItem (Transaction)
	* @Param aszRemoveItemLongName  Long name of the item to be removed. Contains the identifying NonUniqueAttributes.
	* @Param alstIdentifyingMeta  List of Meta-Tags identifying the copy to be removed.
	* @Param aszAddItemLongName Long name of the item to be added. Contains the NonUniqueAttributes.
	* @Param alstNewMeta List of Meta-Tags that the added Item will have. None by default.
	* @Param bFinal Finalize Transaction when function completes.
	*/
	void ReplaceItem(std::string aszRemoveItemLongName,
		std::vector<std::pair<std::string, std::string>> alstIdentifyingMeta,
		std::string aszAddItemLongName,
		std::vector<std::pair<std::string, std::string>> alstNewMeta,
		bool bFinal = true);

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
		std::vector<std::pair<std::string, std::string>> alstMatchMeta = std::vector<std::pair<std::string, std::string>>(),
		bool bFinal = true);
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

	/* SetFeatures (Transaction)
	*  Sets both the attributes and the meta tags of the identified copy.
	*  @Param aszLongName Long Name of copy to set the attribute on. Contains the identifying NonUniqueAttributes.
	*  @Param alstNewMeta Meta Tags to assign to the identified copy.
	*  @Param alstNewAttrs Attributes to assign to the identified copy
	*  @Param alstMatchMeta Meta-Tags Identify the copy to be modified.
	*/
	void SetFeatures(
		std::string aszLongName,
		std::vector<std::pair<std::string, std::string>> alstNewMeta,
		std::vector<std::pair<std::string, std::string>> alstNewAttrs,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta,
		bool bFinal = true);

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
	void LoadBulkChanges(std::vector<std::string>& alstBulkChanges);
	// Clears the history file, then writes the baseline.
	void CreateBaselineHistory();
	void RecordForcedTransaction(std::string aszTransactionString);
	std::vector<std::string> GetCollectionList();
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
		GetCollectionListWithMeta();

	static std::vector<std::pair<std::string, std::string>> ParseAttrs(std::string aszAttrs);
	static bool ParseCardLine(std::string aszLine, int& riCount, std::string& rszName, std::string& rszDetails);

	void PrintList();

private:
	CollectionSource* m_ColSource;

	// Collections just know that they have AT LEAST a copy of a card. It has to access
	//  the Col object to see how many copies.
	std::vector<int> m_lstCollection;
	std::string m_szName;
	std::string m_szParentName;
	std::vector<Transaction> m_lstTransactions;
	std::vector<std::string> m_lstUnreversibleChanges;
	std::string m_szHistoryFileName;
	std::string m_szMetaTagFileName;
	std::vector<std::string>* m_lstLoadedCollectionsBuffer;

	void setName(std::string aszName);

	void addItem(std::string aszNewItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	bool removeItem(
		std::string aszItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	void replaceItem(std::string aszNewItem,
		std::vector<std::pair<std::string, std::string>> alstNewAttrs,
		std::vector<std::pair<std::string, std::string>> alstNewMeta,
		std::string aszOldItem,
		std::vector<std::pair<std::string, std::string>> alstOldAttrs,
		std::vector<std::pair<std::string, std::string>> alstOldMeta);

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
		std::vector<std::pair<std::string, std::string>> alstKeyVals);
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

	/* loadCollectionFromFile
	*  Uses "loadCardLine" to load each line in the collection file.
	*  This function ignores the header lines in the collection file. Ie.
	*  The CollectionName and the Parent Name lines are ignored. These lines
	*  are captured my the collection factory function used to create this collection.
	*/
	void loadCollectionFromFile(std::string aszFileName);

	/* loadCollectionLines
	*  See loadCollectionFromFile
	*  It is probably best to call refreshCopyLinks immediately following a call to this
	*  if loading card lines for other collections.
	*/
	void loadCollectionLines(std::vector<std::string>& alstLines);

	/* loadCardLine
	*  Used in loading a card into a collection that accounts for the fact that the copy may
	*  have been generated by an already loaded collection.
	*  Basically looks to see if the copy generated by the card line actually references an
	*  already existing copy. Perhaps one that was loaded by another collection, for example, if
	*  the other collection loaded a copy that has Parent=this collection.
	*  Requires finalizeTransaction to be called after use.
	*  @Param aszNewCardLine Line of card with # and Longname - e.g. # Longname
	*  @Return True if loaded successfully
	*/
	bool loadCardLine(std::string aszNewCardLine);

	/* loadRemoveCardLine
	*  This is different from the other load line functions because it expects a
	*  DIFFERENT REPRESENTATION.
	*  Expects LongName : {Meta}
	*/
	bool loadRemoveCardLine(std::string aszRemoveCardLine);

	/* loadChangeCardLine
	*  Either changes the features of a card or replaces a card depending on input.
	*  This is different from the other load line functions because it expects a
	*  DIFFERENT REPRESENTATION.
	*  Expects LongName : {Meta} -> LongName : {Meta}
	*/
	bool loadChangeCardLine(std::string aszChangeCardLine);

	/* loadMetaTags
	*  Loads a file of meta tags of the format - LongName : { Meta Tags }
	*  For each line in the file, it searches for a "LongName" copy with NO meta tags
	*  and assigns the Meta Tags to it.
	*/
	void loadMetaTagsFromFile();

	/* loadMetaTagLines
	*  See loadMetaTagsFromFile
	*/
	void loadMetaTagLines(std::vector<std::string>& alstLines);

	/* refreshCopyLinks
	* If there are cards with parent collection == this but
	* this is not in their residents, then that copy no longer exists.
	* This function searches for all such copies and changes them to
	* virtual. I.e. Parent = ""
	*/
	void refreshCopyLinks(std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges);

	Transaction* openTransaction();
	void finalizeTransaction(bool abRecord = true);

	CopyObject* forceAdd(std::string aszNewItem,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>());
	// Only adds the collection object cache locations
	void registerItem(int aiItem);

	PseudoCopy generatePseudoCopy(std::string aszLongName,
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>());

	PseudoCopy generatePseudoCopy(
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::string aszName,
		std::vector<std::pair<std::string, std::string>> alstMeta = std::vector<std::pair<std::string, std::string>>());

	void setTransactionsNoWrite();
	std::vector<int>& getCollectionList();
};

#pragma message ("Finish Collection.h")