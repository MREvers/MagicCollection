#pragma once

class CollectionItem;

class CollectionItemTest
{
public:
   CollectionItemTest();
   ~CollectionItemTest();

   bool AddCopy_Test();
   bool RemoveCopy_EntireChain_Test();
   bool RemoveCopy_PartialChain_Test();

private:
   CollectionItem getTestCollectionItem();
};

