#include "CollectionObject.h"

CollectionObject::CollectionObject(std::string aszName)
{
	m_iAllCopies = 0;
	m_szName = aszName;
}

CollectionObject::~CollectionObject() {}

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
	return "";
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

	for (unsigned int i = 0; i < alstAttrs.size(); i++)
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
			if (Config::Instance()->IsIdentifyingAttributes(pszs.first))
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
		if (IsSameIdentity(&oNewCopy, &(*iter)) &&
			IsSameMetaTags(iter->GetMetaTags(aszCollectionName), alstMeta))
		{
			m_lstCopies.erase(iter);
			return true;
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
			if (IsSameMetaTags(aszCollectionName, &oCompare, *iter_copies, true))
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
		bMatch &= ListHelper::CompareKeyValPairList(
			ListHelper::ConvertMapToList(aoCOne->NonUniqueTraits),
			ListHelper::ConvertMapToList(aoCTwo->NonUniqueTraits)
		);
	}

	return bMatch;
}


bool CollectionObject::IsSameMetaTags(std::string aszCollectionName, CopyObject* aoCOne, CopyObject* aoCTwo, bool abUseIgnore)
{
	return IsSameMetaTags(aoCOne->GetMetaTags(aszCollectionName), aoCTwo->GetMetaTags(aszCollectionName), abUseIgnore);
}

bool CollectionObject::IsSameMetaTags(
	std::vector<std::pair<std::string, std::string>> alstTagsOne,
	std::vector<std::pair<std::string, std::string>> alstTagsTwo,
	bool abUseIgnore)
{
	Config* config = Config::Instance();

	std::vector<std::pair<std::string, std::string>> lstUnignoredTagsOne;
	std::vector<std::pair<std::string, std::string>> lstUnignoredTagsTwo;

	std::vector<std::string> lstIgnoredTags;
	int iFindIgnore;
	if (abUseIgnore && (iFindIgnore = config->List_Find("_ignore", alstTagsOne)) != -1)
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
		if (szKey.size() > 0 && szKey[0] != '_' && config->List_Find(szKey, lstIgnoredTags) == -1)
		{
			lstUnignoredTagsOne.push_back(*iter_UnignoredTagsOne);
		}
	}

	if (abUseIgnore && (iFindIgnore = config->List_Find("_ignore", alstTagsTwo)) != -1)
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
		if (szKey.size() > 0 && szKey[0] != '_' &&  config->List_Find(szKey, lstIgnoredTags) == -1)
		{
			lstUnignoredTagsTwo.push_back(*iter_UnignoredTagsTwo);
		}
	}

	return ListHelper::CompareKeyValPairList(lstUnignoredTagsTwo, lstUnignoredTagsOne);
}

std::vector<std::pair<std::string, std::string>> CollectionObject::FilterOutUniqueTraits(std::vector<std::pair<std::string, std::string>> alstAttrs)
{
	std::vector<std::pair<std::string, std::string>> lstRetVal;
	std::vector<std::pair<std::string, std::string>>::iterator iter_Traits = alstAttrs.begin();
	for (; iter_Traits != alstAttrs.end(); ++iter_Traits)
	{
		if (Config::Instance()->IsIdentifyingAttributes(iter_Traits->first))
		{
			lstRetVal.push_back(*iter_Traits);
		}
	}
	return lstRetVal;
}

