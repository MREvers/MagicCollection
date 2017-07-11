#include "CollectionIO.h"



CollectionIO::CollectionIO()
{
}


CollectionIO::~CollectionIO()
{
}

std::vector<std::string> CollectionIO::GetFileLines(std::string aszFileName)
{
	// Load the collection
	std::ifstream in(aszFileName);
	std::stringstream buffer;
	buffer << in.rdbuf();
	in.close();
	std::string contents(buffer.str());

	return StringHelper::SplitIntoLines(contents);
}

// Returns non-preprocessing lines.
std::vector<std::string> CollectionIO::GetPreprocessLines(std::vector<std::string> alstAllLines, std::vector<std::string>& rlstPreprocessingLines)
{
	std::vector<std::string> lstRetVal;
	std::vector<std::string> lstPreprocessLines;
	std::vector<std::string>::iterator iter_Lines = alstAllLines.begin();
	std::string szDefKey(Config::CollectionDefinitionKey);
	for (; iter_Lines != alstAllLines.end(); ++iter_Lines)
	{
		if (iter_Lines->size() > 2 && iter_Lines->substr(0, szDefKey.size()) == szDefKey)
		{
			lstPreprocessLines.push_back(*iter_Lines);
		}
		else
		{
			lstRetVal.push_back(*iter_Lines);
		}
	}

	rlstPreprocessingLines = lstPreprocessLines;
	return lstRetVal;
}

bool CollectionIO::CollectionFileExists(std::string aszFileName)
{
	std::string szFullFileName = GetCollectionFile(aszFileName);
	std::ifstream f(szFullFileName.c_str());
	return f.good();
}

std::string CollectionIO::GetCollectionFile(std::string aszCollectionName)
{
	return Config::Instance()->GetCollectionsDirectory() + "\\" +
		StringHelper::Str_Replace(aszCollectionName, ' ', '_') + ".txt";
}

std::string CollectionIO::GetMetaFile(std::string aszCollectionName)
{
	return Config::Instance()->GetCollectionsDirectory() + "\\" +
		StringHelper::Str_Replace(aszCollectionName, ' ', '_') + "." + std::string(Config::MetaFileExtension) + ".txt";
}

std::string CollectionIO::GetHistoryFile(std::string aszCollectionName)
{
	return Config::Instance()->GetCollectionsDirectory() + "\\" +
		StringHelper::Str_Replace(aszCollectionName, ' ', '_') + "." + std::string(Config::HistoryFileExtension) + ".txt";
}