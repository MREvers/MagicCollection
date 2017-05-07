#pragma once
#include <vector>
#include <map>
#include <iterator>
#include <string>

#include "CollectionObject.h"
#include "CollectionSource.h"
#include "Collection.h"

/* Class PseudoCopy
*  Used to store information needed to identify/construct a real copy.
*/
class PseudoCopy
{
public:
	unsigned int Count;
	std::string DetailsString;
	std::string Name;
	std::string LongName;
	std::vector<std::pair<std::string, std::string>> IdentifyingAttributes;
	std::vector<std::pair<std::string, std::string>> MetaList;
	CopyObject* RealCopy;
	CollectionObject* Prototype;
	int CacheIndex;
	bool Ok;
	bool SuccessfulParse;
	bool FoundCardClass;
	bool FoundCardCopy;

	PseudoCopy(std::string aszCollectionName,
		CollectionSource* aoColSource,
		std::string aszLongName,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	PseudoCopy(std::string aszCollectionName,
		CollectionSource* aoColSource,
		std::string aszName,
		std::vector<std::pair<std::string, std::string>> alstAttrs,
		std::vector<std::pair<std::string, std::string>> alstMeta);

	bool LoadCard();

	bool FindCopy();

	CopyObject GenerateFalseCopy();

	std::string ToString(bool bFullDets = false, bool bIncludeMeta = false);

private:
	CollectionSource* m_ColSource;
	std::string m_szCollectionName;

	// Private Constructor for common features of the common constructors
	PseudoCopy(std::string aszCollectionName,
		CollectionSource* aoColSource,
		std::vector<std::pair<std::string, std::string>> alstMeta);
};