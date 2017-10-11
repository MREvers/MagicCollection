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

   if( !m_ColSource->IsLoaded() )
   {
      // TODO indicat this
      return szRetVal;
   }

   // Get the file name.
   lstSplitFile = StringHelper::Str_Split(aszFileName, "\\");
   szFile = lstSplitFile[lstSplitFile.size() - 1];
   lstSplitExt = StringHelper::Str_Split(szFile, ".");
   szFile = lstSplitExt[0];

   Collection* oCol = new Collection(Config::NotFoundString, m_ColSource, szFile);
   oCol->LoadCollection(aszFileName, this);
   Location szFoundID = oCol->GetIdentifier();

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
   if( !m_ColSource->IsLoaded() )
   {
      // TODO indicat this
      return Config::NotFoundString;
   }

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
   return CollectionExists(Location(aszCollectionName));
}

bool 
CollectionFactory::CollectionExists(const Location& aAddrColID)
{
   return GetCollection(aAddrColID).Good();
}

TryGet<Collection> 
CollectionFactory::GetCollection(std::string aszCollectionName)
{
   return GetCollection(Location(aszCollectionName));
}

TryGet<Collection> 
CollectionFactory::GetCollection(const Location& aAddrColID)
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
   Addresser addresser;
   unsigned int iHighPrime, iHighPrimeIndex, iCurrentSA;

   Location currentName(aszParentID);
   iCurrentSA = currentName.GetAddress();
   iHighPrimeIndex = addresser.GetHighPrimeIndex(iCurrentSA);

   std::string szSubAddress;
   std::string szRetval = aszParentID;
   TryGet<Collection> oCol = GetCollection(aszParentID);
   if (oCol.Good())
   {
      int iChildren = oCol->GetChildCount();
      iHighPrimeIndex += iChildren;
      
      iHighPrime = addresser.GetPrime(iHighPrimeIndex);
      szSubAddress = std::to_string(iCurrentSA*iHighPrime);
      szRetval = currentName.GetMain() + "-" + szSubAddress;
   }

   return szRetval;
}

void 
CollectionFactory::notifyFamily( std::string aszFamilyName )
{

}