
#include "CollectionFactory.h"


CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
	m_ColSource = aoColSource;
	m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

std::string CollectionFactory::LoadCollectionFromFile(std::string aszFileName)
{
	std::vector<std::pair<std::string, std::string>> lstForcedChanges;
	std::pair<std::string,std::string> pairColNameParent = GetCollectionNameAndParentFromFile(aszFileName);
	std::string szColName = pairColNameParent.first;
	std::string szParentName = pairColNameParent.second;
	if (!CollectionExists(szColName) && szColName != "")
	{
		ItemCollection* oCol = FindOrGenerateCollection(szColName);
		oCol->LoadCollection(aszFileName);
		return szColName;
	}
	return nullptr;
}

std::vector<std::string> CollectionFactory::GetLoadedCollections()
{
	std::vector<std::string> lstRetval;
	auto iter_Colo = m_lstCollections.begin();
	for (; iter_Colo != m_lstCollections.end(); ++iter_Colo)
	{
		lstRetval.push_back(iter_Colo->GetName());
	}
	return lstRetval;
}

ItemCollection* CollectionFactory::FindOrGenerateCollection(std::string aszCollectionName, std::string aszParent)
{
	// Check if we have the collection already.
	std::vector<ItemCollection>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == iter_cols->GetName())
		{
			return &(*iter_cols);
		}
	}

	// If not, create one.
	if (aszParent != "")
	{
		ItemCollection oCol(aszCollectionName, m_ColSource);
		m_lstCollections.push_back(oCol);
	}
	else
	{
		ItemCollection oCol(aszCollectionName, m_ColSource);
		m_lstCollections.push_back(oCol);
	}

	return &m_lstCollections.at(m_lstCollections.size() - 1);
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
	std::vector<ItemCollection>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == iter_cols->GetName())
		{
			return true;
		}
	}

	return false;
}

std::pair<std::string, std::string>  CollectionFactory::GetCollectionNameAndParentFromFile(std::string aszCollectionFileName)
{
	// Get the first line of the collection
	std::ifstream in(aszCollectionFileName);
	if (in.good())
	{
		std::stringstream buffer;
		buffer << in.rdbuf();
		std::string contents(buffer.str());

		int iLinesNeeded = 2; // Name and Parent
		int iFoundLines = 0;
		std::vector<std::string> lstLines;
		std::string szLine = "";
		for (unsigned int i = 0; i < contents.size(); i++)
		{

			if (contents[i] == '\n')
			{
				lstLines.push_back(szLine);
				szLine = "";

				iFoundLines++;
				if (iLinesNeeded == iFoundLines)
				{
					break;
				}
			}
			else
			{
				szLine = szLine + contents[i];
				if (i == contents.size() - 1)
				{
					lstLines.push_back(szLine);
					szLine = "";
				}
			}
		}

		std::string szColName = "";
		std::string szParentName = "";

		std::string szColNameLine = lstLines[0];
		std::string szParentNameLine = "";

		if (lstLines.size() > 1)
		{
			szParentNameLine = lstLines[1];
		}
		
		// Get the name of the collection
		if (szColNameLine.size() > 0 && szColNameLine.at(0) == ':')
		{
			std::string szNoColon = StringHelper::Str_Split(szColNameLine, ":")[1];
			std::vector<std::string> lstPair = StringHelper::Str_Split(szColNameLine, "=");
			szColName = StringHelper::Str_Split(lstPair[1], "\"")[1];
		}

		if (szParentNameLine.size() > 0 && szParentNameLine.at(0) == ':')
		{
			std::string szNoColon = StringHelper::Str_Split(szParentNameLine, ":")[1];
			std::vector<std::string> lstPair = StringHelper::Str_Split(szParentNameLine, "=");
			szParentName = StringHelper::Str_Split(lstPair[1], "\"")[1];
		}

		if (szColName != "")
		{
			return std::make_pair(szColName, szParentName);
		}
	}

	return std::make_pair("","");
}