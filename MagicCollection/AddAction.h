#pragma once
#include <string>
#include <vector>
#include "Action.h"

class RemoveAction;
class Collection;

class AddAction : public Action
{
public:
   AddAction();
   ~AddAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   Action* GetCopy() const override;

   void SetName(std::string aszName);
   void SetIDs(const std::vector<Tag>& alstIDs);
   void SetMeta(const std::vector<Tag>& alstMetas);

private:
   std::string m_szName;
   std::vector<Tag> m_lstIDs;
   std::vector<Tag> m_lstMetas;

   RemoveAction* GetUndoAction(TransactionManager* aoCol) const;
};

