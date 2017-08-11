#include "TransactionManager.h"
#include "Addresser.h"
#include "Collection.h"

TransactionManager::TransactionManager(Collection* aptrCollection)
{
   m_ptrCollection = aptrCollection;
}


TransactionManager::~TransactionManager()
{
   for each (Transaction* ptTr in m_lstOpenTransactions)
   {
      delete ptTr;
   }
   m_lstOpenTransactions.clear();
}

void 
TransactionManager::IncludeAction(const Action& aAction)
{
   Transaction* ptrTrans = getTransaction();
   ptrTrans->AddAction(aAction);
}

void 
TransactionManager::FinalizeTransaction(bool abActual)
{
   if (!abActual) { return; }
   bool bGood = true;

   Transaction* ptrTrans = getTransaction();
   if (ptrTrans->IsOpen())
   {
      bGood = ptrTrans->Finalize(this);
   }

   if (!bGood)
   {
      clearTransactions();
   }
}

void 
TransactionManager::RollbackTransaction()
{
   bool bGood = true;

   Transaction* ptrTrans = getLastTransaction();
   if (ptrTrans != nullptr && !ptrTrans->IsOpen())
   {
      bGood = ptrTrans->Rollback(this);
   }
   
   if (!bGood)
   {
      clearTransactions();
   }
}

void 
TransactionManager::TransactionsAsynced()
{
   clearTransactions();
}

Collection* 
TransactionManager::GetCollection()
{
   return m_ptrCollection;
}

CollectionSource* 
TransactionManager::GetSource()
{
   return m_ptrCollection->m_ptrCollectionSource;
}

void 
TransactionManager::Add(
   std::string aszName,
   std::vector<Tag> alstAttrs,
   std::vector<Tag> alstMetaTags)
{
   m_ptrCollection->addItem(aszName, alstAttrs, alstMetaTags);
}

void 
TransactionManager::AddFrom(
   std::string aszName,
   std::string aszIdentifyingHash,
   const Address& aResiAddress)
{
   m_ptrCollection->addItemFrom(aszName, aszIdentifyingHash, aResiAddress);
}

void 
TransactionManager::Remove(
   std::string aszName,
   std::string aszIdentifyingHash,
   Address aszResidentIn)
{
   m_ptrCollection->removeItem(aszName, aszIdentifyingHash, aszResidentIn);
}

void 
TransactionManager::Change(
   std::string aszName,
   std::string aszIdentifyingHash,
   std::vector<Tag> alstChanges,
   std::vector<Tag> alstMetaChanges)
{
   m_ptrCollection->changeItem( aszName, aszIdentifyingHash,
                                alstChanges, alstMetaChanges );
}

void 
TransactionManager::Replace(
   std::string aszName,
   std::string aszIdentifyingHash,
   std::string aszNewName,
   std::vector<Tag> alstIdChanges,
   std::vector<Tag> alstMetaChanges)
{
   m_ptrCollection->replaceItem( aszName, aszIdentifyingHash, 
                                 aszNewName, alstIdChanges,
                                 alstMetaChanges );
}

Transaction* 
TransactionManager::getTransaction()
{
   size_t uiTransacts = m_lstOpenTransactions.size();
   if (uiTransacts == 0)
   {
      Transaction* ptrNewTrans = new Transaction();
      m_lstOpenTransactions.push_back(ptrNewTrans);
      uiTransacts++;
   }

   if (!m_lstOpenTransactions.at(uiTransacts - 1)->IsOpen())
   {
      Transaction* ptrNewTrans = new Transaction();
      m_lstOpenTransactions.push_back(ptrNewTrans);
   }

   return m_lstOpenTransactions.back();
}

Transaction* 
TransactionManager::getLastTransaction()
{
   if (m_lstOpenTransactions.size() > 0)
   {
      return m_lstOpenTransactions.back();
   }
   else
   {
      return nullptr;
   }
}

void 
TransactionManager::clearTransactions()
{
   for each (Transaction* ptTr in m_lstOpenTransactions)
   {
      delete ptTr;
   }
   m_lstOpenTransactions.clear();
}