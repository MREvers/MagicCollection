#pragma once
#include<string>
#include<vector>
#include"ListHelper.h"
#include "Config.h"

class TraitItem
{
public:
	TraitItem(std::string aszKeyname,
		std::vector<std::string> alstKeyVals, 
		std::vector<Tag> alstPairedTraits = std::vector<Tag>());
	~TraitItem();
	
	std::string GetKeyName();
	std::string GetDefaultValue();

	bool IsAllowedValue(std::string aszTestVal);

private:
	std::vector<std::string> m_lstPairedValues;
	std::vector<std::string> m_lstPossibleValues;
	std::string m_szKeyName;
};

