#include "Collection.h"

Collection::PseudoCopy::PseudoCopy(std::string aszCollectionName,
	CollectionSource* aoColSource,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	m_szCollectionName = aszCollectionName;
	m_ColSource = aoColSource;
	MetaList = alstMeta;
	FoundCardClass = false;
	FoundCardCopy = false;
}

Collection::PseudoCopy::PseudoCopy(std::string aszCollectionName,
	CollectionSource* aoColSource,
	std::string aszLongName,
	std::vector<std::pair<std::string, std::string>> alstMeta) :
	PseudoCopy(aszCollectionName, aoColSource, alstMeta)
{
	LongName = aszLongName;

	Ok = ParseCardLine(aszLongName, Count, Name, DetailsString);

	if (Ok)
	{
		IdentifyingAttributes = ParseAttrs(DetailsString);
	}
}

Collection::PseudoCopy::PseudoCopy(std::string aszCollectionName,
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
		Ok = ParseCardLine(LongName, Count, Name, DetailsString);
	}

}

bool Collection::PseudoCopy::LoadCard()
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

bool Collection::PseudoCopy::FindCopy()
{
	if (Ok)
	{
		Ok = Prototype->FindCopy(m_szCollectionName, IdentifyingAttributes, MetaList, RealCopy);
	}

	FoundCardCopy = Ok;

	return Ok;
}

CopyObject Collection::PseudoCopy::GeneratePseudoCopy()
{
	if (Ok)
	{
		return Prototype->GenerateCopy(m_szCollectionName, IdentifyingAttributes, MetaList);
	}

	return Prototype->GenerateCopy(m_szCollectionName);
}

std::string Collection::PseudoCopy::ToString(bool bFullDets, bool bIncludeMeta)
{
	CopyObject oTempCopy = GeneratePseudoCopy();

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

Collection::Transaction::Transaction(Collection* aoCol)
{
	m_Col = aoCol;
	Recordable = true;

}

Collection::Transaction::~Transaction()
{
	Actions.clear();
}

void Collection::Transaction::AddAction(Action& aoAct)
{
	if (IsOpen)
	{
		Actions.push_back(aoAct);
	}

}

void Collection::Transaction::RemoveAction(int i)
{
	if (IsOpen)
	{
		Actions.erase(Actions.begin() + i);
	}
}

void Collection::Transaction::Finalize(bool abRecordable)
{
	if (IsOpen)
	{
		IsOpen = false;
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Do();
		}
		Recordable = abRecordable;
	}

}

void Collection::Transaction::Rollback()
{
	if (!IsOpen)
	{
		int iSize = Actions.size();
		for (int i = 0; i < iSize; i++)
		{
			Actions.at(i).Undo();
		}
	}
}


Collection::Collection(std::string aszName, CollectionSource* aoSource, std::vector<std::string>* alstLoadedCollections)
{
	m_ColSource = aoSource;
	m_szName = aszName;
	m_szParentName = "";
	m_szHistoryFileName = m_szName + ".history";
	m_szMetaTagFileName = m_szName + ".metatags";
	m_lstLoadedCollectionsBuffer = alstLoadedCollections;
}

Collection::Collection(std::string aszName,
	std::string aszParentName,
	CollectionSource* aoSource,
	std::vector<std::string>* alstLoadedCollections) : Collection(aszName, aoSource, alstLoadedCollections)
{
	m_szParentName = aszParentName;
}

Collection::~Collection()
{
}

std::string Collection::GetName()
{
	return m_szName;
}

void Collection::AddItem(std::string aszNewItem,
	bool bFinal,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(alstAttrs, aszNewItem, alstMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.FoundCardClass)
	{
		// Get the transaction
		Collection::Transaction* oTrans = openTransaction();

		// Initialize the identifier
		std::string szIdentifier = "+ " + oPseudoTarget.ToString();

		std::function<void()> fnDo;
		fnDo = std::bind(&Collection::addItem, this, aszNewItem, alstAttrs, alstMeta);

		std::function<void()> fnUndo;
		fnUndo = std::bind(&Collection::removeItem, this, aszNewItem, alstAttrs, alstMeta);

		Collection::Action oAction;
		oAction.Identifier = szIdentifier; //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
		oAction.Do = fnDo;
		oAction.Undo = fnUndo;

		oTrans->AddAction(oAction);

		if (bFinal)
		{
			oTrans->Finalize();
		}
	}
}

void Collection::AddItem(std::string aszNewItemLongName,
	std::vector<std::pair<std::string, std::string>> alstMetaTags)
{
	std::string szName;
	int iCount;
	std::string szDetails;
	if (Collection::ParseCardLine(aszNewItemLongName, iCount, szName, szDetails))
	{
		auto lstAttrs = Collection::ParseAttrs(szDetails);
		AddItem(szName, true, lstAttrs, alstMetaTags);
	}
}

void Collection::RemoveItem(std::string aszRemoveItem,
	bool bFinal,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{

	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(alstAttrs, aszRemoveItem, alstMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		// Get the transaction
		Collection::Transaction* oTrans = openTransaction();

		std::string szIdentifier = "- " + oPseudoTarget.ToString();

		std::function<void()> fnDo;
		fnDo = std::bind(&Collection::removeItem, this, aszRemoveItem, alstAttrs, alstMeta);

		std::function<void()> fnUndo;
		fnUndo = std::bind(&Collection::addItem, this, aszRemoveItem, alstAttrs, alstMeta);

		Collection::Action oAction;
		oAction.Identifier = szIdentifier; //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
		oAction.Do = fnDo;
		oAction.Undo = fnUndo;

		oTrans->AddAction(oAction);

		if (bFinal)
		{
			oTrans->Finalize();
		}
	}

}

void
Collection::RemoveItem(std::string aszNewItemLongName,
	std::vector<std::pair<std::string, std::string>> alstMetaTags)
{
	std::string szName;
	int iCount;
	std::string szDetails;
	if (Collection::ParseCardLine(aszNewItemLongName, iCount, szName, szDetails))
	{
		auto lstAttrs = Collection::ParseAttrs(szDetails);
		RemoveItem(szName, true, lstAttrs, alstMetaTags);
	}
}

void
Collection::ReplaceItem(std::string aszRemoveItemLongName,
	std::vector<std::pair<std::string, std::string>> alstIdentifyingMeta,
	std::string aszAddItemLongName,
	std::vector<std::pair<std::string, std::string>> alstNewMeta,
	bool bFinal)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszRemoveItemLongName, alstIdentifyingMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		PseudoCopy oPseudoNew = generatePseudoCopy(aszAddItemLongName, alstNewMeta);

		if (oPseudoNew.Ok) { oPseudoNew.LoadCard(); }
		if (oPseudoNew.Ok && oPseudoTarget.Ok)
		{
			// Get the transaction
			Collection::Transaction* oTrans = openTransaction();

			std::string szIdentifier = "% " + oPseudoTarget.ToString(true, true) + " -> " + oPseudoNew.ToString(true, true);

			std::function<void()> fnDo;
			fnDo = std::bind(&Collection::replaceItem, this,
				oPseudoNew.Name,
				oPseudoNew.IdentifyingAttributes,
				oPseudoNew.MetaList,
				oPseudoTarget.Name,
				oPseudoTarget.IdentifyingAttributes,
				oPseudoTarget.MetaList);

			std::function<void()> fnUndo;
			fnUndo = std::bind(&Collection::replaceItem, this,
				oPseudoTarget.Name,
				oPseudoTarget.IdentifyingAttributes,
				oPseudoTarget.MetaList,
				oPseudoNew.Name,
				oPseudoNew.IdentifyingAttributes,
				oPseudoNew.MetaList);

			Collection::Action oAction;
			oAction.Identifier = szIdentifier; //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
			oAction.Do = fnDo;
			oAction.Undo = fnUndo;

			oTrans->AddAction(oAction);

			if (bFinal) { oTrans->Finalize(); }
		}
	}

}

void
Collection::RemoveMetaTag(
	std::string aszLongName,
	std::string aszKey,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName, alstMatchMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		bool bValidTag = false;
		std::string szOldVal = oPseudoTarget.RealCopy->GetMetaTag(m_szName, aszKey).second;
		bValidTag = szOldVal != "!NULL";

		if (bValidTag)
		{
			// Get the transaction
			Collection::Transaction* oTrans = openTransaction();

			// Initialize the identifier
			std::string szIdentifier;

			szIdentifier = "Removed Meta-Tag '" + aszKey + "' from " + aszLongName;// + szCard;

			std::function<void()> fnDo;
			fnDo = std::bind(&Collection::removeMetaTag,
				this, oPseudoTarget.RealCopy, aszKey);

			std::function<void()> fnUndo;
			fnUndo = std::bind(&Collection::setMetaTag,
				this, oPseudoTarget.RealCopy, aszKey, szOldVal);

			Collection::Action oAction;
			oAction.Identifier = szIdentifier; //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
			oAction.Do = fnDo;
			oAction.Undo = fnUndo;

			oTrans->AddAction(oAction);

			oTrans->Finalize();
		}
	}
}

void Collection::SetMetaTag(
	std::string aszLongName,
	std::string aszKey,
	std::string aszValue,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta,
	bool bFinal)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName, alstMatchMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		// Get the transaction
		Collection::Transaction* oTrans = openTransaction();

		// Initialize the identifier
		std::string szIdentifier = "Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;

		// Store the action and how to undo it here.
		std::function<void()> fnDo;
		fnDo = std::bind(&Collection::setMetaTag, this, oPseudoTarget.RealCopy, aszKey, aszValue);

		std::function<void()> fnUndo;
		std::string szOldVal = oPseudoTarget.RealCopy->GetMetaTag(m_szName, aszKey).second;
		if (szOldVal != "!NULL") // Key Doesnt exist
		{
			fnUndo = std::bind(&Collection::setMetaTag,
				this, oPseudoTarget.RealCopy, aszKey, szOldVal);
		}
		else
		{
			fnUndo = std::bind(&Collection::removeMetaTag,
				this, oPseudoTarget.RealCopy, aszKey);
		}

		Collection::Action oAction;
		oAction.Identifier = szIdentifier; //"Set Meta-Tag '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;// + szCard;
		oAction.Do = fnDo;
		oAction.Undo = fnUndo;

		oTrans->AddAction(oAction);

		if (bFinal)
		{
			oTrans->Finalize();
		}
	}
}

void Collection::SetMetaTags(std::string aszLongName,
	std::vector<std::pair<std::string, std::string>> alstKeyVals,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName, alstMatchMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		Collection::Transaction* oTrans = openTransaction();

		// Store the action and how to undo it here.
		// Initialize the identifier
		std::string szIdentifier;

		szIdentifier = "Set Meta-Tags ";// + szCard;
		for (int i = 0; i < alstKeyVals.size(); i++)
		{
			szIdentifier += alstKeyVals[i].first + " to " + alstKeyVals[i].second;
			if (i == alstKeyVals.size() - 1)
			{
				szIdentifier += " on " + aszLongName + ".";
			}
			else
			{
				szIdentifier += ", and ";
			}
		}

		std::function<void()> fnDo;
		fnDo = std::bind(&Collection::setMetaTags, this, oPseudoTarget.RealCopy, alstKeyVals);

		std::function<void()> fnUndo;
		std::vector<std::pair<std::string, std::string>> lstUndoList;
		std::vector<std::pair<std::string, std::string>> lstOldMetaTags = oPseudoTarget.RealCopy->GetMetaTags(m_szName);
		std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags = lstOldMetaTags.begin();
		for (; iter_MetaTags != lstOldMetaTags.end(); ++iter_MetaTags)
		{
			lstUndoList.push_back(std::make_pair(iter_MetaTags->first, iter_MetaTags->second));
		}
		fnUndo = std::bind(&Collection::setMetaTags, this, oPseudoTarget.RealCopy, lstUndoList);

		Collection::Action oAction;
		oAction.Identifier = szIdentifier;
		oAction.Do = fnDo;
		oAction.Undo = fnUndo;

		oTrans->AddAction(oAction);

		oTrans->Finalize();
	}
}

void Collection::SetNonUniqueAttribute(
	std::string aszLongName,
	std::string aszKey,
	std::string aszValue,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName, alstMatchMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		Collection::Transaction* oTrans = openTransaction();

		// Store the action and how to undo it here.
		// Initialize the identifier
		std::string szIdentifier;

		// Check if the attribute is a valid attribute
		bool bValidChange = false;
		bool bValidAttribute = false;
		auto iter_UniqueTrait = oPseudoTarget.RealCopy->NonUniqueTraits.find(aszKey);
		bValidAttribute = iter_UniqueTrait != oPseudoTarget.RealCopy->NonUniqueTraits.end();

		std::vector<std::string> lstDiffKeys;
		if (bValidAttribute)
		{
			// Simulate the change
			CopyObject pseudoCopy = oPseudoTarget.GeneratePseudoCopy();
			setItemAttr(&pseudoCopy, aszKey, aszValue);

			// Check for differences
			auto iter_PrevTraits = oPseudoTarget.RealCopy->NonUniqueTraits.begin();
			for (; iter_PrevTraits != oPseudoTarget.RealCopy->NonUniqueTraits.end(); ++iter_PrevTraits)
			{
				auto iter_NewTraits = pseudoCopy.NonUniqueTraits.find(iter_PrevTraits->first);
				if (iter_NewTraits != oPseudoTarget.RealCopy->NonUniqueTraits.end())
				{
					if (iter_NewTraits->second != iter_PrevTraits->second)
					{
						lstDiffKeys.push_back(iter_PrevTraits->first);
					}
				}
			}

			// Record the differences
			if (lstDiffKeys.size() > 0)
			{
				bValidChange = true;
			}
		}

		if (bValidChange)
		{
			szIdentifier = "Setting attribute '" + aszKey + "' to '" + aszValue + "' on " + aszLongName;

			std::function<void()> fnDo;
			fnDo = std::bind(&Collection::setItemAttr, this, oPseudoTarget.RealCopy, aszKey, aszValue);

			std::function<void()> fnUndo;
			std::string szOldVal = iter_UniqueTrait->second;
			fnUndo = std::bind(&Collection::setItemAttr, this, oPseudoTarget.RealCopy, aszKey, szOldVal);

			Collection::Action oAction;
			oAction.Identifier = szIdentifier;
			oAction.Do = fnDo;
			oAction.Undo = fnUndo;

			oTrans->AddAction(oAction);

			oTrans->Finalize();
		}
	}
}

void Collection::SetFeatures(
	std::string aszLongName,
	std::vector<std::pair<std::string, std::string>> alstNewMeta,
	std::vector<std::pair<std::string, std::string>> alstNewAttrs,
	std::vector<std::pair<std::string, std::string>> alstMatchMeta,
	bool bFinal)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName, alstMatchMeta);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		Collection::Transaction* oTrans = openTransaction();

		// Store the action and how to undo it here.
		// Initialize the identifier
		std::string szIdentifier;

		// Check if the attribute is a valid attribute
		bool bValidChange = false;
		bool bValidAttribute = true;
		for (int i = 0; i < alstNewAttrs.size(); i++)
		{
			auto iter_UniqueTrait = oPseudoTarget.RealCopy->NonUniqueTraits.find(alstNewAttrs[i].first);
			bValidAttribute &= iter_UniqueTrait != oPseudoTarget.RealCopy->NonUniqueTraits.end();
		}

		// All meta tags are valid so if the attributes are valid then so is the change
		std::vector<std::string> lstDiffKeys;
		if (bValidAttribute)
		{
			// Simulate the change
			CopyObject pseudoCopy = oPseudoTarget.GeneratePseudoCopy();
			setFeatures(&pseudoCopy, alstNewMeta, alstNewAttrs);

			// Check for differences
			auto iter_PrevTraits = oPseudoTarget.RealCopy->NonUniqueTraits.begin();
			for (; iter_PrevTraits != oPseudoTarget.RealCopy->NonUniqueTraits.end(); ++iter_PrevTraits)
			{
				auto iter_NewTraits = pseudoCopy.NonUniqueTraits.find(iter_PrevTraits->first);
				if (iter_NewTraits != pseudoCopy.NonUniqueTraits.end())
				{
					if (iter_NewTraits->second != iter_PrevTraits->second)
					{
						lstDiffKeys.push_back(iter_PrevTraits->first);
					}
				}
				else
				{
					lstDiffKeys.push_back(iter_PrevTraits->first);
				}
			}

			// Record the differences
			if (lstDiffKeys.size() > 0)
			{
				bValidChange = true;
			}

			if (bValidChange)
			{
				szIdentifier += "Set Attribute(s) ";
				for (int i = 0; i < lstDiffKeys.size(); i++)
				{
					szIdentifier += lstDiffKeys[i] + " to " + pseudoCopy.NonUniqueTraits[lstDiffKeys[i]];
					if (i != lstDiffKeys.size() - 1)
					{
						szIdentifier += ", and ";
					}
					else
					{
						szIdentifier += " on " + aszLongName + ".";
					}
				}

				std::function<void()> fnDo;
				fnDo = std::bind(&Collection::setFeatures, this, oPseudoTarget.RealCopy, alstNewMeta, alstNewAttrs);

				std::function<void()> fnUndo;
				std::vector<std::pair<std::string, std::string>> lstUndoList;
				std::vector<std::pair<std::string, std::string>> lstOldMetaTags = oPseudoTarget.RealCopy->GetMetaTags(m_szName);
				std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTags = lstOldMetaTags.begin();
				for (; iter_MetaTags != lstOldMetaTags.end(); ++iter_MetaTags)
				{
					lstUndoList.push_back(std::make_pair(iter_MetaTags->first, iter_MetaTags->second));
				}
				fnUndo = std::bind(&Collection::setFeatures, this, oPseudoTarget.RealCopy, lstUndoList, oPseudoTarget.IdentifyingAttributes);

				Collection::Action oAction;
				oAction.Identifier = szIdentifier;
				oAction.Do = fnDo;
				oAction.Undo = fnUndo;

				oTrans->AddAction(oAction);

				oTrans->Finalize();
			}
		}
	}
}

std::vector<std::string> Collection::GetNonUniqueAttributeRestrictions(std::string aszLongName, std::string aszKey)
{
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		auto mapRestrictions = oPseudoTarget.Prototype->GetNonUniqueAttributeRestrictions();
		if (mapRestrictions.find(aszKey) != mapRestrictions.end())
		{
			return mapRestrictions[aszKey];
		}

	}
	std::vector<std::string> lstNullRetVal;
	lstNullRetVal.push_back("*");
	return lstNullRetVal;
}

std::vector<std::vector<std::pair<std::string, std::string>>> Collection::GetMetaTags(std::string aszLongName)
{
	std::vector<std::vector<std::pair<std::string, std::string>>> lstRetVal;
	// Check if all targets exist.
	PseudoCopy oPseudoTarget = generatePseudoCopy(aszLongName);

	if (oPseudoTarget.Ok) { oPseudoTarget.LoadCard(); }
	if (oPseudoTarget.Ok) { oPseudoTarget.FindCopy(); }
	if (oPseudoTarget.Ok)
	{
		std::vector<CopyObject*> LstCopies = oPseudoTarget.Prototype->GetCopies(m_szName);
		std::vector<CopyObject*>::iterator iter_Copies = LstCopies.begin();
		for (; iter_Copies != LstCopies.end(); ++iter_Copies)
		{
			if (CollectionObject::CompareKeyValPairList(
				CollectionObject::FilterOutUniqueTraits(oPseudoTarget.IdentifyingAttributes),
				CollectionObject::ConvertMapToList((*iter_Copies)->NonUniqueTraits)))
			{
				lstRetVal.push_back((*iter_Copies)->GetMetaTags(m_szName));
			}
		}

	}

	return lstRetVal;
}


void Collection::setName(std::string aszName)
{
	m_szName = aszName;
	m_szHistoryFileName = aszName + ".history";
}

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::addItem(std::string aszNewItem,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	forceAdd(aszNewItem, alstAttrs, alstMeta);
}

CopyObject* Collection::forceAdd(std::string aszNewItem,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	int iCard = m_ColSource->LoadCard(aszNewItem);
	if (iCard != -1)
	{
		registerItem(iCard);

		// Get a copy of this card.
		CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

		// Add a copy of this card. Save the reference if we need to add unique traits.
		CopyObject* oCO = oCard->AddCopy(m_szName, alstAttrs, alstMeta);

		// if this is a child collection, add to the parent if parent == this
		// alstAttrs overwrites m_szName in AddCopy if parent is present
		if (m_szParentName != "")
		{
			if (oCO->ParentCollection == m_szName)
			{
				oCO->ParentCollection = m_szParentName;
			}

			// Make sure this collection and the parent collection are both in its residents.
			oCO->AddResidentCollection(m_szParentName);
			oCO->AddResidentCollection(m_szName);

			m_ColSource->NotifyNeedToSync(m_szName);
		}

		return oCO;
	}
}

void Collection::registerItem(int aiItem)
{
	// Make sure we don't already 'have' this card.
	bool bNewCard = true;
	for (int i = 0; i < getCollectionList().size(); i++)
	{
		bNewCard &= getCollectionList().at(i) != aiItem;
	}

	if (bNewCard)
	{
		m_lstCollection.push_back(aiItem);
	}
}

Collection::PseudoCopy Collection::generatePseudoCopy(std::string aszLongName, std::vector<std::pair<std::string, std::string>> alstMeta)
{
	return PseudoCopy(m_szName, m_ColSource, aszLongName, alstMeta);
}

Collection::PseudoCopy Collection::generatePseudoCopy(
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::string aszName,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	return PseudoCopy(m_szName, m_ColSource, aszName, alstAttrs, alstMeta);
}

void Collection::setTransactionsNoWrite()
{
	std::vector<Transaction>::iterator iter_Trans = m_lstTransactions.begin();
	for (; iter_Trans != m_lstTransactions.end(); iter_Trans++)
	{
		iter_Trans->Recordable = false;
	}
}

std::vector<int>& Collection::getCollectionList()
{
	if (m_ColSource->IsSyncNeeded(m_szName))
	{
		m_lstCollection = m_ColSource->GetCollectionCache(m_szName, false);
	}

	return m_lstCollection;
}

bool Collection::removeItem(std::string aszRemoveItem,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMeta)
{
	int iCard = m_ColSource->LoadCard(aszRemoveItem);
	if (iCard != -1)
	{
		// Get a copy of this card.
		CollectionObject* oCard = m_ColSource->GetCardPrototype(iCard);

		// Add a copy of this card.
		return oCard->RemoveCopy(m_szName, alstAttrs, alstMeta);
	}
	return false;
}

void Collection::replaceItem(std::string aszNewItem,
	std::vector<std::pair<std::string, std::string>> alstNewAttrs,
	std::vector<std::pair<std::string, std::string>> alstNewMeta,
	std::string aszOldItem,
	std::vector<std::pair<std::string, std::string>> alstOldAttrs,
	std::vector<std::pair<std::string, std::string>> alstOldMeta)
{
	int iRemoveCard = m_ColSource->LoadCard(aszOldItem);
	int iAddCard = m_ColSource->LoadCard(aszNewItem);
	if (iRemoveCard != -1 && iAddCard != -1)
	{
		// If we successfully removed the card, add the replacement card.
		if (removeItem(aszOldItem, alstOldAttrs, alstOldMeta))
		{
			forceAdd(aszNewItem, alstNewAttrs, alstNewMeta);
		}
	}
}

std::string Collection::changeItemAttribute_string(
	std::string aszCardname,
	CopyObject* aoCO,
	std::string aszKey,
	std::string aszNewVal,
	bool bIsParentCol)
{
	PseudoCopy oBefore = generatePseudoCopy(
		CollectionObject::ConvertMapToList(aoCO->NonUniqueTraits),
		aszCardname);
	std::string szBefore = oBefore.ToString();

	CopyObject oDummyCopy = oBefore.GeneratePseudoCopy();
	setItemAttr(&oDummyCopy, aszKey, aszNewVal);

	PseudoCopy oAfter = generatePseudoCopy(
		CollectionObject::ConvertMapToList(oDummyCopy.NonUniqueTraits),
		aszCardname);
	std::string szAfter = oAfter.ToString();

	return "% " + szBefore + " -> " + szAfter;
}

void Collection::loadCollectionFromFile(std::string aszFileName)
{
	// Load the collection
	std::ifstream in(aszFileName);
	std::stringstream buffer;
	buffer << in.rdbuf();
	std::string contents(buffer.str());

	std::vector<std::string> lstLines = StringHelper::SplitIntoLines(contents);

	loadCollectionLines(lstLines);
}

void Collection::loadCollectionLines(std::vector<std::string>& alstLines)
{
	for (std::vector<std::string>::iterator iter_CardLine = alstLines.begin(); iter_CardLine != alstLines.end(); ++iter_CardLine)
	{
		this->loadCardLine(*iter_CardLine);
	}

	finalizeTransaction(false);
}

bool Collection::loadCardLine(std::string aszNewCardLine)
{
	int iNum;
	std::string szName = "";
	std::string szDetails;
	if (!ParseCardLine(aszNewCardLine, iNum, szName, szDetails))
	{
		m_lstUnreversibleChanges.push_back("Could Not Parse Line \"" + aszNewCardLine + "\"");
		return false;
	}

	if (szName == "")
	{
		return false;
	}

	std::vector<std::pair<std::string, std::string>> lstKeyVals = ParseAttrs(szDetails);

	// Add or find a copy for each count.
	for (int i = 0; i < iNum; i++)
	{
		bool bNeedNewCopy = true;
		int iCardIndex = m_ColSource->LoadCard(szName);
		if (iCardIndex != -1)
		{
			// Create a temporary copy that will be used in comparisons
			CollectionObject* ColPrototype = m_ColSource->GetCardPrototype(iCardIndex);

			// Keep in mind, that lstKeyVals may overwrite m_szName as the parent collection.
			CopyObject oTargetCopy = ColPrototype->GenerateCopy(m_szName, lstKeyVals);

			// Look for a copy that is identical to this copy and check whether it is
			// currently uncounted/unincluded in this collection. If a copy is found,
			// add this collection to its resident collection.

			// Check for all existing copies in the collection that the target copy claims it is in.
			bool bNeedNewCopy = true;
			std::vector<CopyObject*> lstExistingCopies = m_ColSource
				->GetCardPrototype(iCardIndex)
				->GetLocalCopies(oTargetCopy.ParentCollection);
			std::vector<CopyObject*>::iterator iter_ExistingCopy = lstExistingCopies.begin();
			for (; iter_ExistingCopy != lstExistingCopies.end(); ++iter_ExistingCopy)
			{
				// The only way a copy could possibly have this collection as a resi. is if this collection created it
				//  or used it. If that is the case, then we should not use it as the copy.
				if (Config::GetConfigClass()->List_Find(m_szName, (*iter_ExistingCopy)->ResidentCollections) == -1)
				{
					// This copy is not used by this collection. Check if this copy is identical to the target copy.
					if (CollectionObject::IsSameIdentity(&oTargetCopy, *iter_ExistingCopy))
					{
						// Record that this item is in this collection.
						// Normally this is done during the "AddItem" call, but
						// if the copy was loaded by another collection, it must
						// be done here.
						registerItem(iCardIndex);

						// Record this collection in this copy's residents.
						(*iter_ExistingCopy)->ResidentCollections.push_back(m_szName);
						bNeedNewCopy = false;
						break;
					}
				}
			}

			// If we didn't find a copy that matches, generate a new one.
			if (bNeedNewCopy)
			{
				// Just add the copy if it is in this collection or
				// the collection is loaded.
				if (oTargetCopy.ParentCollection == m_szName ||
					(Config::GetConfigClass()->List_Find(oTargetCopy.ParentCollection, *m_lstLoadedCollectionsBuffer) == -1))
				{
					AddItem(szName, false, lstKeyVals);
				}
				else
				{
					// If the parent collection is loaded, and a copy was not found in that collection,
					// add a modified copy where the parent is ""
					std::vector<std::pair<std::string, std::string>> lstKeyValsFixed(lstKeyVals);
					std::vector<std::pair<std::string, std::string>>::iterator iter_keyvals = lstKeyValsFixed.begin();
					for (; iter_keyvals != lstKeyValsFixed.end(); ++iter_keyvals)
					{
						if (iter_keyvals->first == "Parent")
						{
							iter_keyvals->second = "";

							// Record somewhere that this change was made.
							m_lstUnreversibleChanges.push_back(changeItemAttribute_string(szName, &oTargetCopy, "Parent", ""));
							break;
						}
					}

					AddItem(szName, false, lstKeyValsFixed);
				}

			} // End If need new copy

		} // End card name exists
		else
		{
			// Can't find the card in the source... PANIC
			m_lstUnreversibleChanges.push_back("Could Not Find Card \"" + szName + "\"");
		}

	} // End for Card name in file

	return false;
}

bool Collection::loadRemoveCardLine(std::string aszRemoveCardLine)
{
	// Separate the long name from the meta tags
	std::vector<std::string> lstLongNameAndMeta = StringHelper::Str_Split(aszRemoveCardLine, ":");
	if (lstLongNameAndMeta.size() == 2)
	{
		bool bValidInput = true;
		std::string szRemoveName;
		int iCount;
		std::string szDetails;
		std::vector<std::pair<std::string, std::string>> lstRemoveAttrs;
		if (bValidInput &= Collection::ParseCardLine(lstLongNameAndMeta[0], iCount, szRemoveName, szDetails))
		{
			lstRemoveAttrs = Collection::ParseAttrs(szDetails);
		}

		if (bValidInput)
		{
			std::vector<std::pair<std::string, std::string>> lstIdentifyingMeta =
				ParseAttrs(lstLongNameAndMeta[1]);
			RemoveItem(szRemoveName, false, lstRemoveAttrs, lstIdentifyingMeta);
		}
	}

	return false;
}

bool Collection::loadChangeCardLine(std::string aszChangeCardLine)
{
	// Determine if this is a replacement or a change
	// Expects LongName : {Meta} -> LongName : {Meta}
	// First split the line
	std::vector<std::string> lstBeforeAfter = StringHelper::Str_Split(aszChangeCardLine, "->");
	if (lstBeforeAfter.size() == 2)
	{
		// Get the meta tag information
		std::vector<std::string> lstBeforeLongNameAndMeta = StringHelper::Str_Split(lstBeforeAfter[0], ":");
		std::vector<std::string> lstAfterLongNameAndMeta = StringHelper::Str_Split(lstBeforeAfter[1], ":");
		if (lstBeforeLongNameAndMeta.size() >= 1 &&
			lstAfterLongNameAndMeta.size() >= 1)
		{
			// Parse the meta tags
			std::vector<std::pair<std::string, std::string>> lstRemoveMetaTags;
			if (lstBeforeLongNameAndMeta.size() > 1)
			{
				lstRemoveMetaTags =
					ParseAttrs(lstBeforeLongNameAndMeta[1]);
			}

			std::vector<std::pair<std::string, std::string>> lstAddMetaTags;
			if (lstAfterLongNameAndMeta.size() > 1)
			{
				lstAddMetaTags =
					ParseAttrs(lstAfterLongNameAndMeta[1]);
			}

			PseudoCopy oRemoveTarget = generatePseudoCopy(lstBeforeLongNameAndMeta[0], lstRemoveMetaTags);
			PseudoCopy oAddTarget = generatePseudoCopy(lstAfterLongNameAndMeta[0], lstAddMetaTags);

			if (oRemoveTarget.Ok) { oRemoveTarget.LoadCard(); }
			if (oRemoveTarget.Ok) { oRemoveTarget.FindCopy(); }
			std::string szRemoveLong;
			if (oRemoveTarget.Ok)
			{
				szRemoveLong = oRemoveTarget.ToString();
			}

			if (oAddTarget.Ok) { oAddTarget.LoadCard(); }
			if (oAddTarget.Ok) { oAddTarget.FindCopy(); }
			std::string szAddLong;
			if (oAddTarget.Ok)
			{
				szRemoveLong = oAddTarget.ToString();
			}

			if (oRemoveTarget.Ok && oAddTarget.Ok)
			{
				if (oAddTarget.Name != oRemoveTarget.Name)
				{
					// Its a replacement
					// Call the replace function
					ReplaceItem(oRemoveTarget.LongName,
						oRemoveTarget.MetaList,
						oAddTarget.LongName,
						oAddTarget.MetaList,
						false);
				}
				else
				{
					// Its a change
					SetFeatures(oRemoveTarget.LongName ,
						oAddTarget.MetaList,
						oAddTarget.IdentifyingAttributes,
						oRemoveTarget.MetaList,
						false);
				}
			}

		}
	}

	return false;
}

void Collection::refreshCopyLinks(std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges)
{
	// Now that the collection is loaded, if there are cards with parent collection == this but
	// this is not in their residents, then that copy no longer exists.
	std::vector<std::pair<std::string, CopyObject*>> lstFullCol = m_ColSource->GetCollection(m_szName);
	std::vector<std::pair<std::string, CopyObject*>> ::iterator iter_ColOs = lstFullCol.begin();
	for (; iter_ColOs != lstFullCol.end(); ++iter_ColOs)
	{
		if (Config::GetConfigClass()->List_Find(m_szName, (*iter_ColOs).second->ResidentCollections) == -1)
		{
			std::vector<std::string> lstAffectedCols = (*iter_ColOs).second->ResidentCollections;
			std::vector<std::string>::iterator iter_affected_col = lstAffectedCols.begin();
			for (; iter_affected_col != lstAffectedCols.end(); ++iter_affected_col)
			{
				alstOutsideForcedChanges.push_back(
					std::make_pair(
						*iter_affected_col,
						changeItemAttribute_string((*iter_ColOs).first, (*iter_ColOs).second, "Parent", "", false)
					));
			}
			(*iter_ColOs).second->ParentCollection = "";
		}
	}
}

void Collection::loadMetaTagsFromFile()
{
	// Now load meta tags
	std::ifstream in(m_szMetaTagFileName + ".txt");
	if (in.good())
	{
		std::stringstream buffer;
		buffer << in.rdbuf();
		std::string contents(buffer.str());

		std::vector<std::string> lstLines = StringHelper::SplitIntoLines(contents);

		loadMetaTagLines(lstLines);
	}
}

void Collection::loadMetaTagLines(std::vector<std::string>& alstLines)
{
	for (std::vector<std::string>::iterator iter = alstLines.begin(); iter != alstLines.end(); ++iter)
	{
		std::vector<std::string> LstSplitLine = StringHelper::Str_Split(*iter, ":");
		if (LstSplitLine.size() == 2)
		{
			std::string szTempName;
			std::string szTempDetails;
			int iTaggedCards;
			if (ParseCardLine(LstSplitLine[0], iTaggedCards, szTempName, szTempDetails))
			{
				std::string szKey = "";
				std::string szVal = "";
				std::vector<std::pair<std::string, std::string>> LstSplitMeta = ParseAttrs(LstSplitLine[1]);
				if (LstSplitMeta.size() > 0)
				{
					// Store the tags so that
					// There are no MatchMeta because no cards have meta yet. I.e. NO META TAGS
					for (int k = 0; k < iTaggedCards; k++)
					{
						std::vector<std::pair<std::string, std::string>> LstBlank;
						std::vector<std::pair<std::string, std::string>>::iterator iter_NewMeta = LstSplitMeta.begin();
						for (; iter_NewMeta != LstSplitMeta.end(); ++iter_NewMeta)
						{
							SetMetaTag(LstSplitLine[0], iter_NewMeta->first, iter_NewMeta->second, LstBlank, false);
						}
					} // End for number of copies
				}
			} // End if successful parse

		}
	}

	finalizeTransaction(false);
}

void Collection::setItemAttr(
	CopyObject* aoCO,
	std::string aszKey,
	std::string aszNewVal)
{
	aoCO->SetNonUniqueAttr(aszKey, aszNewVal);
}

void Collection::setItemAttrs(
	CopyObject* aoCO,
	std::vector<std::pair<std::string, std::string>> alstKeyVals)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyVals = alstKeyVals.begin();
	for (; iter_KeyVals != alstKeyVals.end(); ++iter_KeyVals)
	{
		aoCO->SetNonUniqueAttr(iter_KeyVals->first, iter_KeyVals->second);
	}
}


void Collection::setMetaTag(
	CopyObject* aoCO,
	std::string aszKey,
	std::string aszNewTag)
{
	aoCO->SetMetaTag(m_szName, aszKey, aszNewTag);
}

void Collection::setMetaTags(
	CopyObject* aoCO,
	std::vector<std::pair<std::string, std::string>> alstKeyVals)
{
	std::vector<std::pair<std::string, std::string>>::iterator iter_KeyVals = alstKeyVals.begin();
	for (; iter_KeyVals != alstKeyVals.end(); ++iter_KeyVals)
	{
		aoCO->SetMetaTag(m_szName, iter_KeyVals->first, iter_KeyVals->second);
	}
}

void  Collection::removeMetaTag(
	CopyObject* aoCO,
	std::string aszKey)
{
	aoCO->RemoveMetaTag(m_szName, aszKey);
}

void Collection::setFeatures(CopyObject* aoCO,
	std::vector<std::pair<std::string, std::string>> alstNewMeta,
	std::vector<std::pair<std::string, std::string>> alstNewAttrs)
{
	setMetaTags(aoCO, alstNewMeta);
	setItemAttrs(aoCO, alstNewAttrs);
}

void Collection::LoadCollection(std::string aszFileName, std::vector<std::pair<std::string, std::string>>& alstOutsideForcedChanges)
{
	this->loadCollectionFromFile(aszFileName);

	this->refreshCopyLinks(alstOutsideForcedChanges);

	this->loadMetaTagsFromFile();
}

void Collection::SaveCollection(std::string aszFileName)
{
	std::vector<std::string> lstLines = GetCollectionList();

	if (m_lstUnreversibleChanges.size() > 0 || m_lstTransactions.size() > 0)
	{
		std::string szTimeString = "";
		time_t now = time(0);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char str[26];
		asctime_s(str, sizeof str, &timeinfo);
		str[strlen(str) - 1] = 0;


		std::vector<std::string> lstHistoryLines;

		std::vector<std::string>::iterator iter_addit = m_lstUnreversibleChanges.begin();
		for (; iter_addit != m_lstUnreversibleChanges.end(); ++iter_addit)
		{
			lstHistoryLines.push_back(*iter_addit);
		}

		std::vector<Transaction>::iterator iter_transactions = m_lstTransactions.begin();
		for (; iter_transactions != m_lstTransactions.end(); ++iter_transactions)
		{
			if (iter_transactions->Recordable)
			{
				std::vector<Action>::iterator iter_actions = iter_transactions->Actions.begin();
				for (; iter_actions != iter_transactions->Actions.end(); ++iter_actions)
				{
					lstHistoryLines.push_back(iter_actions->Identifier);
				}
				iter_transactions->Recordable = false;
			}
		}

		if (lstHistoryLines.size() > 0)
		{
			std::ofstream oHistFile;
			oHistFile.open(m_szHistoryFileName + ".txt", std::ios_base::app);

			oHistFile << "[" << str << "] " << std::endl;

			std::vector<std::string>::iterator iter_histLines = lstHistoryLines.begin();
			for (; iter_histLines != lstHistoryLines.end(); ++iter_histLines)
			{
				oHistFile << *iter_histLines << std::endl;
			}

			oHistFile.close();
			m_lstUnreversibleChanges.clear();
		}

	}

	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
		LstCardWithMeta = GetCollectionListWithMeta();
	std::ofstream oMetaFile;
	oMetaFile.open(m_szMetaTagFileName + ".txt");
	for (int i = 0; i < LstCardWithMeta.size(); i++)
	{
		if (LstCardWithMeta[i].second.size() > 0)
		{
			std::string szTagLine = ": { ";
			std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTag = LstCardWithMeta[i].second.begin();
			for (; iter_MetaTag != LstCardWithMeta[i].second.end(); ++iter_MetaTag)
			{
				szTagLine += iter_MetaTag->first + "=\"" + iter_MetaTag->second + "\" ";
			}
			szTagLine += "}";
			oMetaFile << LstCardWithMeta[i].first << szTagLine << "\n";
		}
	}

	oMetaFile.close();

	std::ofstream oColFile;
	oColFile.open(aszFileName);
	oColFile << ": Name=\"" + m_szName + "\" \n";

	if (m_szParentName != "")
	{
		oColFile << ": Parent=\"" + m_szParentName + "\" \n";
	}

	for (int i = 0; i < lstLines.size(); i++)
	{
		std::cout << lstLines[i] << std::endl;
		oColFile << lstLines[i] << "\n";
	}

	oColFile.close();
}

void Collection::LoadBulkChanges(std::vector<std::string>& alstBulkChanges)
{
	// + or nothing is an add.
	// - is a remove
	// % is a change or replacement
	std::vector<std::string> lstAddLines;
	std::vector<std::string> lstRemoveLines;
	std::vector<std::string> lstChangeLines;

	// Sort the lines
	std::vector<std::string>::iterator iter_Line = alstBulkChanges.begin();
	for (; iter_Line != alstBulkChanges.end(); ++iter_Line)
	{
		// Clean the line
		std::string szLine = StringHelper::Str_Trim(*iter_Line, ' ');

		// Check the first character and sort
		if (szLine.size() > 0)
		{
			if ((szLine[0] == '+') || (szLine[0] == '-') || (szLine[0] == '%'))
			{
				std::string szLineShort = StringHelper::Str_Trim(szLine.substr(1, szLine.size() - 1), ' ');
				if (szLine[0] == '+')
				{
					lstAddLines.push_back(szLineShort);
				}
				else if (szLine[0] == '-')
				{
					lstRemoveLines.push_back(szLineShort);
				}
				else if (szLine[0] == '%')
				{
					lstChangeLines.push_back(szLineShort);
				}
			}
			else
			{
				lstAddLines.push_back(szLine);
			}
		}
	} // End for each line

	iter_Line = lstAddLines.begin();
	for (; iter_Line != lstAddLines.end(); ++iter_Line)
	{
		loadCardLine(*iter_Line);
	}

	iter_Line = lstRemoveLines.begin();
	for (; iter_Line != lstRemoveLines.end(); ++iter_Line)
	{
		loadRemoveCardLine(*iter_Line);
	}

	iter_Line = lstChangeLines.begin();
	for (; iter_Line != lstChangeLines.end(); ++iter_Line)
	{
		loadChangeCardLine(*iter_Line);
	}

	finalizeTransaction(true);
}

void Collection::RecordForcedTransaction(std::string aszTransactionString)
{
	m_lstUnreversibleChanges.push_back(aszTransactionString);
}

void Collection::CreateBaselineHistory()
{
	std::vector<std::string> lstLines = GetCollectionList();

	std::string szTimeString = "";
	time_t now = time(0);
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);
	char str[26];
	asctime_s(str, sizeof str, &timeinfo);
	str[strlen(str) - 1] = 0;

	std::ofstream oHistFile;
	oHistFile.open(m_szHistoryFileName + ".txt");

	oHistFile << "[" << str << "] " << std::endl;

	std::vector<std::string>::iterator iter_lines = lstLines.begin();
	for (; iter_lines != lstLines.end(); ++iter_lines)
	{
		oHistFile << "+ " << *iter_lines << "\n";
	}

	oHistFile.close();

	setTransactionsNoWrite();
}

void Collection::PrintList()
{
	for (int i = 0; i < getCollectionList().size(); i++)
	{
		for (int t = 0; t < m_ColSource->GetCardPrototype(getCollectionList().at(i))->GetCopies(m_szName).size(); t++)
		{
			std::cout << m_ColSource->GetCardPrototype(getCollectionList().at(i))->GetName() << std::endl;
		}

	}
}

Collection::Transaction* Collection::openTransaction()
{
	// Currently, all i can think of is add/remove item, change/unchange attr of card.
	if ((m_lstTransactions.size() > 0 && !m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen) ||
		!(m_lstTransactions.size() > 0))
	{

		m_lstTransactions.push_back(Collection::Transaction(this));
	}
	return &m_lstTransactions.at(m_lstTransactions.size() - 1);
}

void Collection::finalizeTransaction(bool abRecord)
{
	if (m_lstTransactions.size() > 0 && m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
	{
		m_lstTransactions.at(m_lstTransactions.size() - 1).Finalize(abRecord);
	}
}

std::vector<std::string> Collection::GetCollectionList()
{
	// Now look for all cards with resident col == this
	std::vector<std::string> lstLines;
	std::vector<int>::iterator iter_ResiCards = getCollectionList().begin();
	for (; iter_ResiCards != getCollectionList().end(); ++iter_ResiCards)
	{
		int iCacheID = *iter_ResiCards;
		std::vector<CopyObject*> lstPossibleCopies = m_ColSource->GetCardPrototype(iCacheID)->GetCopies(m_szName);
		std::vector<CopyObject*>::iterator iter_PossibleCopies = lstPossibleCopies.begin();

		// Map all the copies that are identical to the same bucket
		std::vector<std::pair<CopyObject*, int>> lstBuckets;
		for (; iter_PossibleCopies != lstPossibleCopies.end(); ++iter_PossibleCopies)
		{
			CopyObject* oCurrentCard = *iter_PossibleCopies;
			bool bFound = false;

			// Check already processed copies
			std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
			for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
			{
				CopyObject* oCompareCard = iter_Bucket->first;
				if (bFound |= CollectionObject::IsSameIdentity(oCompareCard, oCurrentCard))
				{
					iter_Bucket->second = iter_Bucket->second + 1;
					break;
				}
			}

			// If not found, add it to the buckets.
			if (!bFound)
			{
				lstBuckets.push_back(std::make_pair(*iter_PossibleCopies, 1));
			}
			else
			{
				// It should have already been ticked up.
			}
		}

		std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
		for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
		{
			PseudoCopy oTemp = generatePseudoCopy(
				iter_Bucket->first->GetNonUniqueTraits(true),
				m_ColSource->GetCardPrototype(*iter_ResiCards)->GetName(),
				iter_Bucket->first->GetMetaTags(m_szName));
			oTemp.Count = iter_Bucket->second;
			std::string szLine = oTemp.ToString();
			lstLines.push_back(szLine);
		}
	}

	return lstLines;
}

std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
Collection::GetCollectionListWithMeta()
{
	// Sync

	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> lstRetVal;
	std::vector<int>::iterator iter_ResiCards = getCollectionList().begin();
	for (; iter_ResiCards != getCollectionList().end(); ++iter_ResiCards)
	{
		int iCacheID = *iter_ResiCards;
		std::vector<CopyObject*> lstPossibleCopies = m_ColSource->GetCardPrototype(iCacheID)->GetCopies(m_szName);
		std::vector<CopyObject*>::iterator iter_PossibleCopies = lstPossibleCopies.begin();

		// Map all the copies that are identical to the same bucket
		std::vector<std::pair<CopyObject*, int>> lstBuckets;
		std::vector<std::vector<std::pair<std::string, std::string>>> lstTagBucket;
		for (; iter_PossibleCopies != lstPossibleCopies.end(); ++iter_PossibleCopies)
		{
			CopyObject* oCurrentCard = *iter_PossibleCopies;
			bool bFound = false;

			// Check already processed copies
			std::vector<std::pair<CopyObject*, int>>::iterator iter_Bucket = lstBuckets.begin();
			for (; iter_Bucket != lstBuckets.end(); ++iter_Bucket)
			{
				CopyObject* oCompareCard = iter_Bucket->first;
				if (CollectionObject::IsSameIdentity(oCompareCard, oCurrentCard))
				{
					// The meta tags need also be the same
					if (bFound |= CopyObject::IsSameMetaTags(oCompareCard->GetMetaTags(m_szName), oCurrentCard->GetMetaTags(m_szName)))
					{
						iter_Bucket->second = iter_Bucket->second + 1;
						break;
					}

				}
			}

			// If not found, add it to the buckets.
			if (!bFound)
			{
				lstBuckets.push_back(std::make_pair(*iter_PossibleCopies, 1));
				lstTagBucket.push_back(oCurrentCard->GetMetaTags(m_szName));
			}
			else
			{
				// It should have already been ticked up.
			}
		}

		for (int i = 0; i < lstBuckets.size(); i++)
		{
			PseudoCopy oTemp = generatePseudoCopy(
				lstBuckets[i].first->GetNonUniqueTraits(true),
				m_ColSource->GetCardPrototype(*iter_ResiCards)->GetName(),
				lstBuckets[i].first->GetMetaTags(m_szName));
			oTemp.Count = lstBuckets[i].second;
			std::string szLine = oTemp.ToString();
			lstRetVal.push_back(std::make_pair(szLine, lstTagBucket[i]));
		}
	}

	return lstRetVal;
}



std::vector<std::pair<std::string, std::string>> Collection::ParseAttrs(std::string aszAttrs)
{
	std::vector<std::pair<std::string, std::string>> lstKeyVals;
	std::vector<std::string> lstDetails = StringHelper::Str_Split(aszAttrs, " ");
	for (std::vector<std::string>::iterator iter_attrs = lstDetails.begin(); iter_attrs != lstDetails.end(); ++iter_attrs)
	{
		std::vector<std::string> lstPairs = StringHelper::Str_Split(*iter_attrs, "=");
		if (lstPairs.size() > 1)
		{
			std::vector<std::string> lstVal = StringHelper::Str_Split(lstPairs[1], "\"");
			if (lstVal.size() == 3)
			{
				std::string szVal = lstVal[1];
				lstKeyVals.push_back(std::make_pair(lstPairs[0], szVal));
			}

		}

	}
	return lstKeyVals;
}

bool Collection::ParseCardLine(std::string aszLine, int& riCount, std::string& rszName, std::string& rszDetails)
{
	int i = 0;
	std::string szNum = "";
	while (i < aszLine.size() && aszLine.at(i) < '9' && aszLine.at(i) > '0')
	{
		szNum = szNum + aszLine.at(i);
		i++;
	}

	if (i >= aszLine.size())
	{
		return false;
	}

	if (szNum == "")
	{
		szNum = "1";
	}

	try
	{
		int iNum = std::stoi(szNum);
		riCount = iNum;
	}
	catch (...)
	{
		return false;
	}

	if (aszLine.at(i) == 'x')
	{
		i++;
	}

	if (i >= aszLine.size())
	{
		return false;
	}

	std::string szName = "";
	int iter_size = aszLine.size();
	while (i < iter_size &&
		((aszLine.at(i) >= 'a' && aszLine.at(i) <= 'z') ||
		 (aszLine.at(i) >= 'A' && aszLine.at(i) <= 'Z') ||
		 (aszLine.at(i) == ',' || aszLine.at(i) == ' ' || aszLine.at(i) == '-')))
	{
		szName = szName + aszLine.at(i);
		i++;
	}

	szName.erase(0, szName.find_first_not_of(' '));
	szName.erase(szName.find_last_not_of(' ') + 1);

	// Output the name
	rszName = szName;

	while (i < iter_size && (aszLine.at(i) == ' ' || aszLine.at(i) != '{'))
	{
		i++;
	}

	bool hasDets = false;
	if (i < iter_size)
	{
		if (aszLine.at(i) == '{')
		{
			hasDets = true;
		}
	}


	std::string szDetails = "";
	if (i < iter_size && hasDets)
	{

		while (i < iter_size)
		{
			szDetails += aszLine.at(i);
			i++;
		}
	}

	// Output the details
	rszDetails = szDetails;
	return true;
}

void Collection::RollbackTransaction()
{
	if (!m_lstTransactions.at(m_lstTransactions.size() - 1).IsOpen)
	{
		m_lstTransactions.at(m_lstTransactions.size() - 1).Rollback();
		m_lstTransactions.pop_back();
	}
}
