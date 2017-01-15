#pragma once
#include "ICollectionObject.h"
#include "Collection.h"
#include <map>

// Number of copies
// Later: Set specief
class CollectionAnalytics
{
public:

   std::vector<int> ListCopies;
   std::map<int, Collection*> MapParentCollection;

   // Map copyID to card uniques such as set.
};

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class CollectionObject : public ICollectionObject
{
public:
   CollectionObject(std::string aszName);
   ~CollectionObject();

   std::string GetName() override;

   // Gets called by the collection Source. Implement in base class to get needed attributes.
   virtual bool MapAttributes(std::string aszName, std::string aszValue);
   
   // Collection Side Interface
   ICollectionObject* IncludeInCollection(Collection* aoCol, std::vector<ICollectionObject*>& aoColOs);
   std::vector<int> GetCopies(Collection* aoCol);
   int GetNumberOfCopies(Collection* aoCol);
   Collection* GetResidentCollection(Collection* aoCol, int aiCopyID);

private:
   int m_iAllCopies; // Used in assigning IDs.
   std::string m_szName;

   std::map<Collection*, CollectionAnalytics> m_mapAnalytics;

   void addSorted(ICollectionObject* aoColO, std::vector<ICollectionObject*> alstColOs);

};

