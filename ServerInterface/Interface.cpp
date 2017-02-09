#include "Interface.h"

ServerClientInterface::ServerClientInterface()
{
	m_StoreFrontBackEnd = new CStoreFrontBackEnd();
}

ServerClientInterface::~ServerClientInterface()
{

}

void ServerClientInterface::AddItem(System::String^ ahszCollectionName, System::String^ ahszCardName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szCardName = msclr::interop::marshal_as<std::string>(ahszCardName);
	m_StoreFrontBackEnd->AddItem(szCollectionName, szCardName);
}

void ServerClientInterface::RemoveItem(System::String^ ahszCollectionName, System::String^ ahszCardName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szCardName = msclr::interop::marshal_as<std::string>(ahszCardName);
	m_StoreFrontBackEnd->RemoveItem(szCollectionName, szCardName);
}

MCopyObject^ ServerClientInterface::ConvertItemToCopyObject(System::String^ ahszCard)
{
	std::string szCard = msclr::interop::marshal_as<std::string>(ahszCard);

	int iCount = 0;
	std::string szCardName = "";
	std::string szDetails = "";
	bool success = m_StoreFrontBackEnd->ParseCardString(szCard, iCount, szCardName, szDetails);

	MCopyObject^ roCO = gcnew MCopyObject();
	if (success)
	{
		std::vector<std::pair<std::string, std::string>> lstAttrs = m_StoreFrontBackEnd->ParseAttrs(szDetails);
		System::Collections::Generic::Dictionary<System::String^, System::String^>^ mapAttrs =
			gcnew System::Collections::Generic::Dictionary<System::String^, System::String^>();

		std::vector<std::pair<std::string, std::string>>::iterator iter_attrs = lstAttrs.begin();
		for (; iter_attrs != lstAttrs.end(); ++iter_attrs)
		{
			System::String^ hszVal = gcnew System::String(iter_attrs->second.c_str());
			System::String^ hszKey = gcnew System::String(iter_attrs->first.c_str());
			mapAttrs->Add(hszKey, hszVal);
		}

		System::String^ hszCardName = gcnew System::String(szCardName.c_str());
		
		roCO->Amount = iCount;
		roCO->Name = hszCardName;
		roCO->Attributes = mapAttrs;
	}

	return roCO;
}

void ServerClientInterface::SaveCollection(System::String^ ahszCollectionName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	m_StoreFrontBackEnd->SaveCollection(szCollectionName);
}

System::String^ ServerClientInterface::LoadCollection(System::String^ ahszCollectionFileName)
{
	std::string szCollectionFileName = msclr::interop::marshal_as<std::string>(ahszCollectionFileName);
	std::string szCollectionName = m_StoreFrontBackEnd->LoadCollection(szCollectionFileName);
	return gcnew System::String(szCollectionName.c_str());
}

System::Collections::Generic::List<System::String^>^ ServerClientInterface::GetCollectionList(System::String^ ahszCollectionName)
{
	System::Collections::Generic::List<System::String^>^ hlstRetval = gcnew System::Collections::Generic::List<System::String^>();

	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::vector<std::string> lstColList = m_StoreFrontBackEnd->GetCollectionList(szCollectionName);
	std::vector<std::string>::iterator iter_colCards = lstColList.begin();
	for (; iter_colCards != lstColList.end(); ++iter_colCards)
	{
		System::String^ hszCard = gcnew System::String(iter_colCards->c_str());
		hlstRetval->Add(hszCard);
	}

	return hlstRetval;
}

System::Collections::Generic::List<System::String^>^ ServerClientInterface::GetLoadedCollections()
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