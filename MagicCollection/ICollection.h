#pragma once
#pragma message ("Starting ICollection.h")
#include <string>
#include "ICollectionObject.h"

class ICollection
{
public:
   // Get from collection source.
   virtual void AddItem(std::string aszNewItem) = 0;
   
};

#pragma message ("Finish ICollection.h")