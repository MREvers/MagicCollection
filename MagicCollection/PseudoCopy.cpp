
#include "PseudoCopy.h"

PseudoCopy::PseudoCopy(std::string aszCollectionName,
	CollectionSource* aoColSource,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	m_szCollectionName = aszCollectionName;
	m_ColSource = aoColSource;
	MetaList = alstMeta;
	FoundCardClass = false;
	FoundCardCopy = false;
}

PseudoCopy::PseudoCopy(std::string aszCollectionName,
	CollectionSource* aoColSource,
	std::string aszLongName,
	std::vector<std::pair<std::string, std::string>> alstMeta) :
	PseudoCopy(aszCollectionName, aoColSource, alstMeta)
{
	LongName = aszLongName;

	Ok = Collection::ParseCardLine(aszLongName, Count, Name, DetailsString);
	SuccessfulParse = Ok;

	if (Ok)
	{
		IdentifyingAttributes = Collection::ParseAttrs(DetailsString);
	}
}

PseudoCopy::PseudoCopy(std::string aszCollectionName,
	CollectionSource* aoColSource,
	std::string aszName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta) :
	PseudoCopy(aszCollectionName, aoColSource, alstMeta)
{
	Name = aszName;
	IdentifyingAttributes = alstAttrs;
	CacheIndex = -1;

	// Load card is needed to call toString.
	// Coerce OK to true since we don't need to parse the "LongName"
	Ok = true;
	Ok = LoadCard();
	if (Ok)
	{
		LongName = ToString();
		Ok = Collection::ParseCardLine(LongName, Count, Name, DetailsString);
		SuccessfulParse = Ok;
	}

}

bool PseudoCopy::LoadCard()
{
	if (Ok)
	{
		CacheIndex = m_ColSource->LoadCard(Name);
		Ok = CacheIndex != -1;
	}

	if (Ok)
	{
		Prototype = m_ColSource->GetCardPrototype(CacheIndex);
	}

	FoundCardClass = Ok;

	return Ok;
}

bool 
PseudoCopy::FindCopy()
{
	if (Ok)
	{
		Ok = Prototype->FindCopy(m_szCollectionName, IdentifyingAttributes, MetaList, RealCopy);
	}

	FoundCardCopy = Ok;

	return Ok;
}

CopyObject 
PseudoCopy::GenerateFalseCopy()
{
	if (FoundCardClass)
	{
		return Prototype->GenerateCopy(m_szCollectionName, IdentifyingAttributes, MetaList);
	}

	return Prototype->GenerateCopy(m_szCollectionName);
}

std::string
PseudoCopy::ToString(bool bFullDets, bool bIncludeMeta)
{
	CopyObject oTempCopy = GenerateFalseCopy();

	std::string szLine = "";
	if (Count > 1)
	{
		szLine += std::to_string(Count);
		szLine += "x ";
	}

	szLine += Name;
	szLine += " ";

	if (oTempCopy.NonUniqueTraits.size() > 0 ||
		(oTempCopy.ParentCollection != m_szCollectionName) || bFullDets)
	{
		szLine += "{ ";
		if (oTempCopy.ParentCollection != m_szCollectionName || bFullDets)
		{
			szLine += "Parent=\"";
			szLine += oTempCopy.ParentCollection;
			szLine += "\" ";
		}

		if (oTempCopy.NonUniqueTraits.size() > 0)
		{
			std::map<std::string, std::string>::iterator iter_keyValPairs = oTempCopy.NonUniqueTraits.begin();
			for (; iter_keyValPairs != oTempCopy.NonUniqueTraits.end(); ++iter_keyValPairs)
			{
				szLine += iter_keyValPairs->first;
				szLine += "=\"";
				szLine += iter_keyValPairs->second;
				szLine += "\" ";
			}

		}
		szLine += "}";
	}

	if (bIncludeMeta)
	{
		std::vector<std::pair<std::string, std::string>> lstVecMeta = oTempCopy.GetMetaTags(m_szCollectionName);
		if (lstVecMeta.size() > 0)
		{
			szLine += " : ";
			szLine += "{ ";
			std::vector<std::pair<std::string, std::string>>::iterator iter_keyValPairs = lstVecMeta.begin();
			for (; iter_keyValPairs != lstVecMeta.end(); ++iter_keyValPairs)
			{
				szLine += iter_keyValPairs->first;
				szLine += "=\"";
				szLine += iter_keyValPairs->second;
				szLine += "\" ";
			}
			szLine += "}";
		}
	}

	return szLine;
}