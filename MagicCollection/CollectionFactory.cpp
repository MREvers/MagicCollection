
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
	ItemCollection* oCol = new ItemCollection(Config::NotFoundString, m_ColSource);
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
	std::vector<ItemCollection*>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetName())
		{
			return true;
		}
	}
	return false;
}

ItemCollection* CollectionFactory::GetCollection(std::string aszCollectionName)
{
	std::vector<ItemCollection*>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetName())
		{
			return *iter_cols;
		}
	}
	return nullptr;
}