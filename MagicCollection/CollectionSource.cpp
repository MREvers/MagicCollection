
#include "CollectionSource.h"
CollectionSource::CollectionSource()
{
	m_lstNonUniqueKeys.push_back("set");
	m_lstNonUniqueKeys.push_back("multiverseid");

	// THESE ALSO HAVE TO BE ACCOUNTED FOR IN THE IMPORTER
	//  TODO: Tie these together at some point.
	m_lstPairedNonUniques.push_back(std::make_pair("set", "mid"));
	m_iAllCharBuffSize = 0;
	m_AllCharBuff = new char[7500000];
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

		SourceObject* sO = new SourceObject(m_iAllCharBuffSize);
		std::string aszName = xmlNode_CardName->value();
		m_iAllCharBuffSize += sO->AddAttribute("nam", xmlNode_CardName->value(), m_AllCharBuff, m_iAllCharBuffSize);
		m_lstptCardBuffer.push_back(*sO);
		delete sO;
		sO = &m_lstptCardBuffer[m_lstptCardBuffer.size() - 1];

		bool bHasAll = false;
		if (aszName == "Gideon, Ally of Zendikar")
		{
			int i = 0;
		}
		while (xmlNode_CardAttribute != 0)
		{
			std::string szCardKey = xmlNode_CardAttribute->name();
			std::string keyCode = getKeyCode(szCardKey);
			if (keyCode != "")
			{
				if (isUnique(szCardKey))
				{
					m_iAllCharBuffSize += sO->AddAttribute(
						keyCode,
						xmlNode_CardAttribute->value(),
						m_AllCharBuff,
						m_iAllCharBuffSize);
				}
				else
				{

					m_iAllCharBuffSize += sO->AddNonUniqueAttribute(
						keyCode,
						xmlNode_CardAttribute->value(),
						m_AllCharBuff,
						m_iAllCharBuffSize);
				}
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

	// Look in the Source Object Buffer for a matching item.
	int iFound = findInBuffer(szCardName, false);
	if (iFound != -1)
	{
		SourceObject* oSource = &m_lstptCardBuffer.at(iFound);

		// Check if this card has already been loaded. If not, load it.
		iCacheLocation = oSource->GetCacheIndex();
		if (iCacheLocation == -1)
		{
			// The card is already cached.
			CollectionObject oCard(aszCardName);
			std::vector<std::pair<std::string, std::string>> lstAttrs = oSource->GetAttributes(m_AllCharBuff);
			std::vector<std::pair<std::string, std::string>>::iterator att_iter = lstAttrs.begin();
			bool bHasAllAttributes = false;
			for (; att_iter != lstAttrs.end() && !bHasAllAttributes; ++att_iter)
			{
				bHasAllAttributes = oCard.MapAttributes(att_iter->first, att_iter->second);
			}

			oCard.SetNonUniqueAttributeRestrictions(oSource->GetNonUniqueAttributeRestrictions(m_AllCharBuff));
			oCard.SetPairedNonUniqueAttrs(m_lstPairedNonUniques);

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
		std::string szCard = iter_Cards->GetName(m_AllCharBuff);
		std::transform(szCard.begin(), szCard.end(), szCard.begin(), ::tolower);
		size_t iFindIndex = 0;
		iFindIndex = szCard.find(aszText);
		if (iFindIndex != std::string::npos)
		{
			if (iFindIndex == 0)
			{
				lstStartCards.push_back(iter_Cards->GetName(m_AllCharBuff));
			}
			else
			{
				lstOthers.push_back(iter_Cards->GetName(m_AllCharBuff));
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

		szName = m_lstptCardBuffer.at(middle).GetName(m_AllCharBuff);
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

std::string CollectionSource::getKeyCode(std::string aszFullKey)
{
	if (aszFullKey == "power")
	{
		return "pow";
	}
	else if (aszFullKey == "toughness")
	{
		return "tuf";
	}
	else if (aszFullKey == "manaCost")
	{
		return "man";
	}
	else if (aszFullKey == "text")
	{
		return "txt";
	}
	else if (aszFullKey == "loyalty")
	{
		return "loy";
	}
	else if (aszFullKey == "name")
	{
		return "nam";
	}
	else if (aszFullKey == "colors")
	{
		return "clr";
	}
	else if (aszFullKey == "multiverseid")
	{
		return "mid";
	}
	else if (aszFullKey == "set")
	{
		return "set";
	}

}