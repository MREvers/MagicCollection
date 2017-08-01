#pragma once
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "CollectionIO.h"
#include "CollectionSource.h"
#include "CollectionItem.h"
#include "StringHelper.h"
#include "Transaction.h"
#include "CollectionFactory.h"
#include "Addresser.h"

class CollectionFactory;

class Collection
{
public:
   Collection(
      std::string aszName,
      CollectionSource* aoSource,
      std::string aszFileCollection,
      std::string aszID = "");
   ~Collection();

   void SetName(std::string aszNewName);
   std::string GetName();
   Address  GetIdentifier();

   void AddItem(
      std::string aszName,
      std::vector<Tag> alstAttrs = std::vector<Tag>(),
      std::vector<Tag> alstMetaTags = std::vector<Tag>(),
      bool abCloseTransaction = true);

   void AddItem(
      std::string aszName,
      std::string aszHash, 
      std::string aszResidentIn = "", 
      bool abCloseTransaction = true);

   void RemoveItem(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      bool abCloseTransaction = true);

   void ChangeItem(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges, 
      bool abCloseTransaction = true);

   void ReplaceItem(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      std::string aszNewName, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges, 
      bool abCloseTransaction = true);

   std::vector<std::string> GetMetaData();

   // Used to track the number of children in of this collection to 
   // avoid name clashes.
   void ChildAdded();
   int ChildCount();

   long GetTimeStamp();

   void SaveCollection();

   void LoadCollection(
      std::string aszFileName, 
      CollectionFactory* aoFactory);

   void LoadChanges(std::vector<std::string> aszLines);

   std::vector<std::string> GetCollectionList(
      MetaTagType atagType = Visible,
      bool aiCollapsed = false);

   bool IsLoaded = false;

private:
   std::string m_szName;
   Address m_Address;
   std::string m_szFileName;
   unsigned long m_ulTimeStamp;
   unsigned int m_iChildrenCount; // Needed to avoid name clashes.

   CollectionSource* m_ptrCollectionSource;

   bool m_bRecordChanges;
   std::vector<Transaction> m_lstTransactions;

   std::vector<std::pair<std::string, Tag>> m_lstTaggedItems;

   // This should be updated after loading, and whenever history is checked.
   std::map<int, std::vector<CopyItem*>> m_mapCollectionTracker;
   void captureChanges();

   std::vector<int> m_lstItemCacheIndexes;
   std::vector<int> getCollection();

   // These all locate by name and hash for a second 
   // time so we dont risk dangling references.
   void addItem(
      std::string aszName, 
      std::vector<Tag> alstAttrs, 
      std::vector<Tag> alstMetaTags);
   
   // this is for an Item from another collection
   void addExistingItem(
      std::string aszName, 
      std::string aszHash, 
      std::string aszResidentIn);
   
   void removeItem(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      Address aszResidentIn);

   void changeItem(
      std::string aszName,  
      std::string aszIdentifyingHash, 
      std::vector<Tag> alstChanges, 
      std::vector<Tag> alstMetaChanges);

   void replaceItem(
      std::string aszName, 
      std::string aszIdentifyingHash, 
      std::string aszNewName, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges);

   void registerItem(int aiCacheIndex);

   void modifyItem(
      CopyItem* aptCopy,
      std::vector<Tag> alstChanges, 
      std::vector<Tag> alstMetaChanges);

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

