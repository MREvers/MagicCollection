#pragma once
#pragma message ("Starting ICollectionObject.h")

#include <string>
#include <vector>

class ICollection;

class ICollectionObject
{
public:
   virtual std::string GetName() = 0;
   // This should return true when all objects have been mapped.
   // Collection Source Side Interface
   virtual bool MapAttributes(std::string aszKey, std::string aszValue) = 0;
  
   // Collection Side Interface
   virtual ICollectionObject* IncludeInCollection(ICollection* aoCol, std::vector<ICollectionObject*>& alstColOs) = 0; // Generates a new CopyID
   virtual int GetNumberOfCopies(ICollection* aoCol) = 0;
   virtual std::vector<int> GetCopies(ICollection* aoCol) = 0;
   virtual ICollection* GetResidentCollection(ICollection* aoCol, int aiCopyID) = 0;
   //virtual int Get

};
#pragma message ("Finish ICollectionObject.h")
