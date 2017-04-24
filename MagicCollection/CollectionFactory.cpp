
#include "CollectionFactory.h"


CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
	m_ColSource = aoColSource;
	m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

Collection* CollectionFactory::LoadCollection(std::string aszCollectionName, std::string aszFileName)
{

	std::vector<std::pair<std::string, std::string>> lstForcedChanges;
	std::string szColName = GetCollectionNameFromFile(aszFileName);
	if (!CollectionExists(szColName))
	{
		Collection* oCol = GetCollection(szColName);
		oCol->LoadCollection(aszFileName, lstForcedChanges);

		std::vector<std::pair<std::string, std::string>>::iterator iter_change = lstForcedChanges.begin();
		for (; iter_change != lstForcedChanges.end(); ++iter_change)
		{
			if (CollectionExists(iter_change->first))
			{
				Collection* oChangedCol = GetCollection(iter_change->first);
				oChangedCol->RecordForcedTransaction(iter_change->second);
			}
		}
		return oCol;

	}

	if (szColName == "")
	{
		return GetCollection("ErrorLoadingCollection");
	}

	return GetCollection(szColName);
}

Collection* CollectionFactory::GetCollection(std::string aszCollectionName)
{
	// Check if we have the collection already.
	std::vector<Collection>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == iter_cols->GetName())
		{
			return &(*iter_cols);
		}
	}

	// If not, create one.
	Collection oCol(aszCollectionName, m_ColSource, &LoadedCollections);
	m_lstCollections.push_back(oCol);
	LoadedCollections.push_back(aszCollectionName);

	return &m_lstCollections.at(m_lstCollections.size() - 1);
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
	std::vector<Collection>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == iter_cols->GetName())
		{
			return true;
		}
	}

	return false;
}

std::string CollectionFactory::GetCollectionNameFromFile(std::string aszCollectionFileName)
{
	// Get the first line of the collection
	std::ifstream in(aszCollectionFileName);
	if (in.good())
	{
		std::stringstream buffer;
		buffer << in.rdbuf();
		std::string contents(buffer.str());

		std::vector<std::string> lstLines;
		std::string szLine = "";
		for (int i = 0; i < contents.size(); i++)
		{

			if (contents[i] == '\n')
			{
				lstLines.push_back(szLine);
				szLine = "";
				break;
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

		// Get the name of the collection
		if (lstLines[0].size() > 0 && lstLines[0].at(0) == ':')
		{
			std::string szPreprocess = "";
			std::string szNoColon = StringHelper::Str_Split(lstLines[0], ":")[1];
			std::vector<std::string> lstPair = StringHelper::Str_Split(lstLines[0], "=");
			std::string szColName = StringHelper::Str_Split(lstPair[1], "\"")[1];
			return szColName;
		}
	}

	return "";
}