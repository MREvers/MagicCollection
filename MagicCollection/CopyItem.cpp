#include "CopyItem.h"



CopyItem::CopyItem()
{
}


CopyItem::~CopyItem()
{
}

std::string CopyItem::GetHash()
{
	std::function<std::string(MetaTag)> fnExtractor 
		= [](MetaTag tag)-> std::string { return tag.GetKey(); };
	int iMetaHash = Config::Instance()->List_Find(
		std::string("__Hash"),
		m_lstMetaTag,
		fnExtractor);

	if (iMetaHash != -1)
	{
		std::string szHashString = "";
		std::vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
		for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
		{
			// This requires that the tags have an ordering. This ordering can be determined,
			// by the order of the MetaTag object in the collection object.
			szHashString += iter_Tags->second;
		}

		std::vector<MetaTag>::iterator iter_MetaTags = m_lstMetaTag.begin();
		for (; iter_MetaTags != m_lstMetaTag.end(); ++iter_MetaTags)
		{
			szHashString += iter_MetaTags->GetKey() + iter_MetaTags->GetVal();
		}

		return Config::Instance()->GetHash(szHashString);
	}
	else
	{
		return m_lstMetaTag[iMetaHash].GetVal();
	}


}