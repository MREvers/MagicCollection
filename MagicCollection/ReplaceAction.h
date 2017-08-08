#pragma once
#include <vector>
#include "Action.h"

class ReplaceAction : public Action
{
public:
   ReplaceAction();
   ~ReplaceAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   Action* GetCopy() const override;

   void SetName(std::string aszName);
   void SetHash(std::string aszHash);
   void SetNewCard(std::string aszNewCardName);
   void SetIDs(std::vector<Tag> alstIds);
   void SetMeta(std::vector<Tag> alstMetas);

private:
   std::string m_szName;
   std::string m_szIdentifyingHash;
   std::string m_szNewName;
   std::vector<Tag> m_lstIdChanges;
   std::vector<Tag> m_lstMetaChanges;

   std::vector<Tag> m_lstUndoIdChanges;
   std::vector<Tag> m_lstUndoMetaChanges;

   ReplaceAction* GetUndoAction(TransactionManager* aoCol) const;
};

