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

   std::map<std::string, std::string> Attributes;

   bool AddAttribute(std::string key, std::string value);

   bool HasAttribute(std::string aszname);

   std::string GetName();

   bool GetCollectionObject(CollectionObject*& roColO);

   void Cache(CollectionObject* roColO);

private:
   std::string m_szName;
   bool m_bHasCO;
   CollectionObject* m_oCollectionObject;
};

#pragma message ("Finish SourceObject.h")