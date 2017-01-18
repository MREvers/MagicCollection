#include "Collection.h"


Collection::Collection(std::string aszName, CollectionSource* aoSource) : IName(aszName)
{
   m_ColSource = aoSource;
}


Collection::~Collection()
{
}

// Looks up from the source collection then adds it.
// Should either look up from a "source" or another collection.
// AddItem(AnotherCollection, name) -- from another collection.
// AddItem(name) -- from source
// This will also be fairly slow. In the future this should strictly be used for cards from source.
void Collection::AddItem(std::string aszNewItem)
{


}

void Collection::AddItem(std::string aszNewItem, ICollection* aoCol)
{
 
}
