#include "SourceObject.h"


SourceObject::SourceObject(std::string aszName)
{
   AddAttribute("name", aszName);
   m_iCachedIndex = -1;
   m_szName = aszName;
}

SourceObject::~SourceObject()
{
}

bool SourceObject::AddAttribute(std::string key, std::string value)
{
   Attributes.insert(std::make_pair(key, value));
   return false;
}

std::string SourceObject::GetName()
{
   return m_szName;
}

int SourceObject::GetCacheIndex()
{
   return m_iCachedIndex;
}

void SourceObject::Cache(int aiCacheIndex)
{
   m_iCachedIndex = aiCacheIndex;
}

