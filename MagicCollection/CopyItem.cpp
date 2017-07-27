#include "CopyItem.h"
#include <time.h>
#include <sstream>
#include <cstdlib>

CopyItem::CopyItem(std::vector<TraitItem>* alstTraits, std::string aszParentCollection)
{
	m_lstResidentIn.push_back(aszParentCollection);
	m_plstRestrictedTraits = alstTraits;

	setParent(aszParentCollection);

	std::vector<TraitItem>::iterator iter_DefaultVals = m_plstRestrictedTraits->begin();
	for (; iter_DefaultVals != m_plstRestrictedTraits->end(); ++iter_DefaultVals)
	{
		m_lstIdentifyingTags.push_back(std::make_pair(iter_DefaultVals->GetKeyName(), iter_DefaultVals->GetDefaultValue()));
	}

	// Set the chain ID and session here. If one is set later, it will just overwrite this...

	setChainID(std::to_string(Config::Instance()->GetRandom()));
	itemChanged();

	m_bNeedHash = true;
}


CopyItem::~CopyItem()
{
	m_lstMetaTags.clear();
}

CopyItem::CopyItem(std::vector<TraitItem>* alstTraits,
	std::string aszParentCollection,
	std::vector<Tag> alstAttrs,
	std::vector<Tag> alstMetaTags)
	: CopyItem(alstTraits, aszParentCollection)
{
	std::vector<Tag>::iterator iter_Attrs = alstAttrs.begin();
	for (; iter_Attrs != alstAttrs.end(); ++iter_Attrs)
	{
		SetIdentifyingAttribute(iter_Attrs->first, iter_Attrs->second, false);
	}

	for (iter_Attrs = alstMetaTags.begin(); iter_Attrs != alstMetaTags.end(); ++iter_Attrs)
	{
		SetMetaTag(iter_Attrs->first, iter_Attrs->second, DetermineMetaTagType(iter_Attrs->first), false);
	}

	// Populate the Hash metatag for convenience.
	GetHash();
}

std::string CopyItem::GetHash()
{
	std::function<std::string(MetaTag)> fnExtractor
		= GetMetaTagKeyViewer();
	int iMetaHash = ListHelper::List_Find(
		std::string(Config::HashKey),
		m_lstMetaTags,
		fnExtractor);

	if (iMetaHash == -1 || m_bNeedHash)
	{
		std::string szHashString = m_szFullAddress;
		std::vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
		for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
		{
			// This requires that the tags have an ordering. This ordering can be determined,
			// by the order of the MetaTag object in the collection object.
			szHashString += iter_Tags->second;
		}

		std::vector<Tag> lstMetaList = this->GetMetaTags(Public);
		std::vector<Tag>::iterator iter_MetaTags = lstMetaList.begin();
		for (; iter_MetaTags != lstMetaList.end(); ++iter_MetaTags)
		{
			szHashString += iter_MetaTags->first + iter_MetaTags->second;
		}
		std::string szHash = Config::Instance()->GetHash(szHashString);
		SetMetaTag(Config::HashKey, szHash, Hidden, false);

		m_bNeedHash = false;
		return szHash;
	}
	else
	{
		return m_lstMetaTags[iMetaHash].GetVal();
	}
}

std::string  CopyItem::GetParent()
{
	return m_szFullAddress;
}

bool CopyItem::IsParent(std::string aszParent)
{
	std::pair<std::string, std::vector<unsigned int>> pairAddressAndSubs = Config::Instance()->GetIDInfo(aszParent);
	std::string szNewResiAddress = pairAddressAndSubs.first;
	unsigned int iNewResiSubAddress = pairAddressAndSubs.second[0];

	unsigned int iDummy;
	return isResidentIn(m_szAddress, m_lstSubAddresses, szNewResiAddress, iNewResiSubAddress, iDummy);
}

// This will detect if the adding 'resident' is a subset of the parent, if so, it will adjust the parent address.
void CopyItem::AddResident(std::string aszNewResi)
{
	Config* config = Config::Instance();

	std::pair<std::string, std::vector<unsigned int>> pairAddressAndSubs = config->GetIDInfo(aszNewResi);
	std::string szNewResiAddress = pairAddressAndSubs.first;
	unsigned int iNewResiSubAddress = pairAddressAndSubs.second[0];

	if (szNewResiAddress == m_szAddress)
	{
		// First check if the resident is a subset of the parent.
		// We always want the 'Parent location' of the item to be as specific as possible, because
		// the less specific locations will automatically pick up the more specific.
		unsigned int iSubAddressIn;
		bool bIsInSubset = isResidentIn(m_szAddress, m_lstSubAddresses, szNewResiAddress, iNewResiSubAddress, iSubAddressIn);

		// The 'subset of newResi'-location will suffice to say that this item is already in newResi.
		if (!bIsInSubset) 
		{

			// This item is not in a subset of newResi, but is newResi a more specific location than this item's
			for each (unsigned int iPotentialSuperSet in m_lstSubAddresses)
			{
				if (isSuperSet(iPotentialSuperSet, iNewResiSubAddress))
				{
					// This item is in a superset of new resi, and is now in new resi, so change the address.
					setSubAddress(iPotentialSuperSet, iNewResiSubAddress);
				}
			}

			// If we have gotten this far, then this is a new chain off the parent
			m_lstSubAddresses.push_back(iNewResiSubAddress);
		}
	}

	// Otherwise, check the resident locations - these are locations that reference this item.
	unsigned int iSubAddressIn;
	bool bFoundChain = false;
	for (size_t i = 0; i < m_lstResidentIn.size(); i++)
	{
		std::string szFullResiAddress = m_lstResidentIn[i];
		if (IsResidentIn(szFullResiAddress, aszNewResi, iSubAddressIn))
		{
			// This item is already referenced by a subset of newresi.
			bFoundChain = true;
			break;
		}
		else if (IsResidentIn(aszNewResi, szFullResiAddress, iSubAddressIn))
		{
			m_lstResidentIn[i] = aszNewResi;
			bFoundChain = true;
			break;
		}
	}

	if (!bFoundChain)
	{
		m_lstResidentIn.push_back(aszNewResi);
	}
}

void CopyItem::RemoveResident(std::string aszRemoveResi)
{
	std::pair<std::string, std::vector<unsigned int>> pairAddSub = Config::Instance()->GetIDInfo(aszRemoveResi);
	std::string szAddress = pairAddSub.first;
	int iRemoveSubAddress = pairAddSub.second[0];

	if (szAddress == m_szAddress)
	{
		for (size_t i = 0; i < m_lstSubAddresses.size(); i++)
		{
			int iSubAddress = m_lstSubAddresses[i];
			if (isSuperSet(iRemoveSubAddress, iSubAddress))
			{
				int iHighPrime = Config::primes[Config::Instance()->GetHighPrimeIndex(iRemoveSubAddress)];
				int iNewSubAddress = iRemoveSubAddress / iHighPrime;

				if (iNewSubAddress == 1 && iNewSubAddress == iRemoveSubAddress)
				{
					iNewSubAddress = 0;
				}

				setSubAddress(iSubAddress, iNewSubAddress);

				if (m_lstSubAddresses.size() == 0)
				{
					// We are now a virtual item.
					setParent("");
				}
			}
		}
	}

	auto iter_Resi = m_lstResidentIn.begin();
	for (; iter_Resi != m_lstResidentIn.end(); ++iter_Resi)
	{
		std::string szResident = *iter_Resi;
		if (szResident == aszRemoveResi)
		{
			m_lstResidentIn.erase(iter_Resi);
			return;
		}

		unsigned int iDummy;
		if (IsResidentIn(aszRemoveResi, szResident, iDummy))
		{
			int iHighPrime = Config::primes[Config::Instance()->GetHighPrimeIndex(iRemoveSubAddress)];
			int iNewSubAddress = iRemoveSubAddress / iHighPrime;

			m_lstResidentIn.erase(iter_Resi);

			std::string szNewAddress = szAddress + "-" + std::to_string(iNewSubAddress);
			return;
		}
	}
}

std::vector<std::string> CopyItem::GetResidentIn()
{
	return m_lstResidentIn;
}

bool CopyItem::IsResidentIn(std::string aszResident)
{
	bool bSimple = false;
	unsigned int iDummy;

	for each (std::string szResi in m_lstResidentIn)
	{
		if (bSimple) { break; }

		bSimple |= IsResidentIn(szResi, aszResident, iDummy);
	}

	return bSimple;
}

// Collection names have the form <Address>-<SubAddress>... only one subaddress.
bool CopyItem::IsResidentIn(std::string aszResidentLocation, std::string aszCollectionName, unsigned int &riSubIn)
{
	Config* config = Config::Instance();

	std::pair<std::string, std::vector<unsigned int>> pairSource = Config::Instance()->GetIDInfo(aszResidentLocation);
	std::string szBaseAddress = pairSource.first;
	std::vector<unsigned int> lstSubAddresses = pairSource.second;

	std::pair<std::string, std::vector<unsigned int>> pairEnt = Config::Instance()->GetIDInfo(aszCollectionName);
	std::string szTestAddress = pairEnt.first;
	int iSubAddress = pairEnt.second[0];

	return isResidentIn(szBaseAddress, lstSubAddresses, szTestAddress, iSubAddress, riSubIn);
}

MetaTagType CopyItem::DetermineMetaTagType(std::string aszTagKey)
{
	MetaTagType mTagType = Public;
	if (aszTagKey.size() > 0 && aszTagKey[0] == '_')
	{
		mTagType = Ignored;
	}
	if (mTagType == Ignored && aszTagKey.size() > 1 && aszTagKey[1] == '_')
	{
		mTagType = Tracking;
	}
	return mTagType;
}

bool CopyItem::isResidentIn(std::string aszBaseAddress,
	std::vector<unsigned int> alstSubAddresses,
	std::string aszTestAdress,
	unsigned int aiTestSubAddress,
	unsigned int &riSubIn)
{
	Config* config = Config::Instance();
	riSubIn = 1;

	bool bIsResident = true;

	if (!(bIsResident &= aszTestAdress == aszBaseAddress)) { return bIsResident; }

	bool bFoundSubAddressMatch = false;
	for each (int subAddress in alstSubAddresses)
	{
		// Since the subAddress is the MOST specific location, it will be the LARGER than iSubAddress, if
		// iSubAddress contains any item in subAddress.
		bFoundSubAddressMatch |= isSuperSet(aiTestSubAddress, subAddress);
		riSubIn = subAddress;
		if (bFoundSubAddressMatch) { break; }
	}

	return bIsResident &= bFoundSubAddressMatch;
}

bool CopyItem::isSuperSet(unsigned int aiSuperSet, unsigned int aiSubSet)
{
	if (aiSubSet == aiSuperSet) { return true; }
	Config* config = Config::Instance();

	// The subset will have a larger code, ie 30. The superset will be, e.g. 6.
	if (aiSubSet % aiSuperSet == 0)
	{
		int iSmallPrime = config->GetPrimeIndex(aiSubSet / aiSuperSet);
		int iSuperLargePrime = config->GetHighPrimeIndex(aiSuperSet);

		return iSmallPrime >= iSuperLargePrime;
	}
	else
	{
		return false;
	}
}

void CopyItem::setSubAddress(unsigned int aiOldVal, unsigned int aiNewVal)
{
	int iAddressIndex = ListHelper::List_Find(aiOldVal, m_lstSubAddresses);
	if (iAddressIndex == -1) { return; }

	int iAlreadyIndex = ListHelper::List_Find(aiNewVal, m_lstSubAddresses);
	if (iAlreadyIndex != -1)
	{
		m_lstSubAddresses.erase(m_lstSubAddresses.begin() + iAddressIndex);
	}
	else if (aiNewVal != 0)
	{
		m_lstSubAddresses[iAddressIndex] = aiNewVal;
	}
	else
	{
		m_lstSubAddresses.erase(m_lstSubAddresses.begin() + iAddressIndex);
	}

	std::string szNewFullAddress = m_szAddress + "-";
	bool bFirst = true;
	for each (unsigned int iSubAddress in m_lstSubAddresses)
	{
		if (!bFirst) { szNewFullAddress += ","; }
		szNewFullAddress += std::to_string(iSubAddress);

		bFirst = false;
	}

	m_szFullAddress = szNewFullAddress;
}

void CopyItem::SetMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType, bool bTimeChange)
{
	if (bTimeChange) { itemChanged(); }

	std::function<int(MetaTag, MetaTag)> fnComparer;
	std::function<std::string(MetaTag)> fnExtractor =
		[](MetaTag atag1)-> std::string { return atag1.GetKey(); };

	if (aszKey == "Address") { _setParent(aszVal); }

	int iFound = ListHelper::List_Find(aszKey, m_lstMetaTags, fnExtractor);
	if (iFound == -1)
	{
		fnComparer = [](MetaTag atag1, MetaTag atag2)-> int { return atag1.GetKey().compare(atag2.GetKey()); };

		MetaTag newMeta(aszKey, aszVal, atagType);
		ListHelper::List_Insert(newMeta, m_lstMetaTags, fnComparer);
	}
	else if (m_lstMetaTags[iFound].CanView(atagType))
	{
		m_lstMetaTags[iFound].SetVal(aszVal);
	}
	m_bNeedHash = true;
}

std::string CopyItem::GetMetaTag(std::string aszKey, MetaTagType atagType)
{
	std::function<std::string(MetaTag)> fnExtractor =
		[](MetaTag atag)-> std::string { return atag.GetKey(); };
	int iFound = ListHelper::List_Find(aszKey, m_lstMetaTags, fnExtractor);
	if (iFound != -1)
	{
		return m_lstMetaTags[iFound].GetVal(atagType);
	}
	else
	{
		return Config::NotFoundString;
	}
}

std::vector<Tag> CopyItem::GetMetaTags(MetaTagType atagType)
{
	std::vector<Tag> lstRetVal;
	std::vector<MetaTag>::iterator iter_Tags = m_lstMetaTags.begin();
	for (; iter_Tags != m_lstMetaTags.end(); ++iter_Tags)
	{
		if (iter_Tags->CanView(atagType))
		{
			lstRetVal.push_back(std::make_pair(iter_Tags->GetKey(), iter_Tags->GetVal()));
		}
	}

	return lstRetVal;
}

bool CopyItem::SetIdentifyingAttribute(std::string aszKey, std::string aszValue, bool bTimeChange)
{
	if (bTimeChange) { itemChanged(); }

	std::function<std::string(TraitItem)> fnExtractor = [](TraitItem aTI)->std::string { return aTI.GetKeyName(); };
	int iIsAttr = ListHelper::List_Find(aszKey, *m_plstRestrictedTraits, fnExtractor);
	if (iIsAttr != -1)
	{
		TraitItem foundTrait = m_plstRestrictedTraits->at(iIsAttr);
		if (foundTrait.IsAllowedValue(aszValue))
		{
			m_lstIdentifyingTags[iIsAttr].second = aszValue;
			int iValueIndex = ListHelper::List_Find(aszValue, foundTrait.GetAllowedValues());
			setPairedAttributes(aszKey, iValueIndex);
			m_bNeedHash = true;
			return true;
		}
	}

	return false;
}

std::string CopyItem::GetIdentifyingAttribute(std::string aszKey)
{
	int iFound = ListHelper::List_Find(aszKey, m_lstIdentifyingTags, Config::Instance()->GetTagHelper(Key));
	if (iFound != -1)
	{
		return m_lstIdentifyingTags.at(iFound).second;
	}
	else
	{
		return Config::NotFoundString;
	}
}

std::vector<Tag> CopyItem::GetIdentifyingAttributes()
{
	std::vector<Tag> lstRetVal;
	std::vector<Tag>::iterator iter_Tags = m_lstIdentifyingTags.begin();
	for (; iter_Tags != m_lstIdentifyingTags.end(); ++iter_Tags)
	{
		lstRetVal.push_back(std::make_pair(iter_Tags->first, iter_Tags->second));
	}

	return lstRetVal;
}

std::function<std::string(MetaTag)> CopyItem::GetMetaTagValueViewer(MetaTagType atagType)
{
	return [atagType](MetaTag atag)->std::string { return atag.GetVal(atagType); };
}

std::function<std::string(MetaTag)> CopyItem::GetMetaTagKeyViewer()
{
	return [](MetaTag atag)->std::string { return atag.GetKey(); };
}

void CopyItem::itemChanged()
{
	long time = std::time(nullptr);
	std::stringstream ss;
	ss << std::hex << time;
	SetMetaTag("__Session", ss.str(), MetaTagType::Tracking, false);
}

void CopyItem::setChainID(std::string aszNewID)
{
	SetMetaTag("__ChainID", aszNewID, MetaTagType::Tracking);
}

void CopyItem::setParent(std::string aszNewParent)
{
	SetMetaTag("Address", aszNewParent, MetaTagType::Public);
}

void CopyItem::_setParent(std::string aszNewParent)
{
	m_szFullAddress = aszNewParent;
	std::pair<std::string, std::vector<unsigned int>> pID = Config::Instance()->GetIDInfo(aszNewParent);
	m_lstSubAddresses = pID.second;
	m_szAddress = pID.first;
	AddResident(aszNewParent);
}

void CopyItem::setPairedAttributes(std::string aszKey, int iVal)
{
	std::function<std::string(TraitItem)> fnExtractor = [](TraitItem aTI)->std::string { return aTI.GetKeyName(); };
	std::vector<std::string> lstPartners;
	std::vector<Tag> lstPairs = Config::Instance()->GetPairedKeysList();
	for each (Tag var in lstPairs)
	{
		if (var.first == aszKey && ListHelper::List_Find(var.second, lstPartners) == -1)
		{
			lstPartners.push_back(var.second);
		}
		else if (var.second == aszKey && ListHelper::List_Find(var.first, lstPartners) == -1)
		{
			lstPartners.push_back(var.first);
		}
	}

	for each (std::string szKey in lstPartners)
	{
		int iIsAttr = ListHelper::List_Find(szKey, *m_plstRestrictedTraits, fnExtractor);
		if (iIsAttr != -1)
		{
			TraitItem foundTrait = m_plstRestrictedTraits->at(iIsAttr);
			m_lstIdentifyingTags[iIsAttr].second = foundTrait.GetAllowedValues().at(iVal);
		}
	}
}