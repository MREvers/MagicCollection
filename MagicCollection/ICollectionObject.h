#pragma once
#pragma message ("Starting ICollectionObject.h")

#include <string>
#include <vector>

class ICollection;

class ICollectionObject
{
public:

   // This should return true when all objects have been mapped.
   // Collection Source Side Interface
   virtual bool MapAttributes(std::string aszKey, std::string aszValue) = 0;
  
   //virtual int Get

};
#pragma message ("Finish ICollectionObject.h")
