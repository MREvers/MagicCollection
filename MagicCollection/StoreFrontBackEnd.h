#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>

#include "Collection.h"
#include "JSONImporter.h"
#include "CollectionSource.h"
#include "CollectionFactory.h"
//#include "ServerIFace.h"

class __declspec(dllexport) CStoreFrontBackEnd
{
public:
	CStoreFrontBackEnd();
	~CStoreFrontBackEnd();

	// Collection interface
	void AddItem(std::string aszCollection, std::string aszItem,
		std::vector<std::pair<std::string, std::string>> alstMeta);
	void RemoveItem(std::string aszCollection, std::string aszItem, std::vector<std::pair<std::string, std::string>> alstMeta);
	void SaveCollection(std::string aszCollectionToSave);

	std::vector<std::pair<std::string, std::string>> GetCardAttributes(std::string aszCardNameLong);
	// Returns the list of restions if restrictions exist. * if none exist.
	std::vector<std::string> GetCardAttributeRestriction(std::string aszCardNameLong, std::string aszKey);

	// Loads specified collection into memory by file name
	std::string LoadCollection(std::string aszCollectionToLoad);

	// Returns a string list of all the cards in long name form
	std::vector<std::string> GetCollectionList(std::string aszCollection);
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
		GetCollectionListWithMeta(std::string aszCollection);

	// Returns list of loaded collections
	std::vector<std::string> GetLoadedCollections();

	// Returns list of all cards containing aszText
	std::vector<std::string> GetAllCardsStartingWith(std::string aszText);

	std::vector < std::vector<std::pair<std::string, std::string>>> GetMetaTags(std::string aszCollection, std::string aszLongName);
	void AddMetaTag(
		std::string aszCollection,
		std::string aszLongName,
		std::string aszKey,
		std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta);
   void SetMetaTags(
      std::string aszCollection,
      std::string aszLongName,
      std::vector<std::pair<std::string, std::string>> alstNewTags,
      std::vector<std::pair<std::string, std::string>> alstMatchMeta);
	void AddMetaTag(
		std::string aszCollection,
		std::string aszLongName,
		std::string aszKey,
		std::string aszSubKey,
		std::string aszValue,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta);
	void RemoveMetaTag(
		std::string aszCollection,
		std::string aszLongName,
		std::string aszKey,
		std::vector<std::pair<std::string, std::string>> alstMatchMeta);
   void SetFeatures(
      std::string aszCollection,
      std::string aszLongName,
      std::vector<std::pair<std::string, std::string>> alstNewMetaTags,
      std::vector<std::pair<std::string, std::string>> alstNewAttrs,
      std::vector<std::pair<std::string, std::string>> alstMatchMeta);

	// For parsing long names
	std::vector<std::pair<std::string, std::string>> ParseAttrs(std::string aszDetails);
	bool ParseCardString(std::string aszCardString, int& riCount, std::string& rszName, std::string& rszDetails);

	bool IsSameCard(std::string aszLongOne, std::string aszLongTwo);
	bool IsSameMetaTags(std::vector<std::pair<std::string, std::string>> aLstOne, std::vector<std::pair<std::string, std::string>> aLstTwo);

	void ImportCollection();
private:
	//ServerIFace* m_ServerIFace;
	CollectionFactory* m_ColFactory; // This will be the main interaction.
	CollectionSource* m_ColSource; // This can be a vector in the future
};

