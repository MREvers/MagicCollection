#include "CollectionObject.h"


CollectionObject::CollectionObject(std::string aszName) : IName(aszName)
{
   m_iAllCopies = 0;
}


CollectionObject::~CollectionObject()
{
}

bool CollectionObject::MapAttributes(std::string aszName, std::string aszValue)
{
   return true;
}
