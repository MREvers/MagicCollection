#pragma once
#pragma message ("Starting CollectionObject.h")
#include "ICollectionObject.h"
#include "ICollection.h"
#include "IName.h"
//#include "Collection.h"
#include <string>
#include <vector>
#include <map>

class ICollection;

class CopyObject
{
   public:
      std::string ParentCollection;
      std::vector<std::string> ResidentCollections;
};

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class CollectionObject : public ICollectionObject, IName
{
public:
   CollectionObject(std::string aszName);
   ~CollectionObject();

   // Gets called by the collection Source. Implement in base class to get needed attributes.
   virtual bool MapAttributes(std::string aszName, std::string aszValue);


private:
   int m_iAllCopies; // Used in assigning IDs.

   std::map<int, int> m_mapCardCopies;

};

#pragma message ("Finish CollectionObject.h")