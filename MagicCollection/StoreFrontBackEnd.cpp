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
	if (m_ColFactory->CollectionExists(aszCollectionName) && m_ColSource->LoadCard(aszCardName) != -1)
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

std::vector<std::string> CStoreFrontBackEnd::GetLoadedCollections()
{
	return m_ColFactory->LoadedCollections;
}

std::vector<std::pair<std::string, std::string>> CStoreFrontBackEnd::ParseAttrs(std::string aszDetails)
{
	return Collection::ParseAttrs(aszDetails);
}

bool CStoreFrontBackEnd::ParseCardString(std::string aszCardString, int& riCount, std::string& rszName, std::string& rszDetails)
{
	int iNum = 0;
	std::string szName = "";
	std::string szDetails = "";
	bool bRetVal = Collection::ParseCardLine(aszCardString, iNum, szName, szDetails);

	riCount = iNum;
	rszName = szName;
	rszDetails = szDetails;

	return bRetVal;
}

std::vector<std::string> CStoreFrontBackEnd::GetAllCardsStartingWith(std::string aszText)
{
	return m_ColSource->GetAllCardsStartingWith(aszText);
}

int CStoreFrontBackEnd::CountCopiesInCollection(std::string aszCollection, std::string aszLongCardName)
{
	std::string szName;
	int iCount;
	std::string szDetails;
	if (ParseCardString(aszLongCardName, iCount, szName, szDetails))
	{
		int iCacheIndex;
		if (iCacheIndex = m_ColSource->LoadCard(aszLongCardName) != -1)
		{
			CollectionObject* oCO = m_ColSource->GetCardPrototype(iCacheIndex);
			return oCO->GetLocalCopiesWith(aszCollection, ParseAttrs(szDetails)).size();
		}
			
	}
	return 0;
}