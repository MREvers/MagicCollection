#include "StoreFrontBackEnd.h"


CStoreFrontBackEnd::CStoreFrontBackEnd()
{
	// No Server for now
	m_ColSource = new CollectionSource();
	m_ColSource->LoadLib(Config::Instance()->GetSourceFile());

	m_ColFactory = new CollectionFactory(m_ColSource);
}


CStoreFrontBackEnd::~CStoreFrontBackEnd()
{
}

std::string CStoreFrontBackEnd::LoadCollection(std::string aszCollectionFile)
{
	return m_ColFactory->LoadCollectionFromFile(aszCollectionFile)->GetName();
}

std::vector<std::string> CStoreFrontBackEnd::GetLoadedCollections()
{
	return m_ColFactory->GetLoadedCollections();
}

std::vector<std::string>
CStoreFrontBackEnd::GetCollectionList(std::string aszCollection)
{
	std::vector<std::string> lstList;
	if (m_ColFactory->CollectionExists(aszCollection))
	{
		return m_ColFactory->FindOrGenerateCollection(aszCollection)->GetCollectionList();
	}
	else
	{
		std::vector<std::string> lstEmpty;
		return lstEmpty;
	}
}

std::vector<std::string> CStoreFrontBackEnd::GetAllCardsStartingWith(std::string aszSearch)
{
	return m_ColSource->GetAllCardsStartingWith(aszSearch);
}

std::string CStoreFrontBackEnd::GetImagesPath()
{
	return Config::Instance()->GetImagesFolder();
}