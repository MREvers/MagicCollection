#pragma once
#include <string>
#include <vector>
#include "Action.h"


class Collection;

class AddAction : public Action
{
public:
   AddAction();
   ~AddAction();

   void Execute(TransactionManager* aoCol) override;
   void Rollback(TransactionManager* aoCol) override;

   void SetName(std::string aszName);
   void SetIDs(std::vector<Tag> alstIDs);
   void SetMeta(std::vector<Tag> alstMetas);

private:
   std::string m_szName;
   std::vector<Tag> m_lstIDs;
   std::vector<Tag> m_lstMetas;
};

