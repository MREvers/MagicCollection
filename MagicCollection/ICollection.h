#pragma once
#pragma message ("Starting ICollection.h")
#include <string>

class ICollection
{
public:
   // Sort
   // Get from collection source.
   virtual void AddItem(std::string aszNewItem) = 0;
   /*
   // Get from another collection.
   virtual void AddItem(std::string aszNewItem, Collection* aoCol) = 0;
   // Get a particular copy from another collection.
   virtual void AddItem(std::string aszNewItem, Collection* aoCol, int aiCopyID) = 0;
   */
};

#pragma message ("Finish ICollection.h")