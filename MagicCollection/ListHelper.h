#pragma once

#include <string>
#include <vector>
#include <map>

class ListHelper
{
public:
	ListHelper();
	~ListHelper();

	static 
		std::vector<std::pair<std::string, std::string>>
		ConvertMapToList(std::map<std::string, std::string>  aMap);
	static 
		bool CompareKeyValPairList(std::vector<std::pair<std::string, std::string>> alstFirst,
		std::vector<std::pair<std::string, std::string>> alstSecond);
};

