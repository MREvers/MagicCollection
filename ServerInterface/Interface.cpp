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
   return convertTagVecToLst(m_StoreFrontBackEnd->GetPairedAttributes());
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

List<String^>^ ServerClientInterface::GetAllCardsStartingWith(String^ ahszText)
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
	return convertStrVecToLst(lstMeta);
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
	hlstCollection = convertStrVecToLst(lstCollection);

	return hlstCollection;
}

void ServerClientInterface::SubmitBulkChanges(String^ ahszCollectionName, List<String^>^ ahlstBulkChanges)
{
	string szCollection = msclr::interop::marshal_as<string>(ahszCollectionName);
	vector<string> lstChanges = revertStrLstToVec(ahlstBulkChanges);
	m_StoreFrontBackEnd->SubmitBulkChanges(szCollection, lstChanges);
}

void ServerClientInterface::SetAttribute( String^ ahszCardName, String^ ahszUID, String^ ahszKey, String^ ahszVal )
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

   return convertTagVecToLst(m_StoreFrontBackEnd->GetMetaTags(szCardName, szUID));
}

void ServerClientInterface::ImportCollection()
{
	m_StoreFrontBackEnd->ImportCollectionSource();
}

System::Boolean 
ServerClientInterface::IsConfigLoaded()
{
   return m_StoreFrontBackEnd->ConfigIsLoaded();
}

List<String^>^
ServerClientInterface::convertStrVecToLst(vector<string> alstTrans)
{
	List<String^>^ hlstRetVal = gcnew List<String^>();
	vector<string>::iterator iter_stringList = alstTrans.begin();
	for (; iter_stringList != alstTrans.end(); ++iter_stringList)
	{
		hlstRetVal->Add(gcnew String(iter_stringList->c_str()));
	}

	return hlstRetVal;
}

List<HTag^>^ 
ServerClientInterface::convertTagVecToLst( vector<pair<string, string>> alstTups )
{
   List<HTag^>^ hlstRetVal = gcnew List<HTag^>();
   for( auto pair : alstTups )
   {
      String^ hszOne = gcnew String(pair.first.c_str());
      String^ hszTwo = gcnew String(pair.second.c_str());
      hlstRetVal->Add(gcnew System::Tuple<String^, String^>(hszOne, hszTwo));
   }

   return hlstRetVal;
}

vector<Tag> 
ServerClientInterface::revertTupLstToVec(List<HTag^>^ hlstMetaTags)
{
	vector<Tag> lstMetaTagPairs;
	for (int i = 0; i < hlstMetaTags->Count; i++)
	{
		pair<string, string> pair;
		string szFirst = msclr::interop::marshal_as<string>(hlstMetaTags[i]->Item1);
		string szSecond = msclr::interop::marshal_as<string>(hlstMetaTags[i]->Item2);
		pair.first = szFirst;
		pair.second = szSecond;
		lstMetaTagPairs.push_back(pair);
	}
	return lstMetaTagPairs;
}

vector<string>
ServerClientInterface::revertStrLstToVec(List<String^>^ hlstChanges)
{
   vector<string> lstStrings;
   for (int i = 0; i < hlstChanges->Count; i++)
   {
      string szFirst = msclr::interop::marshal_as<string>(hlstChanges[i]);
      lstStrings.push_back(szFirst);
   }
   return lstStrings;
}
