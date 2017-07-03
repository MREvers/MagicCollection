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
		m_lstMetaTags,
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

		std::vector<MetaTag>::iterator iter_MetaTags = m_lstMetaTags.begin();
		for (; iter_MetaTags != m_lstMetaTags.end(); ++iter_MetaTags)
		{
			szHashString += iter_MetaTags->GetKey() + iter_MetaTags->GetVal();
		}

		return Config::Instance()->GetHash(szHashString);
	}
	else
	{
		return m_lstMetaTags[iMetaHash].GetVal();
	}
}

void CopyItem::AddMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType)
{
	MetaTag newMeta(aszKey, aszVal, atagType);
	std::function<int(MetaTag, MetaTag)> fnComparer =
		[](MetaTag atag1, MetaTag atag2)-> int {return atag1.GetKey().compare(atag2.GetKey()); };
	Config::Instance()->List_Insert(newMeta, m_lstMetaTags, fnComparer);
}