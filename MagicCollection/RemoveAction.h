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

   std::shared_ptr<Action> GetCopy() const override;

   void SetResi( const Address& aszName );
   void SetName(const std::string& aszName);
   void SetUID(const std::string& aszUID);

private:
   std::string m_szName;
   std::string m_szUID;
   Address m_Address;

   std::vector<Tag> m_lstTagsOfRMItem;
   std::vector<Tag> m_lstMetaOfRMItem;

   std::shared_ptr<Action> getUndoAction(TransactionManager* aoCol) const override;
};

