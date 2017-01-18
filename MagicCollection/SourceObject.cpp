#include "SourceObject.h"


SourceObject::SourceObject(std::string aszName) : IName(aszName)
{
   AddAttribute("name", aszName);
}

SourceObject::~SourceObject()
{
}

// This may be used in the future.. but not now.
bool SourceObject::HasAttribute(std::string aszName)
{
   return false;
   /* assume that the database is not redundant
      bool bRetval = false;
      for (auto keyVal : Attributes)
      {
         bRetval || keyVal.first == aszName;
         if (bRetval)
         {
            break;
         }
      }

      return bRetval;
      */
}

bool SourceObject::AddAttribute(std::string key, std::string value)
{
   if (key == "set" ||
      key == "power" ||
      key == "toughness" ||
      key == "manaCost" ||
      key == "name" ||
      key == "text" ||
      key == "loyalty" ||
      key == "colors")
   {
      Attributes.insert(std::make_pair(key, value));
   }

   return false;
}

bool SourceObject::GetCollectionObject(CollectionObject*& roColO)
{
   if (m_bHasCO)
   {
      roColO = m_oCollectionObject;
      return true;
   }
   else
   {
      return false;
   }
}

void SourceObject::Cache(CollectionObject* aoColO)
{
   m_oCollectionObject = aoColO;
   m_bHasCO = true;
}

