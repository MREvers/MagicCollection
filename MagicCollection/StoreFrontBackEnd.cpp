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

void CStoreFrontBackEnd::AddItem(
	std::string aszCollectionName,
	std::string aszCardNameLong,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	std::string szName;
	int iCount;
	std::string szDetails;
	if (Collection::ParseCardLine(aszCardNameLong, iCount, szName, szDetails))
	{
		if (m_ColFactory->CollectionExists(aszCollectionName) && m_ColSource->LoadCard(szName) != -1)
		{
			Collection* oCol = m_ColFactory->GetCollection(aszCollectionName);

			std::vector<std::pair<std::string, std::string>> lstCardAttrs;
			lstCardAttrs = Collection::ParseAttrs(szDetails);

			oCol->AddItem(szName, true, lstCardAttrs, alstMeta);
		}

	}
}

void CStoreFrontBackEnd::RemoveItem(std::string aszCollectionName,
	std::string aszCardLong,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		m_ColFactory->GetCollection(aszCollectionName)->RemoveItem(aszCardLong, alstMeta);
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

std::vector<std::pair<std::string,std::string>> CStoreFrontBackEnd::GetCardAttributes(std::string aszCardNameLong)
{
	std::vector<std::pair<std::string, std::string>> LstRetVal;
	std::string szName;
	std::string szDetails;
	int iCount;
	if (ParseCardString(aszCardNameLong, iCount, szName, szDetails))
	{
		int iCacheIndex;
		if ((iCacheIndex = m_ColSource->LoadCard(szName)) != -1)
		{
			std::map<std::string, std::string> mapAttrs = 
				m_ColSource->GetCardPrototype(iCacheIndex)->GetAttributesMap();
			std::map<std::string, std::string>::iterator iter_Attrs = mapAttrs.begin();
			for (; iter_Attrs != mapAttrs.end(); ++iter_Attrs)
			{
				LstRetVal.push_back(std::make_pair(iter_Attrs->first, iter_Attrs->second));
			}

			std::vector<std::pair<std::string, std::string>> LstOverwriteAttrs = ParseAttrs(szDetails);
			std::vector<std::pair<std::string, std::string>>::iterator iter_OverAttrs = LstOverwriteAttrs.begin();
			iter_Attrs = mapAttrs.begin();
			for (; iter_OverAttrs != LstOverwriteAttrs.end(); ++iter_OverAttrs)
			{
				for (; iter_Attrs != mapAttrs.end(); ++iter_Attrs)
				{
					if (iter_Attrs->first == iter_OverAttrs->first)
					{
						iter_Attrs->second = iter_OverAttrs->second;
					}
				}
			}
		}
	}
	return LstRetVal;
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


std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
CStoreFrontBackEnd::GetCollectionListWithMeta(std::string aszCollection)
{
	if (m_ColFactory->CollectionExists(aszCollection))
	{
		return m_ColFactory->GetCollection(aszCollection)->GetCollectionListWithMeta();
	}
	else
	{
		std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> lstEmpty;
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

void CStoreFrontBackEnd::AddMetaTag(std::string aszCollectionName, std::string aszLongName, std::string aszKey, std::string aszValue,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		Collection* oCol = m_ColFactory->GetCollection(aszCollectionName);
		oCol->AddMetaTag(aszLongName, aszKey, aszValue, alstMatchMeta);
	}
}

std::vector < std::vector<std::pair<std::string, std::string>>>
CStoreFrontBackEnd::GetMetaTags(std::string aszCollectionName, std::string aszLongName)
{
	if (m_ColFactory->CollectionExists(aszCollectionName))
	{
		Collection* oCol = m_ColFactory->GetCollection(aszCollectionName);
		return oCol->GetMetaTags(aszLongName);
	}

	std::vector < std::vector<std::pair<std::string, std::string>>> lstno;
	return lstno;
}

bool CStoreFrontBackEnd::IsSameCard(std::string aszLongOne, std::string aszLongTwo)
{
	std::string szName;
	int iAmount;
	std::string szDetails;
	if (Collection::ParseCardLine(aszLongOne, iAmount, szName, szDetails))
	{

		std::string szNameTwo;
		int iAmountTwo;
		std::string szDetailsTwo;
		if (Collection::ParseCardLine(aszLongTwo, iAmountTwo, szNameTwo, szDetailsTwo))
		{
			std::vector<std::pair<std::string, std::string>> lstAttrs = Collection::ParseAttrs(szDetails);
			std::vector<std::pair<std::string, std::string>> lstAttrsTwo = Collection::ParseAttrs(szDetailsTwo);
			CopyObject oCop = CollectionObject::GenerateCopy(std::string("None"), lstAttrs);
			CopyObject oCop2 = CollectionObject::GenerateCopy(std::string("None"), lstAttrsTwo);
			if (CollectionObject::IsSameIdentity(&oCop, &oCop2))
			{
				return true;
			}
		}
	}
	return false;
}

void CStoreFrontBackEnd::ImportCollection()
{
	JSONImporter JI;
	JI.ImportJSON("AllSets.json");

}