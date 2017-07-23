
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

	// Get the file name.
	std::vector<std::string> lstSplitFile = StringHelper::Str_Split(aszFileName, "\\");
	std::string szFile = lstSplitFile[lstSplitFile.size() - 1];
	std::vector<std::string> lstSplitExt = StringHelper::Str_Split(szFile, ".");
	szFile = lstSplitExt[0];

	Collection* oCol = new Collection(Config::NotFoundString, m_ColSource, szFile);
	oCol->LoadCollection(aszFileName, this);
	std::string szFoundName = oCol->GetName();

	if (oCol->IsLoaded && !CollectionExists(szFoundName))
	{
		m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
		szRetVal = szFoundName;

		// Check if this collection is a parent of another. If so, then those cols need this collection.
		for each (std::shared_ptr<Collection> col in m_lstCollections)
		{
			if (col->GetParent() == szFoundName)
			{
				col->RegisterParent(std::shared_ptr<Collection>(oCol));
			}
		}
	}
	else
	{
		delete oCol;
	}

	return szRetVal;
}

std::string CollectionFactory::CreateNewCollection(std::string aszColName, std::string aszParent)
{
	if (!CollectionExists(aszColName))
	{
		// The parent is required to be loaded to have it as a parent
		if (aszParent != "" && !CollectionExists(aszParent)) { aszParent = ""; }
		Collection* oCol = new Collection(aszColName, m_ColSource, aszColName, aszParent);
		m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
		return oCol->GetName();
	}

	return Config::NotFoundString;
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
	std::vector<std::shared_ptr<Collection>>::iterator iter_cols = m_lstCollections.begin();
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
	std::vector<std::shared_ptr<Collection>>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetName())
		{
			return iter_cols->get();
		}
	}
	return nullptr;
}
