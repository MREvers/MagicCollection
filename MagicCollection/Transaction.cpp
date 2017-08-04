#include "Transaction.h"
#include "Action.h"

Transaction::Transaction()
{
   m_bIsOpen = true;
}

Transaction::~Transaction()
{
}

void 
Transaction::AddAction(const Action& aAct)
{

}

void 
Transaction::Finalize(TransactionManager* aoCol)
{
   std::vector<Action>::iterator iter_Action;
   for (iter_Action  = m_lstActions.begin();
        iter_Action != m_lstActions.end();
        ++iter_Action )
   {
      iter_Action->Execute(aoCol);
   }

   m_bIsOpen = false;
}

void 
Transaction::Rollback(TransactionManager* aoCol)
{
   std::vector<Action>::reverse_iterator iter_Action;
   for (iter_Action  = m_lstActions.rbegin();
        iter_Action != m_lstActions.rend();
        ++iter_Action )
   {
      iter_Action->Rollback(aoCol);
   }

   m_bIsOpen = true;
}

bool 
Transaction::IsOpen()
{
   return m_bIsOpen;
}