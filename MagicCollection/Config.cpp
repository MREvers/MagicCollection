#include "Config.h"

/*
const char * const CollectionObject::LstUniqueTraits[] = { "manaCost", "colors", "name", "power",
"toughness", "loyalty", "text" };

const char * const CollectionObject::LstNonUniqueTraits[] = { "set", "multiverseid" };
*/

Config* Config::ms_pConfig = nullptr;

Config::Config()
{
	m_lstStaticAttributes.push_back("manaCost");
	m_lstStaticAttributes.push_back("colors");
	m_lstStaticAttributes.push_back("name");
	m_lstStaticAttributes.push_back("power");
	m_lstStaticAttributes.push_back("toughness");
	m_lstStaticAttributes.push_back("loyalty");
	m_lstStaticAttributes.push_back("text");

	m_lstIdentifyingAttributes.push_back("set");
	m_lstIdentifyingAttributes.push_back("multiverseid");

	m_lstPairedKeys.push_back(std::make_pair("set", "multiverseid"));

	m_lstKeyCodeMappings.push_back(std::make_pair("set", "set"));
	m_lstKeyCodeMappings.push_back(std::make_pair("multiverseid", "mid"));
	m_lstKeyCodeMappings.push_back(std::make_pair("manaCost", "man"));
	m_lstKeyCodeMappings.push_back(std::make_pair("colors", "clr"));
	m_lstKeyCodeMappings.push_back(std::make_pair("name", "nam"));
	m_lstKeyCodeMappings.push_back(std::make_pair("toughness", "tuf"));
	m_lstKeyCodeMappings.push_back(std::make_pair("loyalty", "loy"));
	m_lstKeyCodeMappings.push_back(std::make_pair("text", "txt"));
	m_lstKeyCodeMappings.push_back(std::make_pair("cmc", "cmc"));
	m_lstKeyCodeMappings.push_back(std::make_pair("colorIdentity", "cid"));
	m_lstPerCollectionMetaTags.push_back("Generalization");
}


Config::~Config()
{
}

std::string Config::GetKeyCode(std::string aszFullKey)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyMappings = m_lstKeyCodeMappings.begin();
	for (; iter_KeyMappings != m_lstKeyCodeMappings.end(); ++iter_KeyMappings)
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
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyMappings = m_lstKeyCodeMappings.begin();
	for (; iter_KeyMappings != m_lstKeyCodeMappings.end(); ++iter_KeyMappings)
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
	return m_lstPairedKeys;
}
std::vector < std::string> Config::GetIdentifyingAttributes()
{
	return m_lstIdentifyingAttributes;
}
std::vector<std::string> Config::GetStaticAttributes()
{
	return m_lstStaticAttributes;
}

std::vector<std::string> Config::GetPerCollectionMetaTags()
{
	return m_lstPerCollectionMetaTags;
}

bool Config::IsIdentifyingAttributes(std::string aszAttrs)
{
	return List_Find(aszAttrs, m_lstIdentifyingAttributes) != -1;
}

bool Config::IsStaticAttribute(std::string aszAttr)
{
	return List_Find(aszAttr, m_lstStaticAttributes) != -1;
}

int Config::List_Find(int aiFind, std::vector<int>& alstFindList)
{
	std::vector<int>::iterator iter_list = alstFindList.begin();
	int index = 0;
	for (; iter_list != alstFindList.end(); iter_list++)
	{
		if (*iter_list == aiFind)
		{
			return index;
		}
		index++;
	}
	return -1;
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