#include "ReplaceAction.h"
#include "Collection.h"

ReplaceAction::ReplaceAction()
{
}


ReplaceAction::~ReplaceAction()
{
}


bool
ReplaceAction::Execute(TransactionManager* aoCol)
{
   CollectionSource* refSource = aoCol->GetSource();
   Collection* refCollection = aoCol->GetCollection();

   // Verify the first item
   TryGet<CollectionItem> refItem = refSource->GetCardPrototype(m_szName);
   if (!refItem.Good()) { return false; }

   // Verify the second item
   TryGet<CollectionItem> refItemTwo = refSource->GetCardPrototype(m_szNewName);
   if (!refItemTwo.Good()) { return false; }

   std::shared_ptr<CopyItem> refCItem;
   refCItem = refItem->FindCopyItem( m_szIdentifyingHash, 
                                     refCollection->GetIdentifier() );
   if (refCItem == nullptr) { return false; }

   m_lstUndoMetaChanges = refCItem->GetMetaTags(MetaTagType::Any);
   m_lstUndoIdChanges = refCItem->GetIdentifyingAttributes();

   aoCol->Replace( m_szName, m_szIdentifyingHash, m_szNewName, 
                   m_lstIdChanges, m_lstMetaChanges );
   return true;
}

bool
ReplaceAction::Rollback(TransactionManager* aoCol)
{
   std::shared_ptr<Action> chAction;

   chAction = getUndoAction(aoCol);
   return chAction->Execute(aoCol);
}

std::shared_ptr<Action>
ReplaceAction::GetCopy() const
{
   ReplaceAction* ptCopy = new ReplaceAction(*this);
   return std::shared_ptr<Action>(ptCopy);
}

void
ReplaceAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void
ReplaceAction::SetHash(std::string aszHash)
{
   m_szIdentifyingHash = aszHash;
}

void
ReplaceAction::SetNewCard(std::string aszNewCardName)
{
   m_szNewName = aszNewCardName;
}

void
ReplaceAction::SetIDs(std::vector<Tag> alstIds)
{
   m_lstIdChanges = alstIds;
}

void
ReplaceAction::SetMeta(std::vector<Tag> alstMetas)
{
   m_lstMetaChanges = alstMetas;
}

std::shared_ptr<Action>
ReplaceAction::getUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   TryGet<CollectionItem> refItem = refSource->GetCardPrototype(m_szNewName);
   if (!refItem.Good()) { return false; }
   
   std::string szHashRM;  
   szHashRM = refItem->GetHash( refCollection->GetIdentifier(),
                                m_lstIdChanges, m_lstMetaChanges );

   ReplaceAction* rpAction = new ReplaceAction();
   rpAction->SetName(m_szNewName);
   rpAction->SetHash(szHashRM);
   rpAction->SetIDs(m_lstUndoIdChanges);
   rpAction->SetMeta(m_lstUndoMetaChanges);
   rpAction->SetNewCard(m_szName);

   return std::shared_ptr<Action>(rpAction);
}