#pragma once
#include <string>
#include <vector>
#include "Action.h"
#include "Addresser.h"
#include "AddAction.h"

class RemoveAction : public Action
{
public:
   RemoveAction();
   ~RemoveAction();

   bool Execute(TransactionManager* aoCol) override;
   bool Rollback(TransactionManager* aoCol) override;

   Action* GetCopy() const override;

   void SetName(std::string aszName);
   void SetHash(std::string aszIDHas);
   void SetResi(const Address& aAddress);

private:
   std::string m_szName;
   std::string m_szIdentifyingHash;
   Address m_AddrResidentIn;

   std::vector<Tag> m_lstTagsOfRMItem;
   std::vector<Tag> m_lstMetaOfRMItem;

   AddAction* GetUndoAction(TransactionManager* aoCol) const;
};

