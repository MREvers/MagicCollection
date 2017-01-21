#include "Collection.h"

Collection::Transaction::Transaction(Collection* aoCol)
{
   m_Col = aoCol;
}

Collection* Collection::Transaction::operator->()
{
   m_Col->TransactionIntercept = true;
   return m_Col;
}

void Collection::Transaction::AddAction(Action& aoAct)
{
   if (IsOpen)
   {
      m_lstActions.push_back(aoAct);
   }

}

void Collection::Transaction::RemoveAction(int i)
{
   if (IsOpen)
   {
      m_lstActions.erase(m_lstActions.begin() + i);
   }
}

void Collection::Transaction::Finalize()
{
   if (IsOpen)
   {
      IsOpen = false;
      for (int i = 0; i < m_lstActions.size(); i++)
      {
         m_lstActions.at(i).Do();
      }
   }

}

Collection::Collection(std::string aszName, CollectionSource* aoSource)
{
   m_ColSource = aoSource;
   m_szName = aszName;
   TransactionIntercept = false;
}

Collection::~Collection()
{
}

std::string Collection::GetName()
{
   return m_szName;
}

void Collection::AddItem(std::string aszNewItem, bool bFinal)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {
      // Store the action and how to undo it here.
      Collection::Action oAction;
      oAction.Do = std::bind(&Collection::addItem, this, aszNewItem);
      oAction.Undo = std::bind(&Collection::removeItem, this, aszNewItem);

      // Store the arguments
      Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
      oTrans->AddAction(oAction);

      TransactionIntercept = false;
      return;
   }

   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->AddItem(aszNewItem);

   if (bFinal)
   {
      oTrans->Finalize();
   }

}

void Collection::RemoveItem(std::string aszRemoveItem, bool bFinal)
{
   // If we are in the midst of a transaction
   if (TransactionIntercept)
   {

      Collection::Action oAction;
      oAction.Do = std::bind(&Collection::removeItem, this, aszRemoveItem);
      oAction.Undo = std::bind(&Collection::addItem, this, aszRemoveItem);

      // Store the arguments
      Collection::Transaction* oTrans = &m_lstTransactions.at(m_lstTransactions.size() - 1);
      oTrans->AddAction(oAction);

      TransactionIntercept = false;
      return;
   }

   Collection::Transaction* oTrans = openTransaction();

   (*oTrans)->RemoveItem(aszRemoveItem);

   if (bFinal)
   {
      oTrans->Finalize();
   }

}

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::addItem(std::string aszNewItem)
{
   int iCard = m_ColSource->LoadCard(aszNewItem);
   if (iCard != -1)
   {
      // Make sure we don't already 'have' this card.
      bool bNewCard = true;
      for (int i = 0; i < m_lstCollection.size(); i++)
      {
         bNewCard &= m_lstCollection.at(i) != iCard;
      }

      if (bNewCard)
      {
         m_lstCollection.push_back(iCard);
      }

      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card.
      oCard->AddCopy(m_szName);

   }

}

void Collection::removeItem(std::string aszRemoveItem)
{
   int iCard = m_ColSource->LoadCard(aszRemoveItem);
   if (iCard != -1)
   {
      // Get a copy of this card.
      CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

      // Add a copy of this card.
      oCard->RemoveCopy(m_szName);
   }
}

void Collection::LoadCollection(std::string aszFileName)
{
   std::ifstream in(aszFileName);
   std::stringstream buffer;
   buffer << in.rdbuf();
   std::string contents(buffer.str());

   std::vector<std::string> lstLines;
   std::string szLine = "";
   for (int i = 0; i < contents.size(); i++)
   {

      if (contents[i] == '\n')
      {
         lstLines.push_back(szLine);
         szLine = "";
      }
      else
      {
         szLine = szLine + contents[i];
         if (i == contents.size() - 1)
         {
            lstLines.push_back(szLine);
            szLine = "";
         }
      }

   }

   for (std::vector<std::string>::iterator iter = lstLines.begin(); iter != lstLines.end(); ++iter)
   {
      int i = 0;
      std::string szNum = "";
      while (i < iter->size() && (*iter).at(i) < '9' && (*iter).at(i) > '0')
      {
         szNum = szNum + iter->at(i);
         i++;
      }

      if (i >= iter->size())
      {
         return;
      }

      if (iter->at(i) == 'x')
      {
         i++;
      }

      if (i >= iter->size())
      {
         return;
      }

      std::string szName = "";
      while (i < iter->size() &&
         ((iter->at(i) >= 'a' && iter->at(i) <= 'z') ||
         (iter->at(i) >= 'A' && iter->at(i) <= 'Z') ||
            (iter->at(i) == ',' || iter->at(i) == ' ')))
      {
         szName = szName + iter->at(i);
         i++;
      }

      try
      {
         int iNum = std::stoi(szNum);
         szName.erase(0, szName.find_first_not_of(' '));
         szName.erase(szName.find_last_not_of(' ') + 1);
         for (int i = 0; i < iNum; i++)
         {
            AddItem(szName, false);
         }
         finalizeTransaction();
      }
      catch (...)
      {

      }

   }
}

void Collection::PrintList()
{
   for (int i = 0; i < m_lstCollection.size(); i++)
   {
      for (int t = 0; t < m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetCopies(m_szName).size(); t++)
      {
         std::cout << m_ColSource->GetCardPrototype(m_lstCollection.at(i))->GetName() << std::endl;
      }

   }
}

Collection::Transaction* Collection::openTransaction()
{
// Currently, all i can think of is add/remove item, change/unchange attr of card.
   if ((m_lstTransactions.size() > 0 && !m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen) || 
      !(m_lstTransactions.size() > 0))
   {

      m_lstTransactions.push_back(Collection::Transaction(this));
   }
   return &m_lstTransactions.at(m_lstTransactions.size() - 1);
}

void Collection::finalizeTransaction()
{
   if (m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
   {
      m_lstTransactions.at(m_lstTransactions.size() - 1).Finalize();
   }
}
