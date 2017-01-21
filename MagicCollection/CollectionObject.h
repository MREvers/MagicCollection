#pragma once
#pragma message ("Starting CollectionObject.h")
#include "ICollectionObject.h"
#include "ICollection.h"

#include <string>
#include <vector>
#include <map>

class ICollection;

class CopyObject
{
   public:
      std::string ParentCollection;
      std::vector<std::string> ResidentCollections;
      std::map<std::string, std::string> UniqueTraits;
      // Other analytics go here.
};

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class CollectionObject : public ICollectionObject
{
public:
   CollectionObject(std::string aszName);
   ~CollectionObject();

   // Gets called by the collection Source. Implement in base class to get needed attributes.
   virtual bool MapAttributes(std::string aszName, std::string aszValue);

   // Collection Interface
   std::string GetName();
   CopyObject* AddCopy(std::string aszCollectionName);
   // The col children will have to match exactly... because copies are not identical.
   void RemoveCopy(std::string aszCollectionName);
   std::vector<CopyObject*> GetLocalCopies(std::string aszCollectionName);
   std::vector<CopyObject*> GetCopies(std::string aszCollectionName);

private:
   int m_iAllCopies; // Used in assigning IDs.

   std::vector<CopyObject> m_lstCopies;

   std::string m_szName;

};

#pragma message ("Finish CollectionObject.h")