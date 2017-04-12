#include "CollectionObject.h"

// std::vector<std::string> CopyObject::PerCollectionMetaTagNames({ "Generalization" });

CopyObject::CopyObject(
	std::string aszParent,
	std::vector<std::pair<std::string, std::string>>* alstPairedTags,
	std::map<std::string, std::vector<std::string>>* alstRestrictions)
{
	ParentCollection = aszParent;
	SetPairedTraitsReference(alstPairedTags);
	SetNonUniqueAttributesRestrictionsReference(alstRestrictions);


	// Initialize the nonuniques.
	std::map<std::string, std::vector<std::string>>::iterator iter_NonUniques = m_mapNonUniqueAttributesRestrictions->begin();
	for (; iter_NonUniques != m_mapNonUniqueAttributesRestrictions->end(); ++iter_NonUniques)
	{
		this->SetNonUniqueAttr(iter_NonUniques->first, iter_NonUniques->second.at(0));
	}
}

void CopyObject::SetPairedTraitsReference(std::vector<std::pair<std::string, std::string>>* aLstPairedTraits)
{
	m_LstPairedTraits = aLstPairedTraits;
}
void CopyObject::SetNonUniqueAttributesRestrictionsReference(std::map<std::string, std::vector<std::string>>* aMapNonUAttrRestr)
{
	m_mapNonUniqueAttributesRestrictions = aMapNonUAttrRestr;
}


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
	std::vector<std::string> lstTags = Config::GetConfigClass()->GetPerCollectionMetaTags();
	std::vector<std::string>::iterator iter_KeyVals = lstTags.begin();
	for (; iter_KeyVals != lstTags.end(); ++iter_KeyVals)
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
	std::vector<std::pair<std::string, std::string>>* targetRemoveList = nullptr;
	if (IsPerCollectionTag(aszKey) && HasPerCollectionTag(aszCollection, aszKey))
	{
		targetRemoveList = &PerCollectionMetaTags[aszCollection];
	}
	else if (HasMetaTag(aszKey))
	{
		targetRemoveList = &MetaTags;
	}

	if (targetRemoveList != nullptr)
	{
		int iFound;
		if ((iFound = Config::GetConfigClass()->List_Find(aszKey, *targetRemoveList)) != -1)
		{
			std::vector<std::pair<std::string, std::string>> lstNewMetaTags;
			std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags =
				targetRemoveList->begin();
			int index = 0;
			for (; iter_MetaTags != targetRemoveList->end(); iter_MetaTags++)
			{
				if (index != iFound)
				{
					lstNewMetaTags.push_back(*iter_MetaTags);
				}
				index++;
			}

			*targetRemoveList = lstNewMetaTags;
		}
	}

}

void CopyObject::SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszVal)
{
	if (aszVal == "!NULL")
	{
		RemoveMetaTag(aszCollection, aszKey);
		return;
	}

	// Ensure that this tag is not unique to a collection
	if (IsPerCollectionTag(aszKey))
	{
		if (HasPerCollectionTag(aszCollection, aszKey))
		{
			int iFind = Config::GetConfigClass()->List_Find(aszKey, PerCollectionMetaTags[aszCollection]);
			if (iFind == -1)
			{
				PerCollectionMetaTags[aszCollection].push_back(std::make_pair(aszKey, aszVal));
			}
			else
			{
				PerCollectionMetaTags[aszCollection][iFind].second = aszVal;
			}

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
		else
		{
			int iFound = Config::GetConfigClass()->List_Find(aszKey, MetaTags);
			MetaTags[iFound].second = aszVal;
		}
	}

}

void CopyObject::SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszSubKey, std::string aszVal)
{
	std::string szFullKey = aszKey + "." + aszSubKey;
	SetMetaTag(aszCollection, szFullKey, aszVal);
}

bool CopyObject::HasMetaTag(std::string aszKey)
{
	return Config::GetConfigClass()->List_Find(aszKey, MetaTags) != -1;
}

std::pair<std::string, std::string> CopyObject::GetMetaTag(std::string aszCollection, std::string aszKey)
{
	std::string szKey = aszKey;
	std::string szVal = "!NULL";
	std::vector<std::pair<std::string, std::string>>::iterator iter_Tags = MetaTags.begin();
	for (; iter_Tags != MetaTags.end(); ++iter_Tags)
	{
		if (iter_Tags->first == aszKey)
		{
			szVal = iter_Tags->first;
		}
	}

	if (szVal == "!NULL")
	{
		if (IsPerCollectionTag(aszKey) && HasPerCollectionTag(aszCollection, aszKey))
		{
			std::vector<std::pair<std::string, std::string>>::iterator iter_PerCols =
				PerCollectionMetaTags.at(aszCollection).begin();
			for (; iter_PerCols != PerCollectionMetaTags.at(aszCollection).end(); ++iter_PerCols)
			{
				if (iter_PerCols->first == aszKey)
				{
					szVal = iter_PerCols->second;
				}
			}
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

void CopyObject::SetNonUniqueAttr(std::string aszKey, std::string aszValue)
{
	if (!Config::GetConfigClass()->IsIdentifyingAttributes(aszKey))
	{
		return;
	}

	bool bCheckPairedTraits = false;
	int iIndexOfAllowedTrait = 0;
	bool bRestrictedMatch = false;
	// Already has a value.
	std::map<std::string, std::vector<std::string>>::iterator lstRestrictions =
		m_mapNonUniqueAttributesRestrictions->find(aszKey);
	if (lstRestrictions != m_mapNonUniqueAttributesRestrictions->end())
	{
		// Check to make sure that the attribute is restricted
		// If it is, make sure the value is a legal value
		if ((iIndexOfAllowedTrait = Config::GetConfigClass()->List_Find(aszValue, lstRestrictions->second)) != -1)
		{
			// It is a legal value
			if (NonUniqueTraits[aszKey] != aszValue)
			{
				NonUniqueTraits[aszKey] = aszValue;
				bCheckPairedTraits = true;
				bRestrictedMatch = true;
			}

		}
	}
	else
	{
		// If not, just set the value.
		if (NonUniqueTraits[aszKey] != aszValue)
		{
			NonUniqueTraits[aszKey] = aszValue;
			bCheckPairedTraits = true;
		}
	}

	// Loop through and make sure all paired traits are set.
	if (bCheckPairedTraits)
	{
		std::vector<std::string> lstSetTraits;
		bool bAllSet = false;
		while (!bAllSet)
		{
			bool bFoundAll = true;
			std::vector<std::pair<std::string, std::string>>::iterator iter_PairedTraits = m_LstPairedTraits->begin();
			for (; iter_PairedTraits != m_LstPairedTraits->end(); ++iter_PairedTraits)
			{
				std::string* pszTargetStr = nullptr;
				if (iter_PairedTraits->first == aszKey)
				{
					pszTargetStr = &iter_PairedTraits->second;
				}
				else if (iter_PairedTraits->second == aszKey)
				{
					pszTargetStr = &iter_PairedTraits->first;
				}

				if (pszTargetStr != nullptr && 
					Config::GetConfigClass()->List_Find(*pszTargetStr, lstSetTraits) == -1)
				{
					if (bRestrictedMatch)
					{
						NonUniqueTraits[*pszTargetStr] =
							(*m_mapNonUniqueAttributesRestrictions)[*pszTargetStr][iIndexOfAllowedTrait];
					}
					else
					{
						NonUniqueTraits[*pszTargetStr] = aszValue;
					}
					lstSetTraits.push_back(*pszTargetStr);
					bFoundAll == false;
				}
			}

			bAllSet |= bFoundAll;
		}

	}
}

bool CopyObject::IsSameMetaTags(std::string aszCollectionName, CopyObject* aoCOne, CopyObject* aoCTwo, bool abUseIgnore)
{
	return IsSameMetaTags(aoCOne->GetMetaTags(aszCollectionName), aoCTwo->GetMetaTags(aszCollectionName), abUseIgnore);
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
	if (abUseIgnore && (iFindIgnore = Config::GetConfigClass()->List_Find("_ignore", alstTagsOne)) != -1)
	{
		std::pair<std::string, std::string> pair_szIgnored = alstTagsOne[iFindIgnore];
		std::string szIgnored = pair_szIgnored.second;
		lstIgnoredTags = StringHelper::Str_Split(szIgnored, ";");
	}

	std::vector<std::pair<std::string, std::string>>::iterator iter_UnignoredTagsOne = alstTagsOne.begin();
	for (; iter_UnignoredTagsOne != alstTagsOne.end(); iter_UnignoredTagsOne++)
	{
		// It is a multitiag if the split list is > 1
		std::vector<std::string> lstSplitString = StringHelper::Str_Split(iter_UnignoredTagsOne->first, ".");
		std::string szKey = lstSplitString[0];
		if (szKey.size() > 0 && szKey[0] != '_' && Config::GetConfigClass()->List_Find(szKey, lstIgnoredTags) == -1)
		{
			lstUnignoredTagsOne.push_back(*iter_UnignoredTagsOne);
		}
	}

	if (abUseIgnore && (iFindIgnore = Config::GetConfigClass()->List_Find("_ignore", alstTagsTwo)) != -1)
	{
		std::pair<std::string, std::string> pair_szIgnored = alstTagsTwo[iFindIgnore];
		std::string szIgnored = pair_szIgnored.second;
		lstIgnoredTags = StringHelper::Str_Split(szIgnored, ";");
	}

	std::vector<std::pair<std::string, std::string>>::iterator iter_UnignoredTagsTwo = alstTagsTwo.begin();
	for (; iter_UnignoredTagsTwo != alstTagsTwo.end(); iter_UnignoredTagsTwo++)
	{
		// It is a multitiag if the split list is > 1
		std::vector<std::string> lstSplitString = StringHelper::Str_Split(iter_UnignoredTagsTwo->first, ".");
		std::string szKey = lstSplitString[0];
		if (szKey.size() > 0 && szKey[0] != '_' &&  Config::GetConfigClass()->List_Find(szKey, lstIgnoredTags) == -1)
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

std::vector<std::string> CollectionObject::GetRestrictionListFor(std::string aszAttr)
{
	std::vector<std::string> lstRetVal;

	if (m_mapNonUniqueAttributesRestrictions.find(aszAttr) != m_mapNonUniqueAttributesRestrictions.end())
	{
		lstRetVal == m_mapNonUniqueAttributesRestrictions[aszAttr];
	}

	return lstRetVal;
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
	CopyObject oNewCopy = GenerateCopy(aszCollectionName);

	m_lstCopies.push_back(oNewCopy);
	return &m_lstCopies[m_lstCopies.size() - 1];
}

CopyObject*  CollectionObject::AddCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	CopyObject oNewCopy = GenerateCopy(aszCollectionName, alstAttrs);

	m_lstCopies.push_back(oNewCopy);
	return &m_lstCopies[m_lstCopies.size() - 1];
}

CopyObject*  CollectionObject::AddCopy(std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	CopyObject oNewCopy = GenerateCopy(aszCollectionName, alstAttrs, alstMeta);

	m_lstCopies.push_back(oNewCopy);
	return &m_lstCopies[m_lstCopies.size() - 1];
}

CopyObject CollectionObject::GenerateCopy(std::string aszCollectionName)
{
	CopyObject oNewCopy(aszCollectionName, &m_lstPairedAttributes, &m_mapNonUniqueAttributesRestrictions);
	oNewCopy.ResidentCollections.push_back(aszCollectionName);
	return oNewCopy;
}


CopyObject CollectionObject::GenerateCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	CopyObject oNewCopy = GenerateCopy(aszCollectionName);

	for (int i = 0; i < alstAttrs.size(); i++)
	{
		std::pair<std::string, std::string> pszs = alstAttrs.at(i);
		if (pszs.first == "Parent")
		{
			oNewCopy.ParentCollection = pszs.second;
		}
		else
		{
			// We only need to store the non-unique traits.
			//  All other traits are stored in the collectionobj.
			if (Config::GetConfigClass()->IsIdentifyingAttributes(pszs.first))
			{
				oNewCopy.SetNonUniqueAttr(pszs.first, pszs.second);
			}
		}

	}

	return oNewCopy;
}

CopyObject CollectionObject::GenerateCopy(
	std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	CopyObject oNewCopy = GenerateCopy(aszCollectionName, alstAttrs);

	std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags = alstMeta.begin();
	for (; iter_MetaTags != alstMeta.end(); ++iter_MetaTags)
	{
		oNewCopy.SetMetaTag(aszCollectionName, iter_MetaTags->first, iter_MetaTags->second);
	}

	return oNewCopy;
}

bool CollectionObject::RemoveCopy(std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	CopyObject oNewCopy = GenerateCopy(aszCollectionName, alstAttrs, alstMeta);
	for (std::vector<CopyObject>::iterator iter = m_lstCopies.begin(); iter != m_lstCopies.end(); ++iter)
	{
		if (IsSameIdentity(&oNewCopy, &(*iter)))
		{
			if (CopyObject::IsSameMetaTags(iter->GetMetaTags(aszCollectionName), alstMeta))
			{
				m_lstCopies.erase(iter);
				return true;
			}

		}

	}

	return false;
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

bool CollectionObject::FindCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs, CopyObject*& roCO, bool abExact)
{
	bool bFound = false;
	CopyObject oCompare = GenerateCopy(aszCollectionName, alstAttrs);

	std::vector<CopyObject*> lstCopies = GetLocalCopies(aszCollectionName);
	std::vector<CopyObject*>::iterator iter_copies = lstCopies.begin();
	for (; iter_copies != lstCopies.end(); ++iter_copies)
	{
		if (IsSameIdentity(&oCompare, *iter_copies))
		{
			bFound = true;
			roCO = *iter_copies;
			break;
		}
	}

	return bFound;
}

bool CollectionObject::FindCopy(
	std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta,
	CopyObject*& roCO)
{
	bool bFound = false;
	CopyObject oCompare = GenerateCopy(aszCollectionName, alstAttrs, alstMeta);

	std::vector<CopyObject*> lstCopies = GetCopies(aszCollectionName);
	std::vector<CopyObject*>::iterator iter_copies = lstCopies.begin();
	for (; iter_copies != lstCopies.end(); ++iter_copies)
	{
		if (IsSameIdentity(&oCompare, *iter_copies))
		{
			if (CopyObject::IsSameMetaTags(aszCollectionName, &oCompare, *iter_copies, true))
			{
				bFound = true;
				roCO = *iter_copies;
				break;
			}

		}
	}

	return bFound;
}

std::map<std::string, std::vector<std::string>> CollectionObject::GetNonUniqueAttributeRestrictions()
{
	return m_mapNonUniqueAttributesRestrictions;
}

// Sets nonunique attributes
void CollectionObject::SetNonUniqueAttributeRestrictions(std::map<std::string, std::vector<std::string>> aMapRestrictions)
{
	m_mapNonUniqueAttributesRestrictions = aMapRestrictions;
}

void CollectionObject::SetPairedNonUniqueAttrs(std::vector<std::pair<std::string, std::string>> alstPairedAttrs)
{
	m_lstPairedAttributes = alstPairedAttrs;
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

std::vector<std::pair<std::string, std::string>> CollectionObject::FilterOutUniqueTraits(std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	std::vector<std::pair<std::string, std::string>>::iterator iter_Traits = alstAttrs.begin();
	for (; iter_Traits != alstAttrs.end(); ++iter_Traits)
	{
		if (Config::GetConfigClass()->IsIdentifyingAttributes(iter_Traits->first))
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
