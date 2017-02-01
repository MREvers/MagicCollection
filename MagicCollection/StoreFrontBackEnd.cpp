#include "StoreFrontBackEnd.h"


CStoreFrontBackEnd::CStoreFrontBackEnd()
{
	// No Server for now
	m_ColSource = new CollectionSource();
	m_ColSource->LoadLib("AllSets.json.out");

	m_ColFactory = new CollectionFactory(m_ColSource);
}


CStoreFrontBackEnd::~CStoreFrontBackEnd()
{
}

void CStoreFrontBackEnd::LoadCollection(std::string aszCollection)
{
	// Check if the file exists.
	std::ifstream f(aszCollection.c_str());
	if (f.good())
	{
		// If it does, continue.
		m_ColFactory->LoadCollection("Name Not Found", aszCollection);
	}
}

std::vector<std::string> CStoreFrontBackEnd::GetCollectionList(std::string aszCollection)
{
	if (m_ColFactory->CollectionExists(aszCollection))
	{
		return m_ColFactory->GetCollection(aszCollection)->GetCollectionList();
	}
	else
	{
		std::vector<std::string> lstEmpty;
		return lstEmpty;
	}
}