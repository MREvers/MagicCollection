#pragma once
#include <string>
#include <vector>
class ICollectionObject
{
public:
   virtual std::string GetName() = 0;
   // This should return true when all objects have been mapped.
   // Collection Source Side Interface
   virtual bool MapAttributes(std::string aszKey, std::string aszValue) = 0;
  
   // Collection Side Interface
   virtual ICollectionObject* IncludeInCollection(Collection* aoCol, std::vector<ICollectionObject*>& alstColOs) = 0; // Generates a new CopyID
   virtual int GetNumberOfCopies(Collection* aoCol) = 0;
   virtual std::vector<int> GetCopies(Collection* aoCol) = 0;
   virtual Collection* GetResidentCollection(Collection* aoCol, int aiCopyID) = 0;
   //virtual int Get

};

