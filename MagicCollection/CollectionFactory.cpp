
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
	std::string szFoundID = oCol->GetIdentifier();

	if (oCol->IsLoaded && !CollectionExists(szFoundID))
	{
		m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
		szRetVal = oCol->GetIdentifier();
	}
	else
	{
		delete oCol;
	}

	return szRetVal;
}

std::string CollectionFactory::CreateNewCollection(std::string aszColName, std::string aszParentID)
{
	Collection* oCol;
	// The parent is required to be loaded to have it as a parent
	if (CollectionExists(aszParentID))
	{
		oCol = new Collection(aszColName, m_ColSource, aszColName, getNextChildName(aszParentID));
	}
	else
	{
		oCol = new Collection(aszColName, m_ColSource, aszColName);
	}

	m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
	return oCol->GetName();
}

std::vector<std::string> CollectionFactory::GetLoadedCollections()
{
	std::vector<std::string> lstRetval;
	auto iter_Colo = m_lstCollections.begin();
	for (; iter_Colo != m_lstCollections.end(); ++iter_Colo)
	{
		lstRetval.push_back((*iter_Colo)->GetIdentifier());
	}
	return lstRetval;
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
	std::vector<std::shared_ptr<Collection>>::iterator iter_cols = m_lstCollections.begin();
	for (; iter_cols != m_lstCollections.end(); ++iter_cols)
	{
		if (aszCollectionName == (*iter_cols)->GetIdentifier())
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
		if (aszCollectionName == (*iter_cols)->GetIdentifier())
		{
			return iter_cols->get();
		}
	}
	return nullptr;
}

std::string CollectionFactory::getNextChildName(std::string aszParentID)
{
	int iID;
	std::vector<std::string> lstUIandPF = StringHelper::Str_Split(aszParentID, std::string("-"));
	if (lstUIandPF.size() == 1)
	{
		// The first child gets 2; the first prime.
		iID = 2;
	}
	else
	{
		iID = std::stoi(lstUIandPF[1]);
	}

	int iNextPrime = 1;
	// Find the largest prime number divisor
	for each (int iPrime in Config::primes)
	{
		if (iID % iPrime != 0)
		{
			iNextPrime = iPrime;
			break;
		}
	}
	int iChild = GetCollection(aszParentID)->ChildCount();
	std::string szRetval = lstUIandPF[0] + "-" + std::to_string(iID*(iNextPrime^iChild));
	return szRetval;
}