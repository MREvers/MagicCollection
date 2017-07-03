#pragma once

#include <vector>
#include <map>
#include <iterator>
#include <string>

#include "Action.h"

class ItemCollection;

/* Class Transaction
* Wraps a list of actions. Provides interface for executing associated
* actions and rolling back those actions.
*/
class Transaction
{
public:
	Transaction(ItemCollection* aoCol);
	~Transaction();

	void AddAction(Action& aoAct);
	void RemoveAction(int i);
	void Finalize(bool abRecordable = true);
	void Rollback();

	bool IsOpen = true;
	bool IsRecordable = true;
private:
	ItemCollection* m_Col;
	std::vector<Action> Actions;
};
