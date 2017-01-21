#pragma once
#pragma message ("Starting SourceObject.h")
#include <map>

#include "CollectionObject.h"

class CollectionObject;

class SourceObject
{
public:
   SourceObject(std::string aszName);
   ~SourceObject();

   // Used in building this object
   std::map<std::string, std::string> Attributes;
   //std::vector<std::pair<std::string, std::string>> Attribs;
   bool AddAttribute(std::string key, std::string value);

   // Used in loading from source DB.
   std::string GetName();
   int GetCacheIndex();
   void Cache(int aiCacheIndex);

private:
   int m_iCachedIndex;
   int m_iHaves;
   std::string m_szName;
};

#pragma message ("Finish SourceObject.h")