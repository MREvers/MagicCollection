#pragma once
#pragma message ("Starting SourceObject.h")
#include "IName.h"
#include "CollectionObject.h"

#include <map>


class CollectionObject;

class SourceObject : public IName
{
public:
   SourceObject(std::string aszName);
   ~SourceObject();

   // Used in building this object
   std::map<std::string, std::string> Attributes;
   bool AddAttribute(std::string key, std::string value);
   bool HasAttribute(std::string aszname);

   // Used in loading from source DB.
   bool GetCollectionObject(CollectionObject*& roColO);
   void Cache(CollectionObject* roColO);


private:
   bool m_bHasCO;
   CollectionObject* m_oCollectionObject;
};

#pragma message ("Finish SourceObject.h")