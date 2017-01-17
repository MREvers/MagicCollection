#pragma once
#pragma message ("Starting CollectionObject.h")
#include "ICollectionObject.h"
#include "ICollection.h"
//#include "Collection.h"
#include <vector>
#include <map>

class ICollection;

// Number of copies
// Later: Set specief
// ONE OF THESE PER COLLECTION
class CollectionAnalytics
{
public:

   std::vector<int> ListCopies;
   std::map<int, ICollection*> Map_ID_Parent_Collection;
   std::map<int, std::vector<ICollection*>> Map_ID_Derived_Collections;

   // Map copyID to card uniques such as set.
};

// Since this is a flyweight object, the interface should act as though it is 'in' the collection,
//  but functionaly it should be a flyweight.
class CollectionObject : public ICollectionObject
{
public:
   CollectionObject(std::string aszName);
   ~CollectionObject();

   std::string GetName();

   // Gets called by the collection Source. Implement in base class to get needed attributes.
   virtual bool MapAttributes(std::string aszName, std::string aszValue);
   
   // Collection Side Interface
   ICollectionObject* IncludeInCollection(ICollection* aoCol, std::vector<ICollectionObject*>& aoColOs);
   std::vector<int> GetCopies(ICollection* aoCol);
   int GetNumberOfCopies(ICollection* aoCol);
   std::vector<int> GetLocalCopies(ICollection* aoCol);
   int GetNumOfLocalCopies(ICollection* aoCol);
   ICollection* GetResidentCollection(ICollection* aoCol, int aiCopyID);
   void IncrementItem(ICollection* aoCol);
   void RemoveItem(ICollection* aoCol, int aiCardID);
   bool AddItem(ICollection* aoCallingCol, ICollection* aoSourcingCol);
   bool AddItem(ICollection* aoCallingCol, ICollection* aoSourcingCol, int aiCardID);


private:
   int m_iAllCopies; // Used in assigning IDs.
   std::string m_szName;

   std::map<ICollection*, CollectionAnalytics> m_mapAnalytics;

   void addSorted(ICollectionObject* aoColO, std::vector<ICollectionObject*>& alstColOs);

};

#pragma message ("Finish CollectionObject.h")