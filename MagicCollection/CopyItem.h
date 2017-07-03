#pragma once
#include <string>
#include <vector>

#include "MetaTag.h"
#include "Config.h"

template void Config::List_Insert<MetaTag>(MetaTag&, std::vector<MetaTag>&, std::function<int(MetaTag, MetaTag)>);

class CopyItem
{
public:
	CopyItem();
	~CopyItem();

	std::string GetHash();

	void AddMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType);

	bool SetIdentifyingAttribute(std::string aszKey, std::string aszValue);

private:
	std::string m_szParentCollection;

	std::vector<MetaTag> m_lstMetaTags;
	std::vector<Tag> m_lstIdentifyingTags;
};

