#pragma once
#pragma message ("Starting ICollection.h")
#include <string>
#include "ICollectionObject.h"

class ICollection
{
public:
   // Get from collection source.
   virtual void AddItem(std::string aszNewItem, bool bFinal, std::vector<std::pair<std::string, std::string>> alstAttrs) = 0;
   virtual void RemoveItem(std::string aszRemoveItem, bool bFinal, std::vector<std::pair<std::string, std::string>> alstAttrs,
	 std::vector<std::pair<std::string, std::string>> alstMeta) = 0;
   
};

#pragma message ("Finish ICollection.h")