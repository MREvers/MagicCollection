#pragma once
#include<string>
#include<vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"

class Config
{
public:
	Config();
	~Config();

	std::string GetImportSourceFile();
	std::string GetSourceFile();
	std::string GetHistoryFolderName();
	std::string GetMetaFolderName();
	std::string GetImagesFolder();
	
	std::string GetCollectionsDirectory();
	std::string GetCollectionsFolderName();

	std::string GetKeyCode(std::string aszFullKey);
	std::string GetFullKey(std::string aszKeyCode);

	std::vector<std::pair<std::string, std::string>>& GetPairedKeysList();
	std::vector < std::string>& GetIdentifyingAttributes();
	std::vector<std::string>& GetStaticAttributes();
	std::vector<std::string>& GetPerCollectionMetaTags();

	bool IsPairedKey(std::string aszKey);
	bool IsValidKey(std::string aszKey);
	bool IsStaticAttribute(std::string aszAttrs);
	bool IsIdentifyingAttributes(std::string aszAttrs);

	int List_Find(int aiFind, std::vector<int>& alstFindList);
	int List_Find(std::string aszFind, std::vector<std::string>& alstFindList);
	int List_Find(std::string aszFind, std::vector<std::pair<std::string, std::string>>& alstFindList);

	static Config* GetConfigClass();

private:
	std::vector<std::pair<std::string, std::string>> m_lstKeyCodeMappings;
	std::vector<std::pair<std::string, std::string>> m_lstPairedKeys;
	std::vector<std::string> m_lstIdentifyingAttributes;
	std::vector<std::string> m_lstStaticAttributes;
	std::vector<std::string> m_lstPerCollectionMetaTags;

	std::string m_szImportSource;
	std::string m_szSourceFile;
	std::string m_szSourceFolder;

	std::string m_szCollectionsFolder;
	std::string m_szHistoryFolder;
	std::string m_szMetaFolder;

	std::string m_szImagesFolder;

	void initDefaultSettings();
	void initConfigSettings(std::ifstream& asConfig);

	static Config* Config::ms_pConfig;
};

