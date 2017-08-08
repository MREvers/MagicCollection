#include "RemoveAction.h"
#include "Collection.h"

RemoveAction::RemoveAction()
{
   m_szName = "";
   m_szIdentifyingHash = "";
}


RemoveAction::~RemoveAction()
{
}

bool 
RemoveAction::Execute(TransactionManager* aoCol)
{
   CollectionSource* refSource = aoCol->GetSource();

   int iItem = refSource->LoadCard(m_szName);
   CollectionItem* refItem = refSource->GetCardPrototype(iItem);
   if (refItem == nullptr) { return false; }

   std::shared_ptr<CopyItem> refCItem;
   refCItem = refItem->FindCopyItem(m_szIdentifyingHash, m_AddrResidentIn);
   if (refCItem == nullptr) { return false; }

   m_lstMetaOfRMItem = refCItem->GetMetaTags(MetaTagType::Any);
   m_lstTagsOfRMItem = refCItem->GetIdentifyingAttributes();

   aoCol->Remove(m_szName, m_szIdentifyingHash, m_AddrResidentIn);
   return true;
}

bool 
RemoveAction::Rollback(TransactionManager* aoCol)
{
   std::unique_ptr<AddAction> adAction;

   adAction = std::unique_ptr<AddAction>(GetUndoAction(aoCol));
   return adAction->Execute(aoCol);
}

AddAction* 
RemoveAction::GetUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();

   AddAction* adRetVal = new AddAction();
   adRetVal->SetIDs(m_lstTagsOfRMItem);
   adRetVal->SetMeta(m_lstMetaOfRMItem);
   adRetVal->SetName(m_szName);

   return adRetVal;
}

// It is the responsibility of the caller to delete this.
Action* 
RemoveAction::GetCopy() const
{
   RemoveAction* ptCopy = new RemoveAction(*this);
   return ptCopy;
}

void 
RemoveAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void 
RemoveAction::SetHash(std::string aszIDHash)
{
   m_szIdentifyingHash = aszIDHash;
}

void 
RemoveAction::SetResi(const Address& aAddress)
{
   m_AddrResidentIn = aAddress;
}