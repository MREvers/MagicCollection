#include "TraitItem.h"



TraitItem::TraitItem(std::string aszKeyname,
	std::vector<std::string> alstKeyVals,
	std::vector<Tag> alstPairedTraits)
{
	m_szKeyName = aszKeyname;
	m_lstPossibleValues = alstKeyVals;

	if (m_lstPossibleValues.size() == 0)
	{
		m_lstPossibleValues.push_back("");
	}

	std::string szKeyCode = Config::Instance()->GetKeyCode(aszKeyname);
	std::vector<Tag>::iterator iter_Tags = alstPairedTraits.begin();
	for (; iter_Tags != alstPairedTraits.end(); ++iter_Tags)
	{
		if (szKeyCode == iter_Tags->first)
		{
			m_lstPairedValues.push_back(Config::Instance()->GetFullKey(iter_Tags->first));
		}
		else if (szKeyCode == iter_Tags->second)
		{
			m_lstPairedValues.push_back(Config::Instance()->GetFullKey(iter_Tags->second));
		}
	}
	
}


TraitItem::~TraitItem()
{
}

std::string TraitItem::GetKeyName()
{
	return m_szKeyName;
}

std::string TraitItem::GetDefaultValue()
{
	return m_lstPossibleValues[0];
}

std::vector<std::string> TraitItem::GetAllowedValues()
{
	return m_lstPossibleValues;
}

bool TraitItem::IsAllowedValue(std::string aszTestVal)
{
	return ListHelper::List_Find(aszTestVal, m_lstPossibleValues) != -1;
}