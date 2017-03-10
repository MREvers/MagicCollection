#pragma once
#pragma message ("Starting CollectionObject.h")

#include "ICollectionObject.h"
#include "ICollection.h"
#include "SourceObject.h"


#include <string>
#include <vector>
#include <map>


class CopyObject
{
public:
	static std::vector<std::string> PerCollectionMetaTagNames;
	CopyObject(
		std::string aszParent,
		std::vector<std::pair<std::string, std::string>>* alstPairedTags,
		std::map<std::string, std::vector<std::string>>* alstRestrictions);
	std::string ParentCollection;
	std::vector<std::string> ResidentCollections;

	std::map<std::string, std::vector<std::pair<std::string, std::string>>> PerCollectionMetaTags;
	std::vector<std::pair<std::string, std::string>> MetaTags;

	// Non-unique in the sense that the trait itself can have more than one value.
	std::map<std::string, std::string> NonUniqueTraits;

	// Attr Controlling Function
	void SetPairedTraitsReference(std::vector<std::pair<std::string, std::string>>* aLstPairedTraits);
	void SetNonUniqueAttributesRestrictionsReference(std::map<std::string, std::vector<std::string>>* aMapNonUAttrRestr);

	// Meta Tag Controlling Functions
	std::vector<std::pair<std::string, std::string>> GetMetaTags(std::string aszCollection);
   std::pair<std::string, std::string> GetMetaTag(std::string aszCollection, std::string aszKey);
	bool IsPerCollectionTag(std::string aszKeyName);
	void RemoveMetaTag(std::string aszCollection, std::string aszKey);
	void SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszVal);
	void SetMetaTag(std::string aszCollection, std::string aszKey, std::string aszSubKey, std::string aszVal);
	bool HasMetaTag(std::string aszKey);
	bool HasPerCollectionTag(std::string aszCollection, std::string aszKey);

	void SetNonUniqueAttr(std::string aszKey, std::string aszValue);

	static bool IsSameMetaTags(std::string aszCollectionName, CopyObject* aoCOne, CopyObject* aoCTwo, bool abUseIgnore = true);
	static bool IsSameMetaTags(
		std::vector<std::pair<std::string, std::string>> alstTagsOne,
		std::vector<std::pair<std::string, std::string>> alstTagsTwo, bool abUseIgnore = true);

private:
	std::vector<std::pair<std::string, std::string>>* m_LstPairedTraits;
	std::map<std::string, std::vector<std::string>>* m_mapNonUniqueAttributesRestrictions;

};

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class  CollectionObject : public ICollectionObject
{
public:
	CollectionObject(std::string aszName);
	~CollectionObject();

	// Gets called by the collection Source. Implement in base class to get needed attributes.
	virtual bool MapAttributes(std::string aszName, std::string aszValue);
	std::string GetAttribute(std::string aszAttr);
	std::vector<std::string> GetRestrictionListFor(std::string aszAttr);
	std::map<std::string, std::string>  GetAttributesMap();

	// Collection Interface
	std::string GetName();

	CopyObject* AddCopy(std::string aszCollectionName);
	CopyObject* AddCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs);

	// The col children will have to match exactly... because copies are not identical.
	void RemoveCopy(std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	std::vector<CopyObject*> GetLocalCopies(std::string aszCollectionName);
	std::vector<CopyObject*> GetLocalCopiesWith(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs);

	// Gets all resi copies.
	std::vector<CopyObject*> GetCopies(std::string aszCollectionName);
	// Gets all resi copies resident in aszCollectionName, located in aszParent, with the correct attributes.
	std::vector<CopyObject*> GetCopiesWith(std::string aszCollectionName, std::string aszParent, std::vector<std::pair<std::string, std::string>> alstAttrs);
	// Gets a copy with matching attrs
	bool GetCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs, CopyObject*& roCO, bool abExact = true);
	bool GetCopy(
		std::string aszCollectionName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta,
		CopyObject*& roCO);

	
	std::map<std::string, std::vector<std::string>> GetNonUniqueAttributeRestrictions();
	void SetNonUniqueAttributeRestrictions(std::map<std::string, std::vector<std::string>> aMapRestrictions);
   void SetPairedNonUniqueAttrs(std::vector<std::pair<std::string, std::string>> alstPairedAttrs);
	
	CopyObject GenerateCopy(std::string aszCollectionName);
	// Used in building a printable structure
	CopyObject GenerateCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs);
   CopyObject GenerateCopy(
      std::string aszCollectionName,
      std::vector<std::pair<std::string, std::string>> alstAttrs,
      std::vector<std::pair<std::string, std::string>> alstMeta);
	// Keep in mind that this does not compare names because the name of the card is not known by the copy object.
	// Additionally, it assumes you are comparing two cards of the same type.
	static bool IsSameIdentity(CopyObject* aoCOne, CopyObject* aoCTwo, bool bMatchParent = true);
	static std::vector<std::pair<std::string, std::string>> 
		FilterOutUniqueTraits(std::vector<std::pair<std::string, std::string>> alstAttrs);
	static std::vector<std::pair<std::string, std::string>> 
		ConvertMapToList(std::map<std::string, std::string>  aMap);
	static bool IsUniqueTrait(std::string aszTrait);
	static bool IsNonUniqueTrait(std::string aszTrait);
	static bool CompareKeyValPairList(std::vector<std::pair<std::string, std::string>> alstFirst,
		std::vector<std::pair<std::string, std::string>> alstSecond);
	static std::string str_trim( const std::string& aszTrim, char removeChar);

	static const char * const LstUniqueTraits[];
	static const char * const LstNonUniqueTraits[];
private:
	int m_iAllCopies; // Used in assigning IDs.

	std::vector<CopyObject> m_lstCopies;
	std::map<std::string, std::string> m_mapAttributes;
	std::map<std::string, std::vector<std::string>> m_mapNonUniqueAttributesRestrictions;
	std::vector<std::pair<std::string, std::string>> m_lstPairedAttributes;
	std::string m_szName;

};

#pragma message ("Finish CollectionObject.h")