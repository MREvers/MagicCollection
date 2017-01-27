#pragma once
#pragma message ("Starting Collection.h")
#define _ITERATOR_DEBUG_LEVEL 0  
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
    std::vector<std::pair<std::string,std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>()) override;
   
   void RemoveItem(std::string aszNewItem,
    bool bFinal = true,
    std::vector<std::pair<std::string, std::string>> alstAttrs = std::vector<std::pair<std::string, std::string>>()) override;
   
   void RollbackTransaction();

   void LoadCollection(std::string aszCollectionFile);
   void SaveCollection(std::string aszCollectionFileName);
   // Clears the history file, then writes the baseline.
   void CreateBaselineHistory();

   void PrintList();

   bool TransactionIntercept;

private:
   CollectionSource* m_ColSource;

   // Collections just know that they have AT LEAST a copy of a card. It has to access
   //  the Col object to see how many copies.
   std::vector<int> m_lstCollection;
   std::string m_szName;
   std::vector<Transaction> m_lstTransactions;
   std::string m_szHistoryFileName;
   std::vector<std::string>* m_lstLoadedCollectionsBuffer;

   void addItem(std::string aszNewItem, std::vector<std::pair<std::string, std::string>> alstAttrs);
   void removeItem(std::string aszItem, std::vector<std::pair<std::string, std::string>> alstAttrs);

   Transaction* openTransaction();
   void finalizeTransaction(bool abRecord = true);
   std::vector<std::string> getCollectionString();

   std::string cardToString(int aiCardFlyweight, std::pair<CopyObject*, int>* aoCopy);
};

#pragma message ("Finish Collection.h")