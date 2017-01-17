#pragma once
#pragma message ("Starting ICollection.h")
#include <string>
#include "ICollectionObject.h"

class ICollection
{
public:
   // Sort
   
   virtual int FindInCollection(std::string aszItem) = 0;
   virtual bool GetFromCollection(std::string aszItem, ICollectionObject& rptColO) = 0;
   
   // Get from collection source.
   virtual void AddItem(std::string aszNewItem) = 0;
   // Get from another collection.
   virtual void AddItem(std::string aszNewItem, ICollection* aoCol) = 0;
   // Get a particular copy from another collection.
   //virtual void AddItem(std::string aszNewItem, ICollection* aoCol, int aiCopyID) = 0;

   virtual std::vector<ICollectionObject*>& GetList() = 0;
   
};

#pragma message ("Finish ICollection.h")