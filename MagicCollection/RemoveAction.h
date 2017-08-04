#pragma once
#include <string>
#include <vector>
#include "Action.h"
#include "Addresser.h"

class RemoveAction : public Action
{
public:
   RemoveAction();
   ~RemoveAction();

   void Execute(TransactionManager* aoCol) override;
   void Rollback(TransactionManager* aoCol) override;

   void SetName(std::string aszName);
   void SetHash(std::string aszIDHas);
   void SetMeta(const Address& aAddress);

private:
   std::string m_szName;
   std::string m_szIdentifyingHash;
   Address m_AddrResidentIn;
};

