#include "MagicCardObject.h"


MagicCardObject::MagicCardObject(std::string aszName) : CollectionObject(aszName)
{
   m_AllAttributes = 0;
}


MagicCardObject::~MagicCardObject()
{
}

bool MagicCardObject::MapAttributes(std::string aszKey, std::string aszValue)
{
   if (aszKey == "power")
   {
      m_szPower = aszValue;
      m_AllAttributes &= 0x01;
   }
   else if (aszKey == "text")
   {
      m_szText = aszValue;
      m_AllAttributes &= 0x02;
   }
   else if (aszKey == "toughness")
   {
      m_szToughness = aszValue;
      m_AllAttributes &= 0x04;
   }
   else if (aszKey == "set")
   {
      m_szSets == aszValue;
      m_AllAttributes &= 0x08;
   }
   else if (aszKey == "loyalty")
   {
      m_szLoyalty == aszValue;
      m_AllAttributes &= 0x10;
   }
   else if (aszKey == "type")
   {
      m_szType == aszValue;
      m_AllAttributes &= 0x20;
   }

   return m_AllAttributes == 0x3F;

}