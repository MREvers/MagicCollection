#include "Config.h"

/*
const char * const CollectionObject::LstUniqueTraits[] = { "manaCost", "colors", "name", "power",
"toughness", "loyalty", "text" };

const char * const CollectionObject::LstNonUniqueTraits[] = { "set", "multiverseid" };
*/

Config* Config::ms_pConfig = nullptr;

Config::Config()
{
	ms_lstStaticAttributes.push_back("manaCost");
	ms_lstStaticAttributes.push_back("colors");
	ms_lstStaticAttributes.push_back("name");
	ms_lstStaticAttributes.push_back("power");
	ms_lstStaticAttributes.push_back("toughness");
	ms_lstStaticAttributes.push_back("loyalty");
	ms_lstStaticAttributes.push_back("text");

	ms_lstIdentifyingAttributes.push_back("set");
	ms_lstIdentifyingAttributes.push_back("multiverseid");

	ms_lstPairedKeys.push_back(std::make_pair("set","multiverseid"));

	ms_lstKeyCodeMappings.push_back(std::make_pair("set","set"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("multiverseid", "mid"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("manaCost", "man"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("colors", "clr"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("name", "nam"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("toughness", "tuf"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("loyalty", "loy"));
	ms_lstKeyCodeMappings.push_back(std::make_pair("text", "txt"));
}


Config::~Config()
{
}

std::string Config::GetKeyCode(std::string aszFullKey)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyMappings = ms_lstKeyCodeMappings.begin();
	for (; iter_KeyMappings != ms_lstKeyCodeMappings.end(); ++iter_KeyMappings)
	{
		if (iter_KeyMappings->first == aszFullKey)
		{
			return iter_KeyMappings->second;
		}
	}

	return "";
}
std::string Config::GetFullKey(std::string aszKeyCode)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyMappings = ms_lstKeyCodeMappings.begin();
	for (; iter_KeyMappings != ms_lstKeyCodeMappings.end(); ++iter_KeyMappings)
	{
		if (iter_KeyMappings->second == aszKeyCode)
		{
			return iter_KeyMappings->first;
		}
	}

	return "";
}

std::vector<std::pair<std::string, std::string>> Config::GetPairedKeysList()
{
	return ms_lstPairedKeys;
}
std::vector < std::string> Config::GetIdentifyingAttributes()
{
	return ms_lstIdentifyingAttributes;
}
std::vector<std::string> Config::GetStaticAttributes()
{
	return ms_lstStaticAttributes;
}

bool Config::IsIdentifyingAttributes(std::string aszAttrs)
{
	return List_Find(aszAttrs, ms_lstIdentifyingAttributes) != -1;
}

bool Config::IsStaticAttribute(std::string aszAttr)
{
	return List_Find(aszAttr, ms_lstStaticAttributes) != -1;
}

int Config::List_Find(std::string aszFind, std::vector<std::string>& alstFindList)
{
	std::vector<std::string>::iterator iter_list = alstFindList.begin();
	int index = 0;
	for (; iter_list != alstFindList.end(); iter_list++)
	{
		if (*iter_list == aszFind)
		{
			return index;
		}
		index++;
	}
	return -1;
}

int Config::List_Find(std::string aszFind, std::vector<std::pair<std::string, std::string>>& alstFindList)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_list = alstFindList.begin();
	int index = 0;
	for (; iter_list != alstFindList.end(); iter_list++)
	{
		if (iter_list->first == aszFind)
		{
			return index;
		}
		index++;
	}
	return -1;
}

Config* Config::GetConfigClass()
{
	if (ms_pConfig == nullptr)
	{
		ms_pConfig = new Config();
	}

	return ms_pConfig;
}