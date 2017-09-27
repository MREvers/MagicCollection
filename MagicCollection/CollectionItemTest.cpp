#include "CollectionItemTest.h"

#include "CollectionItem.h"

using namespace std;

CollectionItemTest::CollectionItemTest()
{
}

CollectionItemTest::~CollectionItemTest()
{
}

bool 
CollectionItemTest::AddCopy_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   int iCopies = testItem.FindCopies(copyParent, All).size();
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   bResult &= createdCopy->GetIdentifyingAttribute("Collar")=="blue";
   bResult &= createdCopy->GetIdentifyingAttribute("Leash")=="chain";

   string szUID = createdCopy->GetUID();
   bResult &= testItem.FindCopy(szUID).Good();

   int iNewCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= iNewCopies == iCopies+1;

   return bResult;
}

// Also tests UID uniqueness
bool 
CollectionItemTest::RemoveCopy_EntireChain_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);
   auto createdCopy2 = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   string szUID = createdCopy->GetUID();
   string szUIDTwo = createdCopy2->GetUID();

   bResult &= szUID != szUIDTwo;
   bResult &= testItem.FindCopy(szUID).Good();
   bResult &= testItem.FindCopy(szUIDTwo).Good();

   int iCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= testItem.RemoveCopy(Location("IDTest-1"), szUID );

   int iNewCopies = testItem.FindCopies(copyParent, All).size();

   bResult &= iCopies == iNewCopies+1;

   return bResult;
}

bool 
CollectionItemTest::RemoveCopy_PartialChain_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   string szUID = createdCopy->GetUID();

   bResult &= testItem.FindCopy(szUID).Good();

   int iCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= testItem.RemoveCopy(Location("IDTest-6"), szUID );
   int iNewCopies = testItem.FindCopies(Location("IDTest-1"), All).size();

   bResult &= iCopies == iNewCopies;
   bResult &= createdCopy->GetParent() == "IDTest-2";

   return bResult;
}

CollectionItem 
CollectionItemTest::getTestCollectionItem()
{
   // For this, I don't want to be dependent on outside files
   // Create a custom item.
   std::vector<Tag> vecCommonTags;
   vecCommonTags.push_back(make_pair("set", "BFZ"));
   vecCommonTags.push_back(make_pair("height", "5"));
   vecCommonTags.push_back(make_pair("degree", "Good"));

   std::vector<TraitItem> vecRestrictedTraits;
   TraitItem traitOne( "Collar", { "blue", "red" } );
   TraitItem traitTwo( "Leash", { "chain", "rope" } );
   vecRestrictedTraits.push_back(traitOne);
   vecRestrictedTraits.push_back(traitTwo);

   return CollectionItem( "TestItem", vecCommonTags, vecRestrictedTraits);
}