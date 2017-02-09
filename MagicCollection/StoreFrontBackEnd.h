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

	void AddItem(std::string aszCollection, std::string aszItem);
	void RemoveItem(std::string aszCollection, std::string aszItem);
	void SaveCollection(std::string aszCollectionToSave);
	std::string LoadCollection(std::string aszCollectionToLoad);
	std::vector<std::string> GetCollectionList(std::string aszCollection);
	std::vector<std::string> GetLoadedCollections();
	std::vector<std::pair<std::string, std::string>> ParseAttrs(std::string aszDetails);
	bool ParseCardString(std::string aszCardString, int& riCount, std::string& rszName, std::string& rszDetails);
	std::vector<std::string> GetAllCardsStartingWith(std::string aszText);

private:
	//ServerIFace* m_ServerIFace;
	CollectionFactory* m_ColFactory; // This will be the main interaction.
	CollectionSource* m_ColSource; // This can be a vector in the future
};

