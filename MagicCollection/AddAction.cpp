#include "AddAction.h"
#include "Collection.h"
#include "RemoveAction.h"
#include "CollectionSource.h"
#include "CollectionItem.h"

AddAction::AddAction()
{
   m_szName = "";
}


AddAction::~AddAction()
{
}

bool 
AddAction::Execute(TransactionManager* aoCol)
{
   aoCol->Add(m_szName, m_lstIDs, m_lstMetas);

   return true;
}

bool 
AddAction::Rollback(TransactionManager* aoCol)
{
   std::unique_ptr<RemoveAction> rmAction;

   rmAction = std::unique_ptr<RemoveAction>(GetUndoAction(aoCol));
   return rmAction->Execute(aoCol);
}

// It is the responsibility of the caller to destroy the action.
RemoveAction* 
AddAction::GetUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   int iItem = refSource->LoadCard(m_szName);
   CollectionItem* refItem = refSource->GetCardPrototype(iItem);
   if (refItem == nullptr) { return false; }
   
   std::string szHashRM;  
   szHashRM = refItem->GetHash( refCollection->GetIdentifier(),
                                m_lstIDs, m_lstMetas );

   RemoveAction* rmRetVal = new RemoveAction();
   rmRetVal->SetHash(szHashRM);
   rmRetVal->SetResi(refCollection->GetIdentifier());
   rmRetVal->SetName(m_szName);

   return rmRetVal;
}

Action* 
AddAction::GetCopy() const
{
   AddAction* ptCopy = new AddAction(*this);
   return ptCopy;
}

void 
AddAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void 
AddAction::SetIDs(const std::vector<Tag>& alstIDs)
{
   m_lstIDs = alstIDs;
}

void 
AddAction::SetMeta(const std::vector<Tag>& alstMetas)
{
   m_lstMetas = alstMetas;
}
