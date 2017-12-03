#include "Interface.h"

ServerClientInterface::ServerClientInterface()
{
	m_StoreFrontBackEnd = new CStoreFrontBackEnd();
}

ServerClientInterface::~ServerClientInterface()
{
   delete m_StoreFrontBackEnd;
}

String^ ServerClientInterface::LoadCollection(String^ ahszCollectionFileName)
{
	string szCollectionFileName = msclr::interop::marshal_as<string>(ahszCollectionFileName);
	string szCollectionName = m_StoreFrontBackEnd->LoadCollection(szCollectionFileName);
	return gcnew System::String(szCollectionName.c_str());
}

String^ ServerClientInterface::CreateNewCollection(String^ aszCollectionName, String^ ahszParent)
{
	string szCollectionFileName = msclr::interop::marshal_as<string>(aszCollectionName);
	string szParentName = msclr::interop::marshal_as<string>(ahszParent);
	return gcnew String(m_StoreFrontBackEnd->CreateNewCollection(szCollectionFileName, szParentName).c_str());
}

void ServerClientInterface::SaveCollection(String^ aszCollectionName)
{
	string szCollectionName = msclr::interop::marshal_as<string>(aszCollectionName);
	m_StoreFrontBackEnd->SaveCollection(szCollectionName);
}

String^ ServerClientInterface::GetSourceFilePath()
{
   return gcnew String(m_StoreFrontBackEnd->GetSourceFilePath().c_str());
}

String^ ServerClientInterface::GetImportSourceFilePath()
{
   return gcnew String(m_StoreFrontBackEnd->GetImportSourceFilePath().c_str());
}

String^
ServerClientInterface::GetImagesPath()
{
	return gcnew String(m_StoreFrontBackEnd->GetImagesPath().c_str());
}

List<HTag^>^ 
ServerClientInterface::GetPairedAttributes()
{
   return MarshalHelper::ConvertTagVecToLst(m_StoreFrontBackEnd->GetPairedAttributes());
}

String^ 
ServerClientInterface::GetCardPrototype(String^ ahszCardName)
{
	string szCardName = msclr::interop::marshal_as<string>(ahszCardName);
	return gcnew String(m_StoreFrontBackEnd->GetCardPrototype(szCardName).c_str());
}

List<String^>^
ServerClientInterface::GetLoadedCollections()
{
	List<String^>^ hlstRetval = gcnew List<String^>();
	vector<string> lstColList = m_StoreFrontBackEnd->GetLoadedCollections();
	vector<string>::iterator iter_colCards = lstColList.begin();
	for (; iter_colCards != lstColList.end(); ++iter_colCards)
	{
		System::String^ hszCard = gcnew System::String(iter_colCards->c_str());
		hlstRetval->Add(hszCard);
	}
	return hlstRetval;
}

List<String^>^ 
ServerClientInterface::GetAllCardsStartingWith(String^ ahszText)
{
	string szText = msclr::interop::marshal_as<string>(ahszText);
	vector<string> lstCardList = m_StoreFrontBackEnd->GetAllCardsStartingWith(szText);
	List<String^>^ hlstCardList = gcnew List<String^>();
	
	vector<string>::iterator iter_Cards = lstCardList.begin();
	for (; iter_Cards != lstCardList.end(); ++iter_Cards)
	{
		String^ hszCard = gcnew String(iter_Cards->c_str());
		hlstCardList->Add(hszCard);
	}
	return hlstCardList;
}

List<String^>^
ServerClientInterface::GetCollectionMetaData(String^ ahszCollectionName)
{
	string szCollectionName = msclr::interop::marshal_as<string>(ahszCollectionName);
	vector<string> lstMeta = m_StoreFrontBackEnd->GetCollectionMetaData(szCollectionName);
	return MarshalHelper::ConvertStrVecToLst(lstMeta);
}

// [ { card name - long, [ <tags> ] }, ... }
List<String^>^
ServerClientInterface::GetCollectionList(String^ ahszCollectionName, System::Int32 ahiVisibility)
{
	string szCollectionName = msclr::interop::marshal_as<string>(ahszCollectionName);
	// int iVisibility = 0xF; // Everything
   int iVisibility = ahiVisibility;

	vector<string> lstCollection = m_StoreFrontBackEnd->GetCollectionList(szCollectionName, iVisibility);

	List<String^>^ hlstCollection;
	hlstCollection = MarshalHelper::ConvertStrVecToLst(lstCollection);

	return hlstCollection;
}

void ServerClientInterface::SubmitBulkChanges(String^ ahszCollectionName, List<String^>^ ahlstBulkChanges)
{
	string szCollection = msclr::interop::marshal_as<string>(ahszCollectionName);
	vector<string> lstChanges = MarshalHelper::RevertStrLstToVec(ahlstBulkChanges);
	m_StoreFrontBackEnd->SubmitBulkChanges(szCollection, lstChanges);
}

void 
ServerClientInterface::SetAttribute( String^ ahszCardName, String^ ahszUID, String^ ahszKey, String^ ahszVal )
{
   string szCardName = msclr::interop::marshal_as<string>(ahszCardName);
   string szUID = msclr::interop::marshal_as<string>(ahszUID);
   string szKey = msclr::interop::marshal_as<string>(ahszKey);
   string szVal = msclr::interop::marshal_as<string>(ahszVal);

   m_StoreFrontBackEnd->SetAttribute(szCardName, szUID, szKey, szVal);
}

List<HTag^> ^
ServerClientInterface::GetMetaTags( String^ ahszCardName, String^ ahszUID )
{
   string szCardName = msclr::interop::marshal_as<string>(ahszCardName);
   string szUID = msclr::interop::marshal_as<string>(ahszUID);

   return MarshalHelper::ConvertTagVecToLst(m_StoreFrontBackEnd->GetMetaTags(szCardName, szUID));
}

List<HTag^>^ 
ServerClientInterface::GetIdentifyingAttributes( String^ ahszCardName, String^ ahszUID )
{
   string szCardName = msclr::interop::marshal_as<string>(ahszCardName);
   string szUID = msclr::interop::marshal_as<string>(ahszUID);

   return MarshalHelper::ConvertTagVecToLst(m_StoreFrontBackEnd->GetIdentifyingAttributes(szCardName, szUID));
}

String^ 
ServerClientInterface::GetCardString( String^ ahszCardName, String^ ahszUID )
{
   string szCardName = msclr::interop::marshal_as<string>(ahszCardName);
   string szUID = msclr::interop::marshal_as<string>(ahszUID);
   String^ hszCard = gcnew String(m_StoreFrontBackEnd->GetCardString(szCardName, szUID).c_str());
   return hszCard;
}

void
ServerClientInterface::ImportCollection()
{
	m_StoreFrontBackEnd->ImportCollectionSource();
}

System::Boolean 
ServerClientInterface::IsConfigLoaded()
{
   return m_StoreFrontBackEnd->ConfigIsLoaded();
}
