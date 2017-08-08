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
   int iItem = refSource->LoadCard(m_szName);
   CollectionItem* refItem = refSource->GetCardPrototype(iItem);
   if (refItem == nullptr) { return false; }

   // Verify the second item
   iItem = refSource->LoadCard(m_szNewName);
   CollectionItem* refItemTwo = refSource->GetCardPrototype(iItem);
   if (refItemTwo == nullptr) { return false; }

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
   std::unique_ptr<ReplaceAction> chAction;

   chAction = std::unique_ptr<ReplaceAction>(GetUndoAction(aoCol));
   return chAction->Execute(aoCol);
}

Action*
ReplaceAction::GetCopy() const
{
   ReplaceAction* ptCopy = new ReplaceAction(*this);
   return ptCopy;
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

ReplaceAction*
ReplaceAction::GetUndoAction(TransactionManager* aoCol) const
{
   Collection* refCollection = aoCol->GetCollection();
   CollectionSource* refSource = aoCol->GetSource();

   int iItem = refSource->LoadCard(m_szNewName);
   CollectionItem* refItem = refSource->GetCardPrototype(iItem);
   if (refItem == nullptr) { return false; }
   
   std::string szHashRM;  
   szHashRM = refItem->GetHash( refCollection->GetIdentifier(),
                                m_lstIdChanges, m_lstMetaChanges );

   ReplaceAction* rpAction = new ReplaceAction();
   rpAction->SetName(m_szNewName);
   rpAction->SetHash(szHashRM);
   rpAction->SetIDs(m_lstUndoIdChanges);
   rpAction->SetMeta(m_lstUndoMetaChanges);
   rpAction->SetNewCard(m_szName);

   return rpAction;
}