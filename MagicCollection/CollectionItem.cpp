#include "CollectionItem.h"

CollectionItem::PseudoIdentifier::PseudoIdentifier()
{
}

CollectionItem::PseudoIdentifier::PseudoIdentifier(unsigned int aiCount, std::string aszName, std::string aszDetails, std::string aszMeta)
{
	Count = aiCount;
	Name = aszName;
	DetailString = aszDetails;
	MetaString = aszMeta;

	CollectionItem::ParseTagString(aszDetails, Identifiers);
	CollectionItem::ParseTagString(aszMeta, MetaTags);
}

CollectionItem::PseudoIdentifier::~PseudoIdentifier()
{
}

CollectionItem::CollectionItem(std::string aszName, std::vector<Tag> alstCommon, std::vector<TraitItem> alstRestrictions)
{
	m_szName = aszName;
	m_lstCommonTraits = alstCommon;
	m_lstIdentifyingTraits = alstRestrictions;
}


CollectionItem::~CollectionItem()
{
	m_lstCopies.clear();
}

std::string CollectionItem::GetName()
{
	return m_szName;
}

CopyItem* CollectionItem::AddCopyItem(std::string aszCollectionName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags)
{
	CopyItem* newCopy = GenerateCopy(aszCollectionName, alstAttrs, alstMetaTags);
	m_lstCopies.push_back(newCopy);
	return newCopy;
}

CopyItem* CollectionItem::GenerateCopy(std::string aszCollectionName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags)
{
	CopyItem* newCopy = new CopyItem(&m_lstIdentifyingTraits, aszCollectionName, alstAttrs, alstMetaTags);

	return newCopy;
}

void CollectionItem::RemoveCopyItem(std::string aszHash)
{
	std::vector<CopyItem*>::iterator iter_Copies = m_lstCopies.begin();

	for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
	{
		if ((*iter_Copies)->GetMetaTag(Config::HashKey, Hidden) == aszHash)
		{
			m_lstCopies.erase(iter_Copies);
			break;
		}
	}

}

CopyItem* CollectionItem::FindCopyItem(std::string aszHash, std::string aszResidentIn)
{
	std::vector<CopyItem*>::iterator iter_Copies = m_lstCopies.begin();

	for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
	{
		if ((*iter_Copies)->GetMetaTag(Config::HashKey, Hidden) == aszHash &&
			(aszResidentIn == "" || (*iter_Copies)->IsResidentIn(aszResidentIn)))
		{
			return *iter_Copies;
		}
	}

	return nullptr;
}

std::vector<CopyItem*> CollectionItem::FindAllCopyItems(std::string aszHash, std::string aszResidentIn)
{
	std::vector<CopyItem*> lstRetval;
	std::vector<CopyItem*>::iterator iter_Copies = m_lstCopies.begin();

	for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
	{
		if ((*iter_Copies)->GetMetaTag(Config::HashKey, Hidden) == aszHash &&
			(aszResidentIn == "" || (*iter_Copies)->IsResidentIn(aszResidentIn)))
		{
			lstRetval.push_back(*iter_Copies);
		}
	}

	return lstRetval;
}

void CollectionItem::Erase(CopyItem* ociRemove)
{
	std::vector<CopyItem*>::iterator iter_copy = m_lstCopies.begin();
	int iFound = ListHelper::List_Find(ociRemove, m_lstCopies);
	m_lstCopies.erase(m_lstCopies.begin() + iFound);
}

std::vector<CopyItem*> CollectionItem::GetCopiesForCollection(std::string aszCollection, CollectionItemType aItemType)
{
	std::vector<CopyItem*> lstRetVal;
	std::vector<CopyItem*>::iterator iter_Copies = m_lstCopies.begin();

	for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
	{
		if ((aItemType & Local) > 0 && (*iter_Copies)->GetParent() == aszCollection)
		{
			lstRetVal.push_back(*iter_Copies);
		}
		else if ((aItemType & Borrowed) > 0            &&
			(*iter_Copies)->GetParent() != aszCollection  &&
			(*iter_Copies)->IsResidentIn(aszCollection))
		{
			lstRetVal.push_back(*iter_Copies);
		}
		else if ((aItemType & Virtual) > 0 &&
			(*iter_Copies)->GetParent() == "" &&
			(*iter_Copies)->IsResidentIn(aszCollection))
		{
			lstRetVal.push_back(*iter_Copies);
		}
	}

	return lstRetVal;
}

std::string CollectionItem::GetHash(std::string aszCollectionName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags)
{
	CopyItem copyToHash(&m_lstIdentifyingTraits, aszCollectionName, alstAttrs, alstMetaTags);

	return copyToHash.GetHash();
}

std::string CollectionItem::GetCardString(CopyItem* aItem, MetaTagType aTagType, std::string aszCompareParent)
{
	return CollectionItem::ToCardLine(aItem->GetParent(), m_szName, aItem->GetIdentifyingAttributes(), aItem->GetMetaTags(aTagType));
}

std::string CollectionItem::GetProtoTypeString()
{
	return CollectionItem::ToCardLine("", "", m_lstCommonTraits);
}

bool CollectionItem::ParseCardLine(std::string aszLine, PseudoIdentifier& rPIdentifier)
{
	unsigned int iCount;
	std::string szMeta;
	std::string szDetails;
	std::string szName;

	unsigned int i = 0;
	std::string szNum = "";
	while (i < aszLine.size() && aszLine.at(i) < '9' && aszLine.at(i) > '0')
	{
		szNum = szNum + aszLine.at(i);
		i++;
	}

	if (i >= aszLine.size())
	{
		return false;
	}

	if (szNum == "")
	{
		szNum = "1";
	}

	try
	{
		iCount = std::stoi(szNum);
	}
	catch (...)
	{
		return false;
	}

	if (aszLine.at(i) == 'x')
	{
		i++;
	}

	if (i >= aszLine.size())
	{
		return false;
	}

	szName = "";
	unsigned int iter_size = aszLine.size();
	while (i < iter_size &&
		((aszLine.at(i) >= 'a' && aszLine.at(i) <= 'z') ||
		(aszLine.at(i) >= 'A' && aszLine.at(i) <= 'Z') ||
			(aszLine.at(i) == ',' || aszLine.at(i) == ' ' || aszLine.at(i) == '-')))
	{
		szName = szName + aszLine.at(i);
		i++;
	}

	szName.erase(0, szName.find_first_not_of(' '));
	szName.erase(szName.find_last_not_of(' ') + 1);

	while (i < iter_size && aszLine.at(i) == ' ')
	{
		i++;
	}

	bool hasDets = false;
	bool hasMeta = false;
	if (i < iter_size)
	{
		hasDets = aszLine.at(i) == '{';
		hasMeta = aszLine.at(i) == ':';
	}

	szDetails = "";
	if (i < iter_size && hasDets)
	{
		while (i < iter_size && aszLine.at(i) != '}')
		{
			szDetails += aszLine.at(i);
			i++;
		}
		if (i < iter_size)
		{
			szDetails += aszLine.at(i);
			i++;
		}
	}

	if (!hasMeta && hasDets) 
	{
		while (i < iter_size && aszLine.at(i) != ':')
		{
			i++;
		}
		hasMeta = (i < iter_size) && (aszLine.at(i) == ':');
	}

	szMeta = "";
	if (i < iter_size && hasMeta)
	{
		i++;
		while (i < iter_size)
		{
			szMeta += aszLine.at(i);
			i++;
		}
	}

	// Output the details
	rPIdentifier = PseudoIdentifier(iCount, szName, szDetails, szMeta);
	return true;
}

bool CollectionItem::ParseTagString(std::string aszDetails, std::vector<Tag>& rlstTags)
{
	std::vector<Tag> lstKeyVals;
	std::vector<std::string> lstDetails = StringHelper::Str_Split(aszDetails, " ");
	for (std::vector<std::string>::iterator iter_attrs = lstDetails.begin(); iter_attrs != lstDetails.end(); ++iter_attrs)
	{
		std::vector<std::string> lstPairs = StringHelper::Str_Split(*iter_attrs, "=");
		if (lstPairs.size() > 1)
		{
			std::vector<std::string> lstVal = StringHelper::Str_Split(lstPairs[1], "\"");
			if (lstVal.size() == 3)
			{
				std::string szVal = lstVal[1];
				lstKeyVals.push_back(std::make_pair(lstPairs[0], szVal));
			}
		}
	}
	rlstTags = lstKeyVals;
	return true;
}

std::string CollectionItem::ToCardLine(std::string aszParentCollection,
	std::string aszName,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags,
	std::string aszCompareParent)
{
	std::string szLine = aszName;
	szLine += " { ";

	if (aszParentCollection != aszCompareParent)
	{
		szLine += "Parent=\"";
		szLine += aszParentCollection;
		szLine += "\" ";
	}

	std::vector<Tag>::iterator iter_keyValPairs;
	if (alstAttrs.size() > 0)
	{
		iter_keyValPairs = alstAttrs.begin();
		for (; iter_keyValPairs != alstAttrs.end(); ++iter_keyValPairs)
		{
			szLine += iter_keyValPairs->first;
			szLine += "=\"";
			szLine += iter_keyValPairs->second;
			szLine += "\" ";
		}

	}
	szLine += "}";

	if (alstMetaTags.size() == 0)
	{
		return szLine;
	}

	szLine += " : { ";

	iter_keyValPairs = alstMetaTags.begin();
	for (; iter_keyValPairs != alstMetaTags.end(); ++iter_keyValPairs)
	{
		szLine += iter_keyValPairs->first;
		szLine += "=\"";
		szLine += iter_keyValPairs->second;
		szLine += "\" ";
	}
	szLine += "}";

	return szLine;
}