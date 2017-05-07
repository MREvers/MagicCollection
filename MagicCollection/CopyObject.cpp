#include "CopyObject.h"

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

std::vector<std::pair<std::string, std::string>> CopyObject::GetNonUniqueTraits(bool bWithParent)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	lstRetVal = ListHelper::ConvertMapToList(NonUniqueTraits);
	if (bWithParent)
	{
		lstRetVal.push_back(std::make_pair("Parent", ParentCollection));
	}
	return lstRetVal;
}

void CopyObject::SetPairedTraitsReference(std::vector<std::pair<std::string, std::string>>* aLstPairedTraits)
{
	m_LstPairedTraits = aLstPairedTraits;
}
void CopyObject::SetNonUniqueAttributesRestrictionsReference(std::map<std::string, std::vector<std::string>>* aMapNonUAttrRestr)
{
	m_mapNonUniqueAttributesRestrictions = aMapNonUAttrRestr;
}

void CopyObject::AddResidentCollection(std::string aszCollectionName)
{
	if (Config::GetConfigClass()->List_Find(aszCollectionName, ResidentCollections) == -1)
	{
		ResidentCollections.push_back(aszCollectionName);
	}
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
					bFoundAll = false;
				}
			}

			bAllSet |= bFoundAll;
		}

	}
}
