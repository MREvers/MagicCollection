#include "AddAction.h"
#include "Collection.h"

AddAction::AddAction()
{
   m_szName = "";
}


AddAction::~AddAction()
{
}

void 
AddAction::Execute(TransactionManager* aoCol)
{
   aoCol->Add(m_szName, m_lstIDs, m_lstMetas);
}

void 
AddAction::Rollback(TransactionManager* aoCol)
{

}

void 
AddAction::SetName(std::string aszName)
{
   m_szName = aszName;
}

void 
AddAction::SetIDs(std::vector<Tag> alstIDs)
{
   m_lstIDs = alstIDs;
}

void 
AddAction::SetMeta(std::vector<Tag> alstMetas)
{
   m_lstMetas = alstMetas;
}
