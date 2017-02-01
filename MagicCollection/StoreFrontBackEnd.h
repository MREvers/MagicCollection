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

	void LoadCollection(std::string aszCollectionToLoad);
	std::vector<std::string> GetCollectionList(std::string aszCollection);

private:
	//ServerIFace* m_ServerIFace;
	CollectionFactory* m_ColFactory; // This will be the main interaction.
	CollectionSource* m_ColSource; // This can be a vector in the future
};

