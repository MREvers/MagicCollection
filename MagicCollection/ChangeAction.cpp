#include "ChangeAction.h"
#include "Collection.h"

ChangeAction::ChangeAction()
{
}


ChangeAction::~ChangeAction()
{
}

bool 
ChangeAction::Execute(TransactionManager* aoCol) 
{
   CollectionSource* refSource = aoCol->GetSource();
   Collection* refCollection = aoCol->GetCollection();

   int iItem = refSource->LoadCard(m_szName);
   CollectionItem* refItem = refSource->GetCardPrototype(iItem);
   if (refItem == nullptr) { return false; }

   std::shared_ptr<CopyItem> refCItem;
   refCItem = refItem->FindCopyItem( m_szIdentifyingHash, 
                                     refCollection->GetIdentifier() );
   if (refCItem == nullptr) { return false; }

   aoCol->Change( m_szName, m_szIdentifyingHash, 
                  m_lstIdChanges, m_lstMetaChanges );

   // After the change is made, store off the details of how 
   // to find it again.
   m_szUndoIdentifyingHash = refCItem->GetHash();
   m_lstUndoMeta = refCItem->GetMetaTags(MetaTagType::Any);
   m_lstUndoID = refCItem->GetIdentifyingAttributes();

   return true;
}

bool 
ChangeAction::Rollback(TransactionManager* aoCol)
{
   std::unique_ptr<ChangeAction> chAction;

   chAction = std::unique_ptr<ChangeAction>(GetUndoAction(aoCol));
   return chAction->Execute(aoCol);
}

Action*
ChangeAction::GetCopy() const
{
   ChangeAction* ptCopy = new ChangeAction(*this);
   return ptCopy;
}

void 
ChangeAction::SetName(std::string aszName)
{
   m_szName = aszName;
}
   
void 
ChangeAction::SetIDs(std::vector<Tag> alstIds)
{
   m_lstIdChanges = alstIds;
}
   
void 
ChangeAction::SetHash(std::string aszHash)
{
   m_szIdentifyingHash = aszHash;
}

void 
ChangeAction::SetMeta(std::vector<Tag> alstMeta)
{
   m_lstMetaChanges = alstMeta;
}

ChangeAction* 
ChangeAction::GetUndoAction(TransactionManager* aoCol) const
{
   ChangeAction* chAction = new ChangeAction();
   chAction->SetHash(m_szUndoIdentifyingHash);
   chAction->SetIDs(m_lstUndoID);
   chAction->SetMeta(m_lstUndoMeta);
   chAction->SetName(m_szName);

   return chAction;
}