#pragma once
#pragma message ("Starting CollectionObject.h")

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "Config.h"
#include "StringHelper.h"
#include "CopyObject.h"

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class  CollectionObject
{
public:
	CollectionObject(std::string aszName);
	~CollectionObject();

	// Gets called by the collection Source. Implement in base class to get needed attributes.
	bool MapAttributes(std::string aszName, std::string aszValue);
	std::string GetAttribute(std::string aszAttr);
	std::vector<std::string> GetRestrictionListFor(std::string aszAttr);
	std::map<std::string, std::string>  GetAttributesMap();

	std::string GetName();

	CopyObject* AddCopy(std::string aszCollectionName);
	CopyObject* AddCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs);
	CopyObject* AddCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	// The col children will have to match exactly... because copies are not identical.
	bool RemoveCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	std::vector<CopyObject*> GetLocalCopies(std::string aszCollectionName);
	std::vector<CopyObject*> GetLocalCopiesWith(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs);

	// Gets all resi copies.
	std::vector<CopyObject*> GetCopies(std::string aszCollectionName);
	// Gets all resi copies resident in aszCollectionName, located in aszParent, with the correct attributes.
	std::vector<CopyObject*> GetCopiesWith(std::string aszCollectionName, std::string aszParent, std::vector<std::pair<std::string, std::string>> alstAttrs);
	// Gets a copy with matching attrs
	bool FindCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta,
		CopyObject*& roCO);


	std::map<std::string, std::vector<std::string>> GetNonUniqueAttributeRestrictions();
	void SetNonUniqueAttributeRestrictions(std::map<std::string, std::vector<std::string>> aMapRestrictions);
	void SetPairedNonUniqueAttrs(std::vector<std::pair<std::string, std::string>> alstPairedAttrs);

	CopyObject GenerateCopy(std::string aszCollectionName);
	// Used in building a printable structure
	CopyObject GenerateCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs);
	CopyObject GenerateCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	// Keep in mind that this does not compare names because the name of the card is not known by the copy object.
	// Additionally, it assumes you are comparing two cards of the same type.
	static bool IsSameIdentity(CopyObject* aoCOne, CopyObject* aoCTwo, bool bMatchParent = true);
	static bool IsSameMetaTags(std::string aszCollectionName, CopyObject* aoCOne, CopyObject* aoCTwo, bool abUseIgnore = true);
	static bool IsSameMetaTags(std::vector<std::pair<std::string, std::string>> alstTagsOne,
		std::vector<std::pair<std::string, std::string>> alstTagsTwo, bool abUseIgnore = true);
	static std::vector<std::pair<std::string, std::string>>
		FilterOutUniqueTraits(std::vector<std::pair<std::string, std::string>> alstAttrs);
private:
	int m_iAllCopies; // Used in assigning IDs.

	std::vector<CopyObject> m_lstCopies;
	std::map<std::string, std::string> m_mapAttributes;
	std::map<std::string, std::vector<std::string>> m_mapNonUniqueAttributesRestrictions;
	std::vector<std::pair<std::string, std::string>> m_lstPairedAttributes;
	std::string m_szName;

};

#pragma message ("Finish CollectionObject.h")