#pragma once
#include <string>
#include <vector>
#include <functional>

#include "MetaTag.h"
#include "Config.h"
#include "TraitItem.h"

template void ListHelper::List_Insert<MetaTag>(MetaTag&, std::vector<MetaTag>&, std::function<int(MetaTag, MetaTag)>);

class CopyItem
{
public:
	CopyItem(std::vector<TraitItem>* alstTraits,
		std::string aszParentCollection);
	CopyItem(std::vector<TraitItem>* alstTraits,
		std::string aszParentCollection,
		std::vector<Tag> alstAttrs,
		std::vector<Tag> alstMetaTags = std::vector<Tag>());
	~CopyItem();

	std::string GetHash();

	std::string GetParent();
	std::vector<std::string> GetResidentIn();

	void SetMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType);
	std::string GetMetaTag(std::string aszKey, MetaTagType atagType);
	std::vector<Tag> GetMetaTags(MetaTagType atagType);

	bool SetIdentifyingAttribute(std::string aszKey, std::string aszValue);
	std::string GetIdentifyingAttribute(std::string aszKey);
	std::vector<Tag> GetIdentifyingAttributes();

	std::function<std::string(MetaTag)> GetMetaTagValueViewer(MetaTagType atagType);
	std::function<std::string(MetaTag)> GetMetaTagKeyViewer();

private:
	std::string m_szParentCollection;
	std::vector<std::string> m_lstResidentIn;

	std::vector<MetaTag> m_lstMetaTags;
	std::vector<Tag> m_lstIdentifyingTags;

	std::vector<TraitItem>* m_plstRestrictedTraits;
};

