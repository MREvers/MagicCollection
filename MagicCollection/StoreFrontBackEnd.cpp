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

void CStoreFrontBackEnd::AddItem(std::string aszCollectionName, std::string aszCardName)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		Collection* oCol = m_ColFactory->GetCollection(aszCollectionName);
		std::vector<std::string> lstCardNameAttrs = SourceObject::Str_Split(aszCardName, "{");
		std::string szCardName = lstCardNameAttrs[0];
		std::vector<std::pair<std::string, std::string>> lstCardAttrs;
		if (lstCardNameAttrs.size() > 1)
		{
			lstCardAttrs = Collection::ParseAttrs(lstCardNameAttrs[1]);
		}
		oCol->AddItem(aszCardName, true, lstCardAttrs);
	}
}

void CStoreFrontBackEnd::RemoveItem(std::string aszCollectionName, std::string aszCardName)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		
	}
}

void CStoreFrontBackEnd::SaveCollection(std::string aszCollectionName)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		Collection* oCol = m_ColFactory->GetCollection(aszCollectionName);
		oCol->SaveCollection(oCol->GetName() + ".txt");
	}
}

std::string CStoreFrontBackEnd::LoadCollection(std::string aszCollection)
{
	// Check if the file exists.
	std::ifstream f(aszCollection.c_str());
	if (f.good())
	{
		// If it does, continue.
		return m_ColFactory->LoadCollection("Name Not Found", aszCollection)->GetName();
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