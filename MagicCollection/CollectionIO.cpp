#include "CollectionIO.h"



CollectionIO::CollectionIO()
{
}


CollectionIO::~CollectionIO()
{
}

std::vector<std::string> CollectionIO::LoadLines(std::string aszFileName)
{
	// Load the collection
	std::ifstream in(aszFileName);
	std::stringstream buffer;
	buffer << in.rdbuf();
	in.close();
	std::string contents(buffer.str());

	return StringHelper::SplitIntoLines(contents);
}

std::vector<std::string> CollectionIO::GetPreprocessLines(std::vector<std::string> alstAllLines, std::vector<std::string>& rlstPreprocessingLines)
{
	std::vector<std::string> lstRetVal;
	std::vector<std::string> lstPreprocessLines;
	std::vector<std::string>::iterator iter_Lines = alstAllLines.begin();
	for (; iter_Lines != alstAllLines.end(); ++iter_Lines)
	{
		if (iter_Lines->size() > 2 && iter_Lines->substr(0, 2) == Config::CollectionDefinitionKey)
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
