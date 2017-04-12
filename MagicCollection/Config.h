#pragma once
#include<string>
#include<vector>

class Config
{
public:
	Config();
	~Config();

	std::string GetKeyCode(std::string aszFullKey);
	std::string GetFullKey(std::string aszKeyCode);

	std::vector<std::pair<std::string, std::string>> GetPairedKeysList();
	std::vector < std::string> GetIdentifyingAttributes();
	std::vector<std::string> GetStaticAttributes();
	std::vector<std::string> GetPerCollectionMetaTags();

	bool IsStaticAttribute(std::string aszAttrs);
	bool IsIdentifyingAttributes(std::string aszAttrs);

	int List_Find(std::string aszFind, std::vector<std::string>& alstFindList);
	int List_Find(std::string aszFind, std::vector<std::pair<std::string, std::string>>& alstFindList);

	static Config* GetConfigClass();

private:
	std::vector<std::pair<std::string, std::string>> ms_lstKeyCodeMappings;
	std::vector<std::pair<std::string, std::string>> ms_lstPairedKeys;
	std::vector<std::string> ms_lstIdentifyingAttributes;
	std::vector<std::string> ms_lstStaticAttributes;
	std::vector<std::string> ms_lstPerCollectionMetaTags;

	static Config* Config::ms_pConfig;
};

