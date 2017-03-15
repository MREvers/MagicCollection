#pragma once
#include <string>
#include <vector>

class StringHelper
{
public:
   StringHelper();
   ~StringHelper();

   static std::string Str_Trim(const std::string& srz, char removeChar);
   static std::vector<std::string> Str_Split(std::string& aszSplit, std::string aszDelim);
};

