#pragma once

#include <string>

typedef std::pair<std::string, std::string> Tag;

enum ActionType
{
   Add,
   AddFrom,
   Remove,
   Change, 
   Replace
};

class TransactionManager;

/* Class Action (Used In Collection.cpp)
* Used to wrap an action that changes the collection with an action
* that can undo it. Allows undo button.
* Also maintains a string description of the action so that it can
* be recorded in the .history.txt file.
* This class acts as a data structure.
*/
class Action
{
public:
   Action();
   ~Action();

   virtual void Execute(TransactionManager* aoCol) = 0;
   virtual void Rollback(TransactionManager* aoCol) = 0;

protected:
   bool m_bCanRollBack;
};