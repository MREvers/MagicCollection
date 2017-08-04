#include "TransactionManager.h"
#include "Addresser.h"
#include "Collection.h"

TransactionManager::TransactionManager(Collection* aptrCollection)
{
   m_ptrCollection = aptrCollection;
}


TransactionManager::~TransactionManager()
{

}

void 
TransactionManager::IncludeAction(const Action& aAction)
{

}

void 
TransactionManager::FinalizeTransaction()
{
   Transaction* ptrTrans = getTransaction();
   if (ptrTrans->IsOpen())
   {
      ptrTrans->Finalize(this);
   }
}

void 
TransactionManager::RollbackTransaction()
{
   Transaction* ptrTrans = getTransaction();
   if (!ptrTrans->IsOpen())
   {
      ptrTrans->Rollback(this);
   }
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
   }

   if (!m_lstOpenTransactions.at(uiTransacts - 1)->IsOpen())
   {
      Transaction* ptrNewTrans = new Transaction();
      m_lstOpenTransactions.push_back(ptrNewTrans);
   }

   return m_lstOpenTransactions.back();
}