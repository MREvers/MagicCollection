#include "RemoveAction.h"
#include "Collection.h"

RemoveAction::RemoveAction()
{
   m_szName = "";
   m_szUID = "";
}


RemoveAction::~RemoveAction()
{
}

bool 
RemoveAction::Execute(TransactionManager* aoCol)
{
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionItem> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }

   //std::shared_ptr<CopyItem> refCItem;
   //refCItem = refItem->FindCopy(m_szIdentifyingHash, m_AddrResidentIn);
   //if (refCItem == nullptr) { return false; }

   //m_lstMetaOfRMItem = refCItem->GetMetaTags(MetaTagType::Any);
   //m_lstTagsOfRMItem = refCItem->GetIdentifyingAttributes();

   //aoCol->Remove(m_szName, m_szIdentifyingHash, m_AddrResidentIn);
   return true;
}

bool 
RemoveAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> adAction;

   adAction = getUndoAction(aoCol);
   return adAction->Execute(aoCol);
}

std::shared_ptr<Action>
RemoveAction::getUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();

   AddAction* adRetVal = new AddAction();
   adRetVal->SetIDs(m_lstTagsOfRMItem);
   adRetVal->SetMeta(m_lstMetaOfRMItem);
   adRetVal->SetName(m_szName);

   return std::shared_ptr<Action>(adRetVal);
}

// It is the responsibility of the caller to delete this.
std::shared_ptr<Action>
RemoveAction::GetCopy() const
{
   RemoveAction* ptCopy = new RemoveAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}

void 
RemoveAction::SetResi(const Location& address)
{
   m_Address = address;
}

void 
RemoveAction::SetName(const std::string& aszName)
{
   m_szName = aszName;
}

void 
RemoveAction::SetUID(const std::string& aszUID)
{
   m_szUID = aszUID;
}