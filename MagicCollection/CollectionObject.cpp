#include "CollectionObject.h"


CollectionObject::CollectionObject(std::string aszName)
{
   m_szName = aszName;
}


CollectionObject::~CollectionObject()
{
}

std::string CollectionObject::GetName()
{
   return m_szName;
}