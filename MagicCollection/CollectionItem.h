#pragma once
#include <string>
#include "CopyItem.h"
#include "Config.h"
#include "TraitItem.h"
#include "StringHelper.h"

enum CollectionItemType : int
{
	Local = 0x1,
	Borrowed = 0x2,
	Real = 0x3,
	Virtual = 0x4,
	All = 0xF
};

class CollectionItem
{
public:
	class PseudoIdentifier
	{
	public:
		PseudoIdentifier();
		PseudoIdentifier(unsigned int aiCount, std::string aszName, std::string aszDetails, std::string aszMeta);
		~PseudoIdentifier();

		unsigned int Count;
		std::string Name;
		std::string DetailString;
		std::string MetaString;
		std::vector<Tag> Identifiers;
		std::vector<Tag> MetaTags;
	};

	CollectionItem(std::string aszItemName, std::vector<Tag> alstCommon, std::vector<TraitItem> alstRestrictions);
	~CollectionItem();

	std::string GetName();

	CopyItem* AddCopyItem(std::string aszCollectionName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>());

	CopyItem GenerateCopy(std::string aszCollectionName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>());

	void RemoveCopyItem(std::string aszHash);

	CopyItem* FindCopyItem(std::string aszHash);

	std::vector<CopyItem*> GetCopiesForCollection(std::string aszCollection, CollectionItemType aItemType);

	std::string GetHash(std::string aszCollectionName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>());

	std::string GetCardString(CopyItem* aItem, MetaTagType aTagType = Visible);
	std::string GetProtoTypeString();

	static bool ParseCardLine(std::string aszLine, PseudoIdentifier& rPIdentifier);
	static bool ParseTagString(std::string aszDetails, std::vector<Tag>& rlstTags);
	static std::string ToCardLine(std::string aszParentCollection, 
		std::string aszName,
		std::vector<Tag> alstAttrs = std::vector<Tag>(),
		std::vector<Tag> alstMetaTags = std::vector<Tag>());

private:
	std::string m_szName;

	std::vector<CopyItem> m_lstCopies;

	std::vector<Tag> m_lstCommonTraits;
	std::vector<TraitItem> m_lstIdentifyingTraits;
};

