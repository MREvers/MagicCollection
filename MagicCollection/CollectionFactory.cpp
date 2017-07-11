
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
	std::string szRetVal = Config::NotFoundString;
	Collection* oCol = new Collection(Config::NotFoundString, m_ColSource, aszFileName);
	oCol->LoadCollection(aszFileName);
	std::string szFoundName = oCol->GetName();
	if (oCol->IsLoaded && !CollectionExists(szFoundName))
	{
		m_lstCollections.push_back(oCol);
		szRetVal = szFoundName;
	}
	else
	{
		delete oCol;
	}

	return szRetVal;
}

std::string CollectionFactory::CreateNewCollection(std::string aszColName)
{
	if (!CollectionExists(aszColName))
	{
		Collection* oCol = new Collection(aszColName, m_ColSource, aszColName);
		m_lstCollections.push_back(oCol);
	}

	return aszColName;
}

std::vector<std::string> CollectionFactory::GetLoadedCollections()
{
	std::vector<std::string> lstRetval;
	auto iter_Colo = m_lstCollections.begin();
	for (; iter_Colo != m_lstCollections.end(); ++iter_Colo)
	{
		lstRetval.push_back((*iter_Colo)->GetName());
	}
	return lstRetval;
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
	std::vector<Collection*>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetName())
		{
			return true;
		}
	}
	return false;
}

Collection* CollectionFactory::GetCollection(std::string aszCollectionName)
{
	std::vector<Collection*>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetName())
		{
			return *iter_cols;
		}
	}
	return nullptr;
}