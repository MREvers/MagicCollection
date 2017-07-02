#pragma once

#include <string>
#include <vector>
#include <map>

#include "Config.h"
#include "StringHelper.h"
#include "ListHelper.h"

class CollectionObject;

class CopyObject
{
public:
	std::string ParentCollection;
	std::vector<std::string> ResidentCollections;

	std::map<std::string, std::vector<std::pair<std::string, std::string>>> PerCollectionMetaTags;
	std::vector<std::pair<std::string, std::string>> MetaTags;

	std::map<std::string, std::string> NonUniqueTraits;

	CopyObject(std::string aszParent,
		std::vector<std::pair<std::string, std::string>>* alstPairedTags,
		std::map<std::string, std::vector<std::string>>* alstRestrictions);

	// Non-unique in the sense that the trait itself can have more than one value.
	std::vector<std::pair<std::string, std::string>> GetNonUniqueTraits(bool bWithParent) const;

	// Attr Controlling Function
	void SetPairedTraitsReference(std::vector<std::pair<std::string, std::string>>* aLstPairedTraits);
	void SetNonUniqueAttributesRestrictionsReference(std::map<std::string, std::vector<std::string>>* aMapNonUAttrRestr);

	void AddResidentCollection(std::string aszCollectionName);

	// Meta Tag Controlling Functions
	std::vector<std::pair<std::string, std::string>> GetMetaTags(std::string aszCollection) const;
	std::pair<std::string, std::string> GetMetaTag(std::string aszCollection, std::string aszKey);
	bool IsPerCollectionTag(std::string aszKeyName);
	void RemoveMetaTag(std::string aszCollection, std::string aszKey);
	void SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszVal);
	void SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszSubKey, std::string aszVal);
	bool HasMetaTag(std::string aszKey);
	bool HasPerCollectionTag(std::string aszCollection, std::string aszKey);

	void SetNonUniqueAttr(std::string aszKey, std::string aszValue);

	static std::string GetHash(CopyObject const * aoHashing);

private:
	std::vector<std::pair<std::string, std::string>>* m_LstPairedTraits;
	std::map<std::string, std::vector<std::string>>* m_mapNonUniqueAttributesRestrictions;

};
