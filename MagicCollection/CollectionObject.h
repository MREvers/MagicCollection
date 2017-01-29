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

      // Non-unique in the sense that the trait itself can have more than one value.
      std::map<std::string, std::string> NonUniqueTraits;
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
   // Gets all resi copies.
   std::vector<CopyObject*> GetCopies(std::string aszCollectionName);
   // Gets a copy with matching attrs
   bool GetCopy(std::string aszCollectionName, std::vector<std::pair<std::string,std::string>> alstAttrs, CopyObject& roCO, bool abExact = true);
   // Used in building a printable structure
   static CopyObject GenerateCopy(std::string aszCollectionName);
   // Used in building a printable structure
   static CopyObject GenerateCopy(std::string aszCollectionName, std::vector<std::pair<std::string, std::string>> alstAttrs);
   static void ConstructCopy(CopyObject& roCO, std::vector<std::pair<std::string, std::string>> alstAttrs);
   static bool IsSameIdentity(CopyObject* aoCOne, CopyObject* aoCTwo);

private:
   int m_iAllCopies; // Used in assigning IDs.

   std::vector<CopyObject> m_lstCopies;

   std::string m_szName;

};

#pragma message ("Finish CollectionObject.h")