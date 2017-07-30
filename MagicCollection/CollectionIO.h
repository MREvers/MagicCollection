#pragma once
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <sstream>

#include "Config.h"
#include "StringHelper.h"
#include "CopyItem.h"
#include "CollectionItem.h"
#include "Collection.h"
#include "CollectionFactory.h"
#include "CollectionSource.h"
#include "ListHelper.h"

class CollectionFactory;
class Collection;

class CollectionIO
{
public:
   CollectionIO();
   ~CollectionIO();

   std::vector<std::string> GetFileLines(std::string aszFileName);
   bool GetPreprocessLines(std::vector<std::string> alstAllLines,
      std::vector<std::string>& rlstIOLines,
      std::vector<std::string>& rlstPreprocessingLines);

   bool CaptureUnlistedItems(Address aAddrColID,
      CollectionSource* aptCollectionSource,
      std::map<int, std::list<CopyItem*>>& rlstAdditionalItems,
      std::map<int, std::list<CopyItem*>>& rlstAlreadyCapturedItems);

   bool ConsolodateLocalItems(Address aAddrColID,
      CollectionSource* aptCollectionSource,
      std::map<int, std::list<CopyItem*>>& rlstPotentialDuplicates,
      std::map<int, std::list<CopyItem*>>& rlstNonDuplicates);

   bool RejoinAsyncedLocalItems(Address aAddrColID,
      CollectionSource* aptCollectionSource,
      unsigned long aulNewItemTS,
      std::map<int, std::list<CopyItem*>>& rlstPotentialDuplicates,
      std::map<int, std::list<CopyItem*>>& rlstNonDuplicates);

   bool ConsolodateBorrowedItems(Address aAddrColID,
      CollectionSource* aptCollectionSource,
      CollectionFactory* aptCollFactory);

   bool ReleaseUnfoundReferences(Address aAddrColID,
      CollectionSource* aptCollectionSource);

   bool CollectionFileExists(std::string aszFileName);
   std::string GetCollectionFile(std::string aszCollectionName);
   std::string GetMetaFile(std::string aszCollectionName);
   std::string GetHistoryFile(std::string aszCollectionName);
};

