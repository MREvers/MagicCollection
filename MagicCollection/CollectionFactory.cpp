#include "CollectionFactory.h"


CollectionFactory::CollectionFactory(CollectionSource* aoColSource)
{
   m_ColSource = aoColSource;
   m_lstCollections.reserve(10);
}


CollectionFactory::~CollectionFactory()
{
}

std::string 
CollectionFactory::LoadCollectionFromFile(std::string aszFileName)
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
      oCol = new Collection(aszColName, m_ColSource,
                            aszColName, getNextChildName(aszParentID));
      GetCollection(aszParentID)->ChildAdded();
   }
   else
   {
      oCol = new Collection(aszColName, m_ColSource, aszColName);
   }

   m_lstCollections.push_back(std::shared_ptr<Collection>(oCol));
   return oCol->GetIdentifier().GetFullAddress();
}

std::vector<std::string>
CollectionFactory::GetLoadedCollections()
{
   std::vector<std::string> lstRetval;
   std::vector<std::shared_ptr<Collection>>::iterator iter_Colo;
   for ( iter_Colo = m_lstCollections.begin();
         iter_Colo != m_lstCollections.end(); 
         ++iter_Colo)
   {
      Collection* curr_Col = iter_Colo->get();
      lstRetval.push_back(curr_Col->GetIdentifier().GetFullAddress());
   }
   return lstRetval;
}

bool 
CollectionFactory::CollectionExists(std::string aszCollectionName)
{
   return CollectionExists(Address(aszCollectionName));
}

bool 
CollectionFactory::CollectionExists(const Address& aAddrColID)
{
   return GetCollection(aAddrColID).Good();
}

TryGet<Collection> 
CollectionFactory::GetCollection(std::string aszCollectionName)
{
   return GetCollection(Address(aszCollectionName));
}

TryGet<Collection> 
CollectionFactory::GetCollection(const Address& aAddrColID)
{
   TryGet<Collection> oRetVal;

   std::vector<std::shared_ptr<Collection>>::iterator iter_cols;
   for ( iter_cols  = m_lstCollections.begin();
         iter_cols != m_lstCollections.end(); 
         ++iter_cols)
   {
      Collection* curr_Col = iter_cols->get();
      if (aAddrColID == curr_Col->GetIdentifier())
      {
         oRetVal.Set(iter_cols->get());
         break;
      }
   }

   return oRetVal;
}

std::string 
CollectionFactory::getNextChildName(std::string aszParentID)
{
   int iID;
   std::vector<std::string> lstUIandPF;
   
   lstUIandPF = StringHelper::Str_Split(aszParentID, std::string("-"));
   if (lstUIandPF.size() == 1)
   {
      iID = 1;
   }
   else
   {
      iID = std::stoi(lstUIandPF[1]);
   }

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

   std::string szSubAddress;
   std::string szRetval = aszParentID;
   TryGet<Collection> oCol = GetCollection(aszParentID);
   if (oCol.Good())
   {
      int iChildren = oCol->GetChildCount();
      iPrimeIndex += iChildren;
      
      int iChild = oCol->GetChildCount();

      szSubAddress = std::to_string(iID*(Addresser::Primes[iPrimeIndex]));
      szRetval = lstUIandPF[0] + "-" + szSubAddress;
   }

   return szRetval;
}