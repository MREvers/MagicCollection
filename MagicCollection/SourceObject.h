#pragma once
#include <map>
#include "ICollectionObject.h"

class SourceObject : ICollectionObject
{
public:
   SourceObject(std::string aszName);
   ~SourceObject();

   std::map<std::string, std::string> Attributes;

   bool AddAttribute(std::string key, std::string value);

   bool HasAttribute(std::string aszname);

   std::string GetName() override;

private:
   std::string m_szName;
};

