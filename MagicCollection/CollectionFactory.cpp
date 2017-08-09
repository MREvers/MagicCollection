#include "CollectionFactory.h"


CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
   m_ColSource = aoColSource;
   m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

std::string CollectionFactory::LoadCollectionFromFile(std::string aszFileName)
{
   std::string szRetVal = Config::NotFoundString;
   std::vector<std::string> lstSplitFile;
   std::vector<std::string> lstSplitExt;
   std::string szFile;

   // Get the file name.
   lstSplitFile = StringHelper::Str_Split(aszFileName, "\\");
   szFile = lstSplitFile[lstSplitFile.size() - 1];
   lstSplitExt = StringHelper::Str_Split(szFile, ".");
   szFile = lstSplitExt[0];

   Collection* oCol = new Collection(Config::NotFoundString, m_ColSource, szFile);
   oCol->LoadCollection(aszFileName, this);
   Address szFoundID = oCol->GetIdentifier();

   if (oCol->IsLoaded && !CollectionExists(szFoundID))
   {
      m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
      szRetVal = szFoundID.GetFullAddress();
   }
   else
   {
      delete oCol;
   }

   return szRetVal;
}

std::string 
CollectionFactory::CreateNewCollection(std::string aszColName, std::string aszParentID)
{
   Collection* oCol;
   // The parent is required to be loaded to have it as a parent
   if (CollectionExists(aszParentID))
   {
      oCol = new Collection(aszColName, m_ColSource, aszColName, getNextChildName(aszParentID));
      GetCollection(aszParentID)->ChildAdded();
   }
   else
   {
      oCol = new Collection(aszColName, m_ColSource, aszColName);
   }

   m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
   return oCol->GetIdentifier().GetFullAddress();
}

std::vector<std::string> CollectionFactory::GetLoadedCollections()
{
   std::vector<std::string> lstRetval;
   auto iter_Colo = m_lstCollections.begin();
   for (; iter_Colo != m_lstCollections.end(); ++iter_Colo)
   {
      lstRetval.push_back((*iter_Colo)->GetIdentifier().GetFullAddress());
   }
   return lstRetval;
}

bool CollectionFactory::CollectionExists(std::string aszCollectionName)
{
   return CollectionExists(Address(aszCollectionName));
}

bool CollectionFactory::CollectionExists(const Address& aAddrColID)
{
   std::vector<std::shared_ptr<Collection>>::iterator iter_cols = m_lstCollections.begin();
   for (; iter_cols != m_lstCollections.end(); ++iter_cols)
   {
      if (aAddrColID == (*iter_cols)->GetIdentifier())
      {
         return true;
      }
   }
   return false;
}

Collection* CollectionFactory::GetCollection(std::string aszCollectionName)
{
   std::vector<std::shared_ptr<Collection>>::iterator iter_cols = m_lstCollections.begin();
   for (; iter_cols != m_lstCollections.end(); ++iter_cols)
   {
      if (Address(aszCollectionName) == (*iter_cols)->GetIdentifier())
      {
         return iter_cols->get();
      }
   }
   return nullptr;
}

std::string CollectionFactory::getNextChildName(std::string aszParentID)
{
   int iID;
   std::vector<std::string> lstUIandPF = StringHelper::Str_Split(aszParentID, std::string("-"));
   if (lstUIandPF.size() == 1)
   {
      iID = 1;
   }
   else
   {
      iID = std::stoi(lstUIandPF[1]);
   }

   // To name a child from set UI-C, where C is a composite of 
   //  primes, P_k * P_k-r1 * P_k-r2 ..., take the largest prime in
   //  C, and find the nth following prime, where n is the number of
   //  existing children. i.e. Take P_k if there are no children yet.
   //  Then get C2 = P_k+n * P_k * P_k-r1 * P_k-r2 .... Then use UI -C2
   int iPrimeIndex = 0;

   // Find the largest prime number divisor
   for (size_t i = Addresser::Primes.size()-1; i >= 0; i--)
   {
      int iPrime = Addresser::Primes[i];
      if (iID % iPrime == 0)
      {
         iPrimeIndex = i;
         break;
      }
   }

   if (iPrimeIndex == 0) { iPrimeIndex = 1; }

   int iChildren = GetCollection(aszParentID)->GetChildCount();
   iPrimeIndex += iChildren;

   int iChild = GetCollection(aszParentID)->GetChildCount();
   std::string szRetval = lstUIandPF[0] + "-" + std::to_string(iID*(Addresser::Primes[iPrimeIndex]));
   return szRetval;
}