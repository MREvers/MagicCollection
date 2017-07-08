#include "CopyItem.h"

CopyItem::CopyItem(std::vector<TraitItem>* alstTraits, std::string aszParentCollection)
{
	m_szParentCollection = aszParentCollection;
	m_plstRestrictedTraits = alstTraits;

	std::vector<TraitItem>::iterator iter_DefaultVals = m_plstRestrictedTraits->begin();
	for (; iter_DefaultVals != m_plstRestrictedTraits->end(); ++iter_DefaultVals)
	{
		m_lstIdentifyingTags.push_back(std::make_pair(iter_DefaultVals->GetKeyName(), iter_DefaultVals->GetDefaultValue()));
	}
}


CopyItem::~CopyItem()
{
}

CopyItem::CopyItem(std::vector<TraitItem>* alstTraits,
	std::string aszParentCollection,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags)
	: CopyItem(alstTraits, aszParentCollection)
{
	std::vector<Tag>::iterator iter_Attrs = alstAttrs.begin();
	for (; iter_Attrs != alstAttrs.end(); ++iter_Attrs)
	{
		SetIdentifyingAttribute(iter_Attrs->first, iter_Attrs->second);
	}

	for (iter_Attrs = alstMetaTags.begin(); iter_Attrs != alstMetaTags.end(); ++iter_Attrs)
	{
		SetMetaTag(iter_Attrs->first, iter_Attrs->second, Public);
	}

	GetHash();
}

std::string CopyItem::GetHash()
{
	std::function<std::string(MetaTag)> fnExtractor
		= GetMetaTagKeyViewer();
	int iMetaHash = ListHelper::Instance()->List_Find(
		std::string(Config::HashKey),
		m_lstMetaTags,
		fnExtractor);

	if (iMetaHash == -1)
	{
		std::string szHashString = m_szParentCollection;
		std::vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
		for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
		{
			// This requires that the tags have an ordering. This ordering can be determined,
			// by the order of the MetaTag object in the collection object.
			szHashString += iter_Tags->second;
		}

		std::vector<Tag> lstMetaList = this->GetMetaTags(Visible);
		std::vector<Tag>::iterator iter_MetaTags = lstMetaList.begin();
		for (; iter_MetaTags != lstMetaList.end(); ++iter_MetaTags)
		{
			szHashString += iter_MetaTags->first + iter_MetaTags->second;
		}
		std::string szHash = Config::Instance()->GetHash(szHashString);
		SetMetaTag(Config::HashKey, szHash, Hidden);

		return szHash;
	}
	else
	{
		return m_lstMetaTags[iMetaHash].GetVal();
	}
}

std::string CopyItem::GetParent()
{
	return m_szParentCollection;
}

std::vector<std::string> CopyItem::GetResidentIn()
{
	return m_lstResidentIn;
}

void CopyItem::SetMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType)
{
	std::function<std::string(MetaTag)> fnExtractor =
		[](MetaTag atag1)-> std::string { return atag1.GetKey(); };
	int iFound = ListHelper::Instance()->List_Find(aszKey, m_lstMetaTags, fnExtractor);
	if (iFound == -1)
	{
		MetaTag newMeta(aszKey, aszVal, atagType);
		std::function<int(MetaTag, MetaTag)> fnComparer =
			[](MetaTag atag1, MetaTag atag2)-> int { return atag1.GetKey().compare(atag2.GetKey()); };
		ListHelper::Instance()->List_Insert(newMeta, m_lstMetaTags, fnComparer);
	}
	else if (m_lstMetaTags[iFound].CanView(atagType))
	{
		m_lstMetaTags[iFound].SetVal(aszVal);
	}
}

std::string CopyItem::GetMetaTag(std::string aszKey, MetaTagType atagType)
{
	std::function<std::string(MetaTag)> fnExtractor =
		[](MetaTag atag)-> std::string { return atag.GetKey(); };
	int iFound = ListHelper::Instance()->List_Find(aszKey, m_lstMetaTags, fnExtractor);
	if (iFound != -1)
	{
		return m_lstMetaTags[iFound].GetVal(atagType);
	}
	else
	{
		return Config::NotFoundString;
	}
}

std::vector<Tag> CopyItem::GetMetaTags(MetaTagType atagType)
{
	std::vector<Tag> lstRetVal;
	std::vector<MetaTag>::iterator iter_Tags = m_lstMetaTags.begin();
	for (; iter_Tags != m_lstMetaTags.end(); ++iter_Tags)
	{
		if (iter_Tags->CanView(atagType))
		{
			lstRetVal.push_back(std::make_pair(iter_Tags->GetKey(), iter_Tags->GetVal()));
		}
	}

	return lstRetVal;
}

bool CopyItem::SetIdentifyingAttribute(std::string aszKey, std::string aszValue)
{
	std::function<std::string(TraitItem)> fnExtractor = [](TraitItem aTI)->std::string { return aTI.GetKeyName(); };
	int iIsAttr = ListHelper::Instance()->List_Find(aszKey, *m_plstRestrictedTraits, fnExtractor);
	if (iIsAttr != -1)
	{
		TraitItem foundTrait = m_plstRestrictedTraits->at(iIsAttr);
		if (foundTrait.IsAllowedValue(aszValue))
		{
			m_lstIdentifyingTags[iIsAttr].second = aszValue;
			return true;
		}
	}

	return false;
}

std::string CopyItem::GetIdentifyingAttribute(std::string aszKey)
{
	int iFound = ListHelper::Instance()->List_Find(aszKey, m_lstIdentifyingTags, Config::Instance()->GetTagHelper(Key));
	if (iFound != -1)
	{
		return m_lstIdentifyingTags.at(iFound).second;
	}
	else
	{
		return Config::NotFoundString;
	}
}

std::vector<Tag> CopyItem::GetIdentifyingAttributes()
{
	std::vector<Tag> lstRetVal;
	std::vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
	for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
	{
		lstRetVal.push_back(std::make_pair(iter_Tags->first, iter_Tags->second));
	}

	return lstRetVal;
}

std::function<std::string(MetaTag)> CopyItem::GetMetaTagValueViewer(MetaTagType atagType)
{
	return [atagType](MetaTag atag)->std::string { return atag.GetVal(atagType); };
}

std::function<std::string(MetaTag)> CopyItem::GetMetaTagKeyViewer()
{
	return [](MetaTag atag)->std::string { return atag.GetKey(); };
}