#pragma once
#include <string>
#include <vector>

#include "MetaTag.h"
#include "Config.h"

class CopyItem
{
public:
	CopyItem();
	~CopyItem();

	std::string GetHash();

private:
	std::string m_szParentCollection;

	std::vector<MetaTag> m_lstMetaTag;
	std::vector<Tag> m_lstIdentifyingTags;
};

