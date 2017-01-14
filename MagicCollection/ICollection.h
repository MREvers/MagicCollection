#pragma once
#include "ICollectionObject.h"
#include <string>

class ICollection
{
public:
   // Sort
   virtual void AddItem(std::string aNewItem) = 0;

};

