#include "Interface.h"

ServerClientInterface::ServerClientInterface()
{
	m_StoreFrontBackEnd = new CStoreFrontBackEnd();
}

ServerClientInterface::~ServerClientInterface()
{

}

System::String^ ServerClientInterface::LoadCollection(System::String^ ahszCollectionFileName)
{
	std::string szCollectionFileName = msclr::interop::marshal_as<std::string>(ahszCollectionFileName);
	std::string szCollectionName = m_StoreFrontBackEnd->LoadCollection(szCollectionFileName);
	return gcnew System::String(szCollectionName.c_str());
}

System::String^
ServerClientInterface::GetImagesPath()
{
	return  gcnew System::String(m_StoreFrontBackEnd->GetImagesPath().c_str());
}

// [ { card name - long, [ <tags> ] }, ... }
System::Collections::Generic::List<System::String^>^
ServerClientInterface::GetCollectionListWithMeta(System::String^ ahszCollectionName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);

	std::vector<std::string> lstCollection = m_StoreFrontBackEnd->GetCollectionList(szCollectionName);

	System::Collections::Generic::List<System::String^>^ hlstCollection;
	hlstCollection = stringVectorToList(lstCollection);

	return hlstCollection;
}

System::Collections::Generic::List<System::String^>^ 
ServerClientInterface::GetLoadedCollections()
{
	System::Collections::Generic::List<System::String^>^ hlstRetval = gcnew System::Collections::Generic::List<System::String^>();
	std::vector<std::string> lstColList = m_StoreFrontBackEnd->GetLoadedCollections();
	std::vector<std::string>::iterator iter_colCards = lstColList.begin();
	for (; iter_colCards != lstColList.end(); ++iter_colCards)
	{
		System::String^ hszCard = gcnew System::String(iter_colCards->c_str());
		hlstRetval->Add(hszCard);
	}
	return hlstRetval;
}

System::Collections::Generic::List<System::String^>^ ServerClientInterface::GetAllCardsStartingWith(System::String^ ahszText)
{
	std::string szText = msclr::interop::marshal_as<std::string>(ahszText);
	std::vector<std::string> lstCardList = m_StoreFrontBackEnd->GetAllCardsStartingWith(szText);
	System::Collections::Generic::List<System::String^>^ hlstCardList = gcnew System::Collections::Generic::List<System::String^>();
	
	std::vector<std::string>::iterator iter_Cards = lstCardList.begin();
	for (; iter_Cards != lstCardList.end(); ++iter_Cards)
	{
		System::String^ hszCard = gcnew System::String(iter_Cards->c_str());
		hlstCardList->Add(hszCard);
	}
	return hlstCardList;
}

void ServerClientInterface::ImportCollection()
{
	//m_StoreFrontBackEnd->ImportCollection();
}

System::Collections::Generic::List<System::String^>^ 
ServerClientInterface::stringVectorToList(std::vector<std::string> alstTrans)
{
	System::Collections::Generic::List<System::String^>^ hlstRetVal = gcnew System::Collections::Generic::List<System::String^>();
	std::vector<std::string>::iterator iter_stringList = alstTrans.begin();
	for (; iter_stringList != alstTrans.end(); ++iter_stringList)
	{
		hlstRetVal->Add(gcnew System::String(iter_stringList->c_str()));
	}
	return hlstRetVal;
}

std::vector<Tag> ServerClientInterface::tupleListToVector(
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
	std::vector<Tag> lstMetaTagPairs;
	for (int i = 0; i < hlstMetaTags->Count; i++)
	{
		std::pair<std::string, std::string> pair;
		std::string szFirst = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item1);
		std::string szSecond = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item2);
		pair.first = szFirst;
		pair.second = szSecond;
		lstMetaTagPairs.push_back(pair);
	}
	return lstMetaTagPairs;
}

std::vector<std::string>
ServerClientInterface::stringListToVector(System::Collections::Generic::List<System::String^>^ hlstChanges)
{
   std::vector<std::string> lstStrings;
   for (int i = 0; i < hlstChanges->Count; i++)
   {
      std::string szFirst = msclr::interop::marshal_as<std::string>(hlstChanges[i]);
      lstStrings.push_back(szFirst);
   }
   return lstStrings;
}
