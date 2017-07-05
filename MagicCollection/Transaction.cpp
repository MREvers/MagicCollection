#include "Transaction.h"

Transaction::Transaction(Collection* aoCol)
{
	m_Col = aoCol;
	m_bIsRecordable = true;

}

Transaction::~Transaction()
{
	Actions.clear();
}

void Transaction::AddAction(Action& aoAct)
{
	if (m_bIsOpen)
	{
		Actions.push_back(aoAct);
	}
}

void Transaction::RemoveAction(int i)
{
	if (m_bIsOpen)
	{
		Actions.erase(Actions.begin() + i);
	}
}

void Transaction::Finalize(bool abRecordable)
{
	if (m_bIsOpen)
	{
		m_bIsOpen = false;
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Execute();
		}
		m_bIsRecordable = abRecordable;
	}

}

void Transaction::Rollback()
{
	if (!m_bIsOpen)
	{
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Rollback();
		}
		m_bIsOpen = true;
	}
}

std::vector<std::string> Transaction::GetDescriptions()
{
	std::vector<std::string> lstRetVal;

	if (m_bIsRecordable)
	{
		for (size_t i = 0; i < Actions.size(); i++)
		{
			lstRetVal.push_back(Actions[i].GetIdentifier());
		}
	}

	return lstRetVal;
}

bool Transaction::IsOpen()
{
	return m_bIsOpen;
}

bool Transaction::IsRecordable()
{
	return m_bIsRecordable;
}