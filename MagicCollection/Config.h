#pragma once
#include<string>
#include<vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "MD5.h"

typedef std::pair<std::string, std::string> Tag;

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

	std::string GetHash(std::string& aszHashingString);

	std::vector<Tag>& GetPairedKeysList();
	std::vector<std::string>& GetIdentifyingAttributes();
	std::vector<std::string>& GetStaticAttributes();
	std::vector<std::string>& GetPerCollectionMetaTags();

	bool IsPairedKey(std::string aszKey);
	bool IsValidKey(std::string aszKey);
	bool IsStaticAttribute(std::string aszAttrs);
	bool IsIdentifyingAttributes(std::string aszAttrs);

	template<typename T>
	int List_Find(T aiFind, std::vector<T>& alstFindList);
	int List_Find(std::string aszFind, std::vector<std::pair<std::string, std::string>>& alstFindList);

	template<class T, class R>
	int List_Find(T& aiFind, std::vector<R>& alstFindList, std::function<T (R)> afuncExtractor)
	{
		std::vector<R>::iterator iter_list = alstFindList.begin();
		int index = 0;
		for (; iter_list != alstFindList.end(); iter_list++)
		{
			if (aiFind == afuncExtractor(*iter_list))
			{
				return index;
			}
			index++;
		}
		return -1;
	}

	template<class T> inline
	void List_Insert(T& aInsert, std::vector<T>& alstInsertList, std::function<int(T, T)> afuncComparer)
	{
		std::vector<T>::iterator iter = alstInsertList.begin();
		for (; iter != alstInsertList.end(); ++iter)
		{
			int iCmp = afuncComparer(aInsert, *iter);
			if (iCmp >= 0) { break; }
		}

		alstInsertList.insert(iter, aInsert);
	}

	static Config* Instance();

private:
	std::vector<Tag> m_lstKeyCodeMappings;
	std::vector<Tag> m_lstPairedKeys;
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

template int Config::List_Find<int>(int aiFind, std::vector<int>& alstFind);
template int Config::List_Find<std::string>(std::string aiFind, std::vector<std::string>& alstFind);