#include "CollectionObject.h"

std::vector<std::string> CopyObject::PerCollectionMetaTagNames({ "Generalization" });

std::vector<std::pair<std::string, std::string>> CopyObject::GetMetaTags(std::string aszCollection)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	if (PerCollectionMetaTags.find(aszCollection) != PerCollectionMetaTags.end())
	{
		lstRetVal = PerCollectionMetaTags[aszCollection];
	}

	std::vector<std::pair<std::string, std::string>>::iterator iter_NUTags = MetaTags.begin();
	for (; iter_NUTags != MetaTags.end(); ++iter_NUTags)
	{
		lstRetVal.push_back(*iter_NUTags);
	}

	return lstRetVal;
}

bool CopyObject::IsPerCollectionTag(std::string aszKeyValName)
{
	std::vector<std::string>::iterator iter_KeyVals = PerCollectionMetaTagNames.begin();
	for (; iter_KeyVals != PerCollectionMetaTagNames.end(); ++iter_KeyVals)
	{
		if ((*iter_KeyVals) == aszKeyValName)
		{
			return true;
		}
	}
	return false;
}

void CopyObject::RemoveMetaTag(std::string aszCollection, std::string aszKey)
{
	// Ensure that this tag is not unique to a collection
	if (IsPerCollectionTag(aszKey) && HasPerCollectionTag(aszCollection, aszKey))
	{
		int iFound;
		if ((iFound = SourceObject::List_Find(aszKey, PerCollectionMetaTags[aszCollection])) != -1)
		{
			std::vector<std::pair<std::string, std::string>> lstNewMetaTags;
			std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags =
				PerCollectionMetaTags[aszCollection].begin();
			int index = 0;
			for (; iter_MetaTags != PerCollectionMetaTags[aszCollection].end(); iter_MetaTags++)
			{
				if (index != iFound)
				{
					lstNewMetaTags.push_back(*iter_MetaTags);
				}
				index++;
			}

			PerCollectionMetaTags[aszCollection] = lstNewMetaTags;
		}

	}
	else
	{
		if (HasMetaTag(aszKey))
		{
			int iFound;
			if ((iFound = SourceObject::List_Find(aszKey, MetaTags)) != -1)
			{
				std::vector<std::pair<std::string, std::string>> lstNewMetaTags;
				std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags =
					PerCollectionMetaTags[aszCollection].begin();
				int index = 0;
				for (; iter_MetaTags != PerCollectionMetaTags[aszCollection].end(); iter_MetaTags++)
				{
					if (index != iFound)
					{
						lstNewMetaTags.push_back(*iter_MetaTags);
					}
					index++;
				}

				MetaTags = lstNewMetaTags;
			}
		}
	}
}

void CopyObject::AddMetaTag(std::string aszCollection, std::string aszKey, std::string aszVal)
{
	// Ensure that this tag is not unique to a collection
	if (IsPerCollectionTag(aszKey))
	{
		if (HasPerCollectionTag(aszCollection, aszKey))
		{
			PerCollectionMetaTags[aszCollection].push_back(std::make_pair(aszKey, aszVal));
		}
		else
		{
			std::vector<std::pair<std::string, std::string>> lstNewCol;
			lstNewCol.push_back(std::make_pair(aszKey, aszVal));
			PerCollectionMetaTags[aszCollection] = lstNewCol;
		}
	}
	else
	{
		if (!HasMetaTag(aszKey))
		{
			MetaTags.push_back(std::make_pair(aszKey, aszVal));
		}
	}

}

void CopyObject::AddMetaTag(std::string aszCollection, std::string aszKey, std::string aszSubKey, std::string aszVal)
{
	std::string szFullKey = aszKey + "." + aszSubKey;
	AddMetaTag(aszCollection, szFullKey, aszVal);
}

bool CopyObject::HasMetaTag(std::string aszKey)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_Tags = MetaTags.begin();
	for (; iter_Tags != MetaTags.end(); ++iter_Tags)
	{
		if (iter_Tags->first == aszKey)
		{
			return true;
		}
	}
	return false;
}

std::pair<std::string, std::string> CopyObject::GetMetaTag(std::string aszKey)
{
	std::string szKey = aszKey;
	std::string szVal = "";
	std::vector<std::pair<std::string, std::string>>::iterator iter_Tags = MetaTags.begin();
	for (; iter_Tags != MetaTags.end(); ++iter_Tags)
	{
		if (iter_Tags->first == aszKey)
		{
			szVal = iter_Tags->first;
		}
	}
	return std::make_pair(szKey, szVal);
}

bool CopyObject::HasPerCollectionTag(std::string aszCollection, std::string aszKey)
{
	if (PerCollectionMetaTags.find(aszCollection) != PerCollectionMetaTags.end())
	{
		std::vector<std::pair<std::string, std::string>>::iterator iter_Tags = PerCollectionMetaTags[aszCollection].begin();
		for (; iter_Tags != PerCollectionMetaTags[aszCollection].end(); ++iter_Tags)
		{
			if (iter_Tags->first == aszKey)
			{
				return true;
			}
		}
	}
	return false;

}

bool CopyObject::IsSameMetaTags(CopyObject* aoCOne, CopyObject* aoCTwo, bool abUseIgnore)
{
	return IsSameMetaTags(aoCOne->MetaTags, aoCTwo->MetaTags, abUseIgnore);
}

bool CopyObject::IsSameMetaTags(
	std::vector<std::pair<std::string, std::string>> alstTagsOne,
	std::vector<std::pair<std::string, std::string>> alstTagsTwo,
	bool abUseIgnore)
{
	std::vector<std::pair<std::string, std::string>> lstUnignoredTagsOne;
	std::vector<std::pair<std::string, std::string>> lstUnignoredTagsTwo;

	std::vector<std::string> lstIgnoredTags;
	int iFindIgnore;
	if (abUseIgnore && (iFindIgnore = SourceObject::List_Find("_ignore", alstTagsOne)) != -1)
	{
		std::pair<std::string, std::string> pair_szIgnored = alstTagsOne[iFindIgnore];
		std::string szIgnored = pair_szIgnored.second;
		lstIgnoredTags = SourceObject::Str_Split(szIgnored, ";");
	}

	std::vector<std::pair<std::string, std::string>>::iterator iter_UnignoredTagsOne = alstTagsOne.begin();
	for (; iter_UnignoredTagsOne != alstTagsOne.end(); iter_UnignoredTagsOne++)
	{
		// It is a multitiag if the split list is > 1
		std::vector<std::string> lstSplitString = SourceObject::Str_Split(iter_UnignoredTagsOne->first, ".");
		std::string szKey = lstSplitString[0];
		if (szKey.size() > 0 && szKey[0] != '_' && SourceObject::List_Find(szKey, lstIgnoredTags) == -1)
		{
			lstUnignoredTagsOne.push_back(*iter_UnignoredTagsOne);
		}
	}

	if (abUseIgnore && (iFindIgnore = SourceObject::List_Find("_ignore", alstTagsTwo)) != -1)
	{
		std::pair<std::string, std::string> pair_szIgnored = alstTagsTwo[iFindIgnore];
		std::string szIgnored = pair_szIgnored.second;
		lstIgnoredTags = SourceObject::Str_Split(szIgnored, ";");
	}

	std::vector<std::pair<std::string, std::string>>::iterator iter_UnignoredTagsTwo = alstTagsTwo.begin();
	for (; iter_UnignoredTagsTwo != alstTagsTwo.end(); iter_UnignoredTagsTwo++)
	{
		// It is a multitiag if the split list is > 1
		std::vector<std::string> lstSplitString = SourceObject::Str_Split(iter_UnignoredTagsTwo->first, ".");
		std::string szKey = lstSplitString[0];
		if (szKey.size() > 0 && szKey[0] != '_' && SourceObject::List_Find(szKey, lstIgnoredTags) == -1)
		{
			lstUnignoredTagsTwo.push_back(*iter_UnignoredTagsTwo);
		}
	}

	return CollectionObject::CompareKeyValPairList(lstUnignoredTagsTwo, lstUnignoredTagsOne);
}

CollectionObject::CollectionObject(std::string aszName)
{
	m_iAllCopies = 0;
	m_szName = aszName;
}

CollectionObject::~CollectionObject()
{
}

bool CollectionObject::MapAttributes(std::string aszName, std::string aszValue)
{
	m_mapAttributes[aszName] = aszValue;
	return false;
}

std::string CollectionObject::GetAttribute(std::string aszAttr)
{
	if (m_mapAttributes.find(aszAttr) != m_mapAttributes.end())
	{
		return m_mapAttributes[aszAttr];
	}
}

std::map<std::string, std::string> CollectionObject::GetAttributesMap()
{
	return m_mapAttributes;
}

std::string CollectionObject::GetName()
{
	return m_szName;
}

// By default the parent is the collection that adds it.
CopyObject* CollectionObject::AddCopy(std::string aszCollectionName)
{
	CopyObject oNewCopy;
	oNewCopy.ParentCollection = aszCollectionName;
	oNewCopy.ResidentCollections.push_back(aszCollectionName);
	m_lstCopies.push_back(oNewCopy);
	return &m_lstCopies[m_lstCopies.size() - 1];
}

CopyObject CollectionObject::GenerateCopy(std::string aszCollectionName)
{
	CopyObject oNewCopy;
	oNewCopy.ParentCollection = aszCollectionName;
	oNewCopy.ResidentCollections.push_back(aszCollectionName);
	return oNewCopy;
}


CopyObject CollectionObject::GenerateCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	CopyObject oNewCopy;
	oNewCopy.ParentCollection = aszCollectionName;
	oNewCopy.ResidentCollections.push_back(aszCollectionName);
	ConstructCopy(oNewCopy, alstAttrs);
	return oNewCopy;
}


void CollectionObject::RemoveCopy(std::string aszCollectionName)
{
	// In the future, we need to find the copy that matches the description.
	// If more than one copy matches the description, remove any.
	for (std::vector<CopyObject>::iterator iter = m_lstCopies.begin(); iter != m_lstCopies.end(); ++iter)
	{
		if (iter->ParentCollection == aszCollectionName)
		{
			m_lstCopies.erase(iter);
			break;
		}
	}
}

void CollectionObject::RemoveCopy(std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	// In the future, we need to find the copy that matches the description.
	// If more than one copy matches the description, remove any.
	for (std::vector<CopyObject>::iterator iter = m_lstCopies.begin(); iter != m_lstCopies.end(); ++iter)
	{
		if (iter->ParentCollection == aszCollectionName)
		{
			if (CompareKeyValPairList(
				ConvertMapToList(iter->NonUniqueTraits),
				FilterNonUniqueTraits(alstAttrs)))
			{
				if (CopyObject::IsSameMetaTags(iter->MetaTags, alstMeta))
				{
					m_lstCopies.erase(iter);
					break;
				}

			}

		}
	}
}

std::vector<CopyObject*> CollectionObject::GetLocalCopiesWith(std::string aszParent, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	std::vector<CopyObject*> rLstRetVal;
	std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
	for (; card_iter != m_lstCopies.end(); ++card_iter)
	{
		if (card_iter->ParentCollection == aszParent)
		{
			CopyObject oCOID = GenerateCopy(aszParent, alstAttrs);
			CopyObject* oFoundCard = card_iter._Ptr;
			if (IsSameIdentity(&oCOID, oFoundCard, true))
			{
				rLstRetVal.push_back(oFoundCard);
			}
			break;
		}

	}

	return rLstRetVal;
}

std::vector<CopyObject*> CollectionObject::GetLocalCopies(std::string aszCollectionName)
{
	std::vector<CopyObject*> rLstRetVal;
	std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
	for (; card_iter != m_lstCopies.end(); ++card_iter)
	{
		if (card_iter->ParentCollection == aszCollectionName)
		{
			CopyObject* oFoundCard = card_iter._Ptr;
			rLstRetVal.push_back(oFoundCard);
		}
	}

	return rLstRetVal;
}

std::vector<CopyObject*> CollectionObject::GetCopies(std::string aszCollectionName)
{
	std::vector<CopyObject*> rLstRetVal;
	std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
	for (; card_iter != m_lstCopies.end(); ++card_iter)
	{
		std::vector<std::string>::iterator resi_iter = card_iter->ResidentCollections.begin();
		for (; resi_iter != card_iter->ResidentCollections.end(); ++resi_iter)
		{
			if (*resi_iter == aszCollectionName)
			{
				CopyObject* oFoundCard = card_iter._Ptr;
				rLstRetVal.push_back(oFoundCard);
				break;
			}
		}
	}

	return rLstRetVal;
}

std::vector<CopyObject*> CollectionObject::GetCopiesWith(std::string aszCollectionName, std::string aszCardParent, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	std::vector<CopyObject*> rLstRetVal;
	std::vector<CopyObject>::iterator card_iter = m_lstCopies.begin();
	for (; card_iter != m_lstCopies.end(); ++card_iter)
	{
		std::vector<std::string>::iterator resi_iter = card_iter->ResidentCollections.begin();
		for (; resi_iter != card_iter->ResidentCollections.end(); ++resi_iter)
		{
			if (*resi_iter == aszCollectionName)
			{
				CopyObject oCOID = GenerateCopy(aszCardParent, alstAttrs);
				CopyObject* oFoundCard = card_iter._Ptr;
				if (IsSameIdentity(&oCOID, oFoundCard, true))
				{
					rLstRetVal.push_back(oFoundCard);
				}
				break;
			}
		}
	}

	return rLstRetVal;
}

bool CollectionObject::GetCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs, CopyObject& roCO, bool abExact)
{
	bool bFound = false;
	CopyObject oCompare = GenerateCopy(aszCollectionName);
	ConstructCopy(oCompare, alstAttrs);

	std::vector<CopyObject*> lstCopies = GetLocalCopies(aszCollectionName);
	std::vector<CopyObject*>::iterator iter_copies = lstCopies.begin();
	for (; iter_copies != lstCopies.end(); ++iter_copies)
	{
		if (IsSameIdentity(&oCompare, *iter_copies))
		{
			bFound = true;
			roCO = **iter_copies;
			break;
		}
	}

	return bFound;
}

void CollectionObject::ConstructCopy(CopyObject& roCO, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	for (int i = 0; i < alstAttrs.size(); i++)
	{
		std::pair<std::string, std::string> pszs = alstAttrs.at(i);
		if (pszs.first == "Parent")
		{
			roCO.ParentCollection = pszs.second;
		}
		else
		{
			// Only if its a nonunique trait
			if (!(IsUniqueTrait(pszs.first)))
			{
				roCO.NonUniqueTraits.at(pszs.first) = pszs.second;
			}

		}

	}
}

// ASSUMES TWO CARDS OF THE SAME TYPE
// Keep in mind that this does not compare names because the name of the card is not known by the copy object.
// Only compares attributes
bool CollectionObject::IsSameIdentity(CopyObject* aoCOne, CopyObject* aoCTwo, bool bMatchParent)
{
	bool bMatch = true;

	bMatch &= (aoCOne->ParentCollection == aoCTwo->ParentCollection) || !bMatchParent;

	if (bMatch)
	{
		std::map<std::string, std::string>::iterator iter_keyVals = aoCOne->NonUniqueTraits.begin();
		for (; iter_keyVals != aoCOne->NonUniqueTraits.end(); ++iter_keyVals)
		{

			bool bFoundMatch = false;
			std::map<std::string, std::string>::iterator iter_keyValsTwo = aoCTwo->NonUniqueTraits.begin();
			for (; iter_keyValsTwo != aoCTwo->NonUniqueTraits.end(); ++iter_keyValsTwo)
			{
				if (iter_keyVals->first == iter_keyValsTwo->first)
				{
					if (bFoundMatch |= iter_keyVals->second == iter_keyValsTwo->second)
					{
						break;
					}
				}
			}

			if (!(bMatch &= bFoundMatch))
			{
				break;
			}
		}

		return bMatch;
	}
}

std::vector<std::pair<std::string, std::string>> CollectionObject::FilterNonUniqueTraits(std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	std::vector<std::pair<std::string, std::string>>::iterator iter_Traits = alstAttrs.begin();
	for (; iter_Traits != alstAttrs.end(); ++iter_Traits)
	{
		if (!(IsUniqueTrait(iter_Traits->first)))
		{
			lstRetVal.push_back(*iter_Traits);
		}
	}
	return lstRetVal;
}

std::vector<std::pair<std::string, std::string>> CollectionObject::ConvertMapToList(std::map<std::string, std::string>  aMap)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	std::map<std::string, std::string>::iterator iter_Map = aMap.begin();
	for (; iter_Map != aMap.end(); ++iter_Map)
	{
		lstRetVal.push_back(std::make_pair(iter_Map->first, iter_Map->second));
	}
	return lstRetVal;
}

const char * const CollectionObject::LstUniqueTraits[] = { "manaCost", "colors", "name", "power",
"toughness", "loyalty", "text" };

bool CollectionObject::IsUniqueTrait(std::string aszTrait)
{
	for (int i = 0; i < 0; i++)
	{
		if (aszTrait == LstUniqueTraits[i])
		{
			return true;
		}
	}
	return false;
}

bool CollectionObject::CompareKeyValPairList(std::vector<std::pair<std::string, std::string>> alstFirst,
	std::vector<std::pair<std::string, std::string>> alstSecond)
{
	bool bMatch = true;

	if (bMatch = (alstFirst.size() == alstSecond.size()))
	{
		std::vector<std::pair<std::string, std::string>>::iterator iter_First = alstFirst.begin();
		std::vector<std::pair<std::string, std::string>>::iterator iter_Second = alstSecond.begin();

		for (; iter_First != alstFirst.end(); ++iter_First)
		{
			bool bFoundMatch = false;
			for (; iter_Second != alstSecond.end(); ++iter_Second)
			{
				if (iter_First->first == iter_Second->first)
				{
					if (iter_First->second == iter_Second->second)
					{
						bFoundMatch = true;
						break;
					}
				}
			}

			if (!bFoundMatch)
			{
				bMatch = false;
				break;
			}
		}
	}
	return bMatch;
}

std::string CollectionObject::str_trim(const std::string& src, char removeChar)
{
	size_t first = src.find_first_not_of(' ');
	if (std::string::npos == first)
	{
		return src;
	}
	size_t last = src.find_last_not_of(' ');
	return src.substr(first, (last - first + 1));
}