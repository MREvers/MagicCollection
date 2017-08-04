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

void 
RemoveAction::Execute(TransactionManager* aoCol)
{
   aoCol->Remove(m_szName, m_szIdentifyingHash, m_AddrResidentIn);
}

void 
RemoveAction::Rollback(TransactionManager* aoCol)
{

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
RemoveAction::SetMeta(const Address& aAddress)
{
   m_AddrResidentIn = aAddress;
}