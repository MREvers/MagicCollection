
#include "CollectionSource.h"
CollectionSource::CollectionSource()
{
	m_lstNonUniqueKeys.push_back("set");
}

CollectionSource::~CollectionSource()
{
}

void CollectionSource::LoadLib(std::string aszFileName)
{
	m_lstptCardBuffer.clear();
	m_lstptCardBuffer.reserve(17000);

	rapidxml::xml_document<> doc;
	std::ifstream file(aszFileName);
	if (!file.good())
	{
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::cout << "Parsing Doc" << std::endl;
	std::string content(buffer.str());
	doc.parse<0>(&content[0]);
	std::cout << "Parse Done" << std::endl;

	// Mechanisms
	clock_t begin = clock();

	rapidxml::xml_node<> *xmlNode_CardDatabase = doc.first_node();
	// With the xml example above this is the <document/> node
	rapidxml::xml_node<> *xmlNode_Cards = xmlNode_CardDatabase->first_node("cards");
	rapidxml::xml_node<> *xmlNode_Card = xmlNode_Cards->first_node();

	while (xmlNode_Card != 0)
	{
		rapidxml::xml_node<> *xmlNode_CardAttribute = xmlNode_Card->first_node();

		rapidxml::xml_node<> *xmlNode_CardName = xmlNode_Card->first_node("name");

		SourceObject* sO = new SourceObject(xmlNode_CardName->value());
		m_lstptCardBuffer.push_back(*sO);
		delete sO;
		sO = &m_lstptCardBuffer[m_lstptCardBuffer.size() - 1];

		bool bHasAll = false;

		while (xmlNode_CardAttribute != 0)
		{
			std::string szCardKey = xmlNode_CardAttribute->name();
			if (isUnique(szCardKey))
			{
				sO->AddAttribute(szCardKey, xmlNode_CardAttribute->value());
			}
			else
			{
				sO->AddNonUniqueAttribute(szCardKey, xmlNode_CardAttribute->value());//->AddAttribute(szCardKey, xmlNode_CardAttribute->value());
			}


			xmlNode_CardAttribute = xmlNode_CardAttribute->next_sibling();
		}

		xmlNode_Card = xmlNode_Card->next_sibling();
	}

	// Mechanisms
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Load Time: " << elapsed_secs << std::endl;

	std::cout << "Load Done" << std::endl;
}

int CollectionSource::LoadCard(std::string aszCardName)
{
	int iCacheLocation = -1;
	std::string szCardName = CollectionObject::str_trim(aszCardName, ' ');
	int iFound = findInBuffer(szCardName, false);
	if (iFound != -1)
	{
		SourceObject* oSource = &m_lstptCardBuffer.at(iFound);

		iCacheLocation = oSource->GetCacheIndex();
		if (iCacheLocation == -1)
		{
			// The card is already cached.
			CollectionObject oCard(aszCardName);

			std::map<std::string, std::string>::iterator att_iter = oSource->Attributes.begin();
			bool bHasAllAttributes = false;
			for (; att_iter != oSource->Attributes.end() && !bHasAllAttributes; ++att_iter)
			{
				bHasAllAttributes = oCard.MapAttributes(att_iter->first, att_iter->second);
			}

			// Store the location of the CollectionObject in the cache
			iCacheLocation = m_lstoCardCache.size();
			oSource->Cache(iCacheLocation);

			// Cache the collectionobject
			m_lstoCardCache.push_back(oCard);
		}
	}

	return iCacheLocation;
}

CollectionObject* CollectionSource::GetCardPrototype(int aiCacheIndex)
{
	if (aiCacheIndex < m_lstoCardCache.size())
	{
		return &m_lstoCardCache.at(aiCacheIndex);
	}
	else
	{
		return NULL;
	}
}

std::vector<std::pair<std::string, CopyObject*>> CollectionSource::GetCollection(std::string aszCollectionName)
{
	std::vector<std::pair<std::string, CopyObject*>>  lstCopies;
	std::vector<CollectionObject>::iterator iter_colObjs = m_lstoCardCache.begin();
	for (; iter_colObjs != m_lstoCardCache.end(); ++iter_colObjs)
	{
		std::vector<CopyObject*> lstFoundCopies = iter_colObjs->GetLocalCopies(aszCollectionName);
		std::vector<CopyObject*>::iterator iter_copies = lstFoundCopies.begin();
		for (; iter_copies != lstFoundCopies.end(); ++iter_copies)
		{
			lstCopies.push_back(std::make_pair(iter_colObjs->GetName(), *iter_copies));
		}
	}

	return lstCopies;
}

std::vector<std::string> CollectionSource::GetAllCardsStartingWith(std::string aszText)
{
	std::vector<std::string> lstCards;
	std::vector<std::string> lstStartCards;
	std::vector<std::string> lstOthers;

	std::vector<SourceObject>::iterator iter_Cards = m_lstptCardBuffer.begin();
	for (; iter_Cards != m_lstptCardBuffer.end(); ++iter_Cards)
	{
		std::string szCard = iter_Cards->GetName();
		std::transform(szCard.begin(), szCard.end(), szCard.begin(), ::tolower);
		size_t iFindIndex = 0;
		iFindIndex = szCard.find(aszText);
		if (iFindIndex != std::string::npos)
		{
			if (iFindIndex == 0)
			{
				lstStartCards.push_back(iter_Cards->GetName());
			}
			else
			{
				lstOthers.push_back(iter_Cards->GetName());
			}
		}
	}

	for (std::string sz : lstStartCards)
	{
		lstCards.push_back(sz);
	}
	for (std::string sz : lstOthers)
	{
		lstCards.push_back(sz);
	}


	return lstCards;
}

int CollectionSource::findInBuffer(std::string aszCardName, bool abCaseSensitive)
{
	std::string szCardNameFixed = aszCardName;
	if (abCaseSensitive)
	{
		std::transform(szCardNameFixed.begin(), szCardNameFixed.end(), szCardNameFixed.begin(), ::tolower);
	}

	int iSize = m_lstptCardBuffer.size();
	int iLeft = 0;
	int iRight = iSize;
	if (iRight < 1)
	{
		return -1;
	}

	std::string szName;
	while (iLeft <= iRight)
	{
		int middle = (iLeft + iRight) / 2;

		if (middle < 0 || middle >= iSize)
		{
			return -1;
		}

		szName = m_lstptCardBuffer.at(middle).GetName();
		if (szName == szCardNameFixed)
			return middle;
		else if (szCardNameFixed.compare(szName) < 0)
			iRight = middle - 1;
		else
			iLeft = middle + 1;
	}

	return -1;
}

bool CollectionSource::isUnique(std::string aszFind)
{
	for (auto sz : m_lstNonUniqueKeys)
	{
		if (aszFind == sz)
		{
			return false;
		}
	}
	return true;
}