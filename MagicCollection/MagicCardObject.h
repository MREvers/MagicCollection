#pragma once
#pragma message ("Starting MagicCardObject.h")

#include "CollectionObject.h"


class MagicCardObject :
   public CollectionObject
{
public:
   MagicCardObject(std::string aszName);
   ~MagicCardObject();

   bool MapAttributes(std::string aszKey, std::string aszValue) override;

private:

   std::string m_szPower;
   std::string m_szToughness;
   std::string m_szText;
   std::string m_szType;
   std::string m_szSets;
   std::string m_szLoyalty;

   int m_AllAttributes;
   
};

#pragma message ("Finish MagicCardObject.h")