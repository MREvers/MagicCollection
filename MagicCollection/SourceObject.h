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

   std::string GetName();

   // Used in building this object
   std::map<std::string, std::string> Attributes;
   bool AddAttribute(std::string key, std::string value);
   bool HasAttribute(std::string aszname);

   // Used in loading from source DB.
   bool GetCollectionObject(CollectionObject*& roColO);
   void Cache(CollectionObject* roColO);


private:
   std::string m_szName;
   bool m_bHasCO;
   CollectionObject* m_oCollectionObject;
};

#pragma message ("Finish SourceObject.h")