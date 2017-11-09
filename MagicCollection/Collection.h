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
#include "CollectionTracker.h"
#include "StringHelper.h"
#include "TransactionManager.h"
#include "CollectionFactory.h"
#include "Addresser.h"
#include "CollectionDetails.h"

class CollectionFactory;

class Collection
{
public:
   Collection(
      std::string aszName,
      CollectionSource* aoSource,
      std::string aszID = "");
   ~Collection();

   std::string GetName();
   Location  GetIdentifier();
   unsigned int GetChildCount();
   void ChildAdded();

   void AddItem(
      std::string aszName,
      std::vector<Tag> alstAttrs    = std::vector<Tag>(),
      std::vector<Tag> alstMetaTags = std::vector<Tag>(),
      bool abCloseTransaction       = true);

   void AddItemFrom(
      std::string aszName,
      std::string aszUID,
      const Location& aAddress,
      bool abCloseTransaction = true);

   void RemoveItem(
      std::string aszName, 
      std::string aszUID, 
      bool abCloseTransaction = true);

   void ChangeItem(
      std::string aszName, 
      std::string aszUID, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges, 
      bool abCloseTransaction = true);

   void ReplaceItem(
      std::string aszName, 
      std::string aszUID, 
      std::string aszNewName, 
      std::vector<Tag> alstIdChanges, 
      std::vector<Tag> alstMetaChanges, 
      bool abCloseTransaction = true);

   std::vector<std::string> GetMetaData();

   void SaveCollection();

   bool InitializeCollection();
   bool InitializeCollection( std::string aszFileName,
                              std::vector<std::string>& rlstInitializeLines );
   void LoadCollection(
      std::string aszFileName, 
      CollectionFactory* aoFactory);
   void LoadChanges(std::vector<std::string> aszLines);

   std::vector<std::string> GetCollectionList(
      MetaTagType atagType = Visible,
      bool abCollapsed = true);
   std::vector<std::string> GetShortList();

   bool IsLoaded = false;

private:
   friend class CollectionTracker;
   friend class TransactionManager;

   CollectionDetails* m_ptrCollectionDetails;
   CollectionSource* m_ptrCollectionSource;
   CollectionTracker* m_ptrCollectionTracker;
   TransactionManager* m_ptrTransactionManager;

   std::vector<std::pair<std::string, Tag>> m_lstTaggedItems;

   std::vector<int> m_lstItemCacheIndexes;
   std::vector<int> getCollection();

   // These all locate by name and hash for a second 
   // time so we dont risk dangling references.
   CopyItem* addItem(
      const std::string& aszName, 
      const std::vector<Tag>& alstAttrs, 
      const std::vector<Tag>& alstMetaTags);
   
   void addItemFrom(
      const std::string& aszName, 
      const std::string& aszUID,
      const Identifier& aAddress);

   void removeItem(
      const std::string& aszName, 
      const std::string& aszUID);

   void changeItem(
      const std::string& aszName, 
      const std::string& aszUID,
      const std::vector<Tag>& alstChanges, 
      const std::vector<Tag>& alstMetaChanges);

   void replaceItem(
      const std::string& aszName, 
      const std::string& aszUID,
      const std::string& aszNewName, 
      const std::vector<Tag>& alstChanges, 
      const std::vector<Tag>& alstMetaChanges);

   void registerItem(int aiCacheIndex);
   void unregisterItem(int aiCacheIndex);

   void modifyItem(
      CopyItem* aptCopy,
      const std::vector<Tag>& alstChanges, 
      const std::vector<Tag>& alstMetaChanges);

   void loadMetaTagFile();

   void loadOverheadFile( std::vector<std::string>& rlstUnprocessedLines );
   bool loadOverheadLine(const std::string& aszLine);
   void loadCollectionDataLine(const std::string& aszData);

   void loadInterfaceLine(const std::string& aszLine);

   void loadAdditionLine(const std::string& aszLine);
   void loadRemoveLine(const std::string& aszLine);
   void loadDeltaLine(const std::string& aszLine);

   void saveHistory();
   void saveMeta();
   void saveOverhead();
   void saveCollection();
};

