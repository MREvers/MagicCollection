#include "StoreFrontBackEnd.h"


CStoreFrontBackEnd::CStoreFrontBackEnd()
{
   // No Server for now
   m_ColSource = new CollectionSource();
   m_ColSource->LoadLib(Config::Instance()->GetSourceFile());

   m_ColFactory = new CollectionFactory(m_ColSource);
}


CStoreFrontBackEnd::~CStoreFrontBackEnd()
{
}

void CStoreFrontBackEnd::SaveCollection(std::string aszCollectionName)
{
   if (m_ColFactory->CollectionExists(aszCollectionName))
   {
      m_ColFactory->GetCollection(aszCollectionName)->SaveCollection();
   }
}

std::string CStoreFrontBackEnd::LoadCollection(std::string aszCollectionFile)
{
   return m_ColFactory->LoadCollectionFromFile(aszCollectionFile);
}

std::string CStoreFrontBackEnd::CreateNewCollection(std::string aszCollectionName, std::string aszParent)
{
   return m_ColFactory->CreateNewCollection(aszCollectionName, aszParent);
}

std::vector<std::string> CStoreFrontBackEnd::GetLoadedCollections()
{
   return m_ColFactory->GetLoadedCollections();
}

std::vector<std::string> CStoreFrontBackEnd::GetCollectionMetaData(std::string aszCollection)
{
   if (m_ColFactory->CollectionExists(aszCollection))
   {
      return m_ColFactory->GetCollection(aszCollection)->GetMetaData();
   }
   else
   {
      std::vector<std::string> noRetval;
      return noRetval;
   }
}

std::vector<std::string>
CStoreFrontBackEnd::GetCollectionList(std::string aszCollection, int aiVisibility, bool bCollapsed)
{
   if (m_ColFactory->CollectionExists(aszCollection))
   {
      return m_ColFactory->GetCollection(aszCollection)->GetCollectionList((MetaTagType)aiVisibility, bCollapsed);
   }
   else
   {
      std::vector<std::string> lstEmpty;
      return lstEmpty;
   }
}

std::string CStoreFrontBackEnd::GetCardPrototype(std::string aszCardName)
{
   int iValidCard = m_ColSource->LoadCard(aszCardName);
   if (iValidCard != -1) 
   {
      return m_ColSource->GetCardPrototype(iValidCard)->GetProtoTypeString();
   }
   else
   {
      return "";
   }
}

std::vector<std::string> CStoreFrontBackEnd::GetAllCardsStartingWith(std::string aszSearch)
{
   return m_ColSource->GetAllCardsStartingWith(aszSearch);
}

std::string CStoreFrontBackEnd::GetImagesPath()
{
   return Config::Instance()->GetImagesFolder();
}

void CStoreFrontBackEnd::SubmitBulkChanges(std::string aszCollection, std::vector<std::string> alstChanges)
{
   if (m_ColFactory->CollectionExists(aszCollection))
   {
      m_ColFactory->GetCollection(aszCollection)->LoadChanges(alstChanges);
   }
}

void CStoreFrontBackEnd::ImportCollection()
{
   JSONImporter JI;
   JI.ImportJSON(Config::Instance()->GetImportSourceFile());
}