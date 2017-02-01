#pragma once
#pragma message ("Starting SourceObject.h")

#include <map>
#include <vector>

#include "CollectionObject.h"

class CollectionObject;

class SourceObject
{
public:
   SourceObject(std::string aszName);
   ~SourceObject();

   // Used in building this object
   std::map<std::string, std::string> Attributes;
   std::map<std::string, std::vector<std::string>> NonUniqueAttributes;
   //std::vector<std::pair<std::string, std::string>> Attribs;
   bool AddAttribute(std::string key, std::string value);
   bool AddNonUniqueAttribute(std::string, std::string value);

   // Used in loading from source DB.
   std::string GetName();
   int GetCacheIndex();
   void Cache(int aiCacheIndex);

   static std::vector<std::string> Str_Split(std::string aszSplit, std::string aszDelim);

private:
   int m_iCachedIndex;
   int m_iHaves;
   std::string m_szName;

   
};

#pragma message ("Finish SourceObject.h")