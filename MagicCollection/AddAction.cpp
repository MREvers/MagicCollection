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
   std::shared_ptr<Action> rmAction;

   rmAction = getUndoAction(aoCol);
   return rmAction->Execute(aoCol);
}

// It is the responsibility of the caller to destroy the action.
std::shared_ptr<Action>
AddAction::getUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionItem> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }
   
   std::string szHashRM;  
   szHashRM = refItem->GetHash( refCollection->GetIdentifier(),
                                m_lstIDs, m_lstMetas );

   RemoveAction* rmRetVal = new RemoveAction();
   rmRetVal->SetHash(szHashRM);
   rmRetVal->SetResi(refCollection->GetIdentifier());
   rmRetVal->SetName(m_szName);

   return std::shared_ptr<Action>(rmRetVal);
}

std::shared_ptr<Action> 
AddAction::GetCopy() const
{
   Action* ptCopy = new AddAction(*this);
   return std::shared_ptr<Action>(ptCopy);
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
