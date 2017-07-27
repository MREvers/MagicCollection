#pragma once
#include <string>
#include <vector>
#include <functional>

#include "MetaTag.h"
#include "Config.h"
#include "StringHelper.h"
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
	bool IsParent(std::string aszParent);

	void AddResident(std::string aszNewResi);
	void RemoveResident(std::string aszRemoveResi);
	std::vector<std::string> GetResidentIn();
	bool IsResidentIn(std::string aszResident);

	void SetMetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType, bool bTimeChange = true);
	std::string GetMetaTag(std::string aszKey, MetaTagType atagType);
	std::vector<Tag> GetMetaTags(MetaTagType atagType);

	bool SetIdentifyingAttribute(std::string aszKey, std::string aszValue, bool bTimeChange = true);
	std::string GetIdentifyingAttribute(std::string aszKey);
	std::vector<Tag> GetIdentifyingAttributes();

	std::function<std::string(MetaTag)> GetMetaTagValueViewer(MetaTagType atagType);
	std::function<std::string(MetaTag)> GetMetaTagKeyViewer();

	static bool IsResidentIn(std::string aszResidentLocation, std::string aszTestCollection, unsigned int &riSubIn);
	static MetaTagType DetermineMetaTagType(std::string aszTagKey);
private:
	bool m_bNeedHash;

	// These are stored to save computation time.
	std::vector<unsigned int> m_lstSubAddresses;
	// Includes <Address>-<SubAddress1>,<SubAddress2>...
	// SubAddressX's smallest prime factor is the xth prime.
	std::string m_szAddress;
	std::string m_szFullAddress;
	std::vector<std::string> m_lstResidentIn;

	void itemChanged();
	void setChainID(std::string aszNewID);
	void setParent(std::string aszNewParent);
	void _setParent(std::string aszNewParent);
	void setSubAddress(unsigned int aiOldVal, unsigned int aiNewVal);

	// Metatags are visible by all collections. They 'may' be used to identify the card.
	// In general, meta-tags are attached to some physical meaning related to the card such as
	// aquisition date, price, etc...
	std::vector<MetaTag> m_lstMetaTags;

	// Note that these will correspond 1-1 to each traititem in m_plstRest...
	std::vector<Tag> m_lstIdentifyingTags;
	std::vector<TraitItem>* m_plstRestrictedTraits;
	void setPairedAttributes(std::string aszKey, int iVal);

	static bool isResidentIn(std::string aszBaseAddress, std::vector<unsigned int> alstSubAddresses, std::string aszTestAdress, unsigned int aiTestAddress, unsigned int &riSubIn);
	static bool isSuperSet(unsigned int aiSuperSet, unsigned int aiSubSet);
};

