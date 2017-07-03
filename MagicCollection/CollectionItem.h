#pragma once
#include <string>
#include "CopyObject.h"
#include "Config.h"

class CollectionItem
{
public:
	CollectionItem(std::string aszItemName);
	~CollectionItem();

	std::string GetName();

	CopyObject* AddCopyItem(std::string aszCollectionName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>());

	CopyObject* FindCopyItem(std::string aszHash);

private:
	std::string m_szName;

	std::vector<CopyObject> m_lstCopies;
};

