#pragma once
#include "ICollectionObject.h"
#include <map>

class CollectionObject : public ICollectionObject
{
public:
   CollectionObject(std::string aszName);
   ~CollectionObject();

   std::string GetName() override;

   //static std::string VALID_ATTS[5] = {"name", "manaCost", "power", "toughness", "type", "sets"};

private:
   std::string m_szName;


};

