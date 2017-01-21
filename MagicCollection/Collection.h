#pragma once
#pragma message ("Starting Collection.h")

#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <functional>

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
      std::function<void()> Do;
      std::function<void()> Undo;
   };

   class Transaction
   {
   public:
      Transaction(Collection* aoCol);

      Collection* operator-> ();
      void AddAction(Action& aoAct);
      void RemoveAction(int i);
      void Finalize();

      bool IsOpen = true;
   private:
      Collection* m_Col;
      std::vector<Action> m_lstActions;
   };
public:
   Collection(std::string aszName, CollectionSource* aoSource);
   ~Collection();

   // Meta
   std::string GetName();

   void AddItem(std::string aszNewItem, bool bFinal = true) override;
   void RemoveItem(std::string aszNewItem, bool bFinal = true) override;

   void LoadCollection(std::string aszCollectionFile);

   void PrintList();

   bool TransactionIntercept;

private:
   CollectionSource* m_ColSource;

   // Collections just know that they have AT LEAST a copy of a card. It has to access
   //  the Col object to see how many copies.
   std::vector<int> m_lstCollection;
   std::string m_szName;
   std::vector<Transaction> m_lstTransactions;

   void addItem(std::string aszNewItem);
   void removeItem(std::string aszItem);

   Transaction* openTransaction();
   void finalizeTransaction();
};

#pragma message ("Finish Collection.h")