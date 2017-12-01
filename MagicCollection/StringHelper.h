#pragma once
#include <string>
#include <vector>

class StringHelper
{
public:
   StringHelper();
   ~StringHelper();

   static std::string Str_Trim(const std::string& srz, char removeChar);
   static std::string Str_Replace(const std::string& srz, char removeChar, char replaceChar);
   static std::string Str_Clean(const std::string& srz);
   static std::vector<std::string> Str_CmdLineParse(const std::string& srz);
   static std::vector<std::string> Str_Split(const std::string& aszSplit, const std::string& aszDelim);
   static std::vector<std::string> SplitIntoLines(const std::string& aszString);
   static int Find_First_Instance_In(const std::string& aszString, const std::string& aszFind);
};

