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

void ServerClientInterface::SaveCollection(System::String^ ahszCollectionName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	m_StoreFrontBackEnd->SaveCollection(szCollectionName);
}

void ServerClientInterface::LoadCollection(System::String^ ahszCollectionName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	m_StoreFrontBackEnd->LoadCollection(szCollectionName);
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