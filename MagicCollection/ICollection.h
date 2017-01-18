#pragma once
#pragma message ("Starting ICollection.h")
#include <string>
#include "ICollectionObject.h"

class ICollection
{
public:
   // Get from collection source.
   virtual void AddItem(std::string aszNewItem) = 0;
   // Get from another collection.
   virtual void AddItem(std::string aszNewItem, ICollection* aoCol) = 0;
   
};

#pragma message ("Finish ICollection.h")