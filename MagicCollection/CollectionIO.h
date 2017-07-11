#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Config.h"
#include "StringHelper.h"

class CollectionIO
{
public:
	CollectionIO();
	~CollectionIO();

	std::vector<std::string> GetFileLines(std::string aszFileName);
	std::vector<std::string> GetPreprocessLines(std::vector<std::string> alstAllLines, std::vector<std::string>& rlstPreprocessingLines);

	bool CollectionFileExists(std::string aszFileName);
	std::string GetCollectionFile(std::string aszCollectionName);
	std::string GetMetaFile(std::string aszCollectionName);
	std::string GetHistoryFile(std::string aszCollectionName);
};

