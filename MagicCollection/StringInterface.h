#pragma once
#include<string>
#include<vector>
#include "Config.h"

class __declspec(dllexport) StringInterface
{
public:
   StringInterface();
   ~StringInterface();

   bool ParseCardLine( const std::string& aszLine,
                       unsigned int& riCount,
                       std::string& rszName,
                       std::string& rszDetails,
                       std::string& rszMeta ) const;

   bool ParseCardLine( const std::string& aszLine,
                       unsigned int& riCount,
                       std::string& rszName,
                       std::vector<Tag>& rszDetails,
                       std::vector<Tag>& rszMeta ) const;

   bool ParseTagString( const std::string& aszDetails,
                        std::vector<Tag>& rlstTags ) const;
};

