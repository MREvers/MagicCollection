#include "Transaction.h"

Transaction::Transaction(ItemCollection* aoCol)
{
	m_Col = aoCol;
	IsRecordable = true;

}

Transaction::~Transaction()
{
	Actions.clear();
}

void Transaction::AddAction(Action& aoAct)
{
	if (IsOpen)
	{
		Actions.push_back(aoAct);
	}
}

void Transaction::RemoveAction(int i)
{
	if (IsOpen)
	{
		Actions.erase(Actions.begin() + i);
	}
}

void Transaction::Finalize(bool abRecordable)
{
	if (IsOpen)
	{
		IsOpen = false;
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Execute();
		}
		IsRecordable = abRecordable;
	}

}

void Transaction::Rollback()
{
	if (!IsOpen)
	{
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Rollback();
		}
	}
}
