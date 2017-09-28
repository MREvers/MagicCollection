#pragma once
#include<string> 
#include<memory>

class Config;
class Collection;
class CollectionSource;
class CollectionTest
{
public:
   CollectionTest();
   ~CollectionTest();

   bool AddItem_Test();
   bool RemoveItem_Test();
   bool ChangeItem_Test();
   bool AddItemFrom_Test();
   bool ReplaceItem_Test();
   bool LoadCollection_Test();
   bool SaveCollection_Test();

private:
   void writeTestColSourceFile();
   void deleteTestColSourceFile();
   
   std::shared_ptr<Collection> getTestCollection();
   const std::string* cardName(int iName);
   const std::string* getAttr(int iAttr);

   CollectionSource* m_ptColSource;
};

