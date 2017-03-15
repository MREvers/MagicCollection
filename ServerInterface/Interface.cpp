#include "Interface.h"

ServerClientInterface::ServerClientInterface()
{
	m_StoreFrontBackEnd = new CStoreFrontBackEnd();
}

ServerClientInterface::~ServerClientInterface()
{

}

void ServerClientInterface::AddItem(System::String^ ahszCollectionName, System::String^ ahszCardNameLong,
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ ahlstMeta)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szCardName = msclr::interop::marshal_as<std::string>(ahszCardNameLong);
	std::vector<std::pair<std::string, std::string>> lstMeta;
	for (int i = 0; i < ahlstMeta->Count; i++)
	{
		std::string szKey = msclr::interop::marshal_as<std::string>(ahlstMeta[i]->Item1);
		std::string szValue = msclr::interop::marshal_as<std::string>(ahlstMeta[i]->Item2);
		lstMeta.push_back(std::make_pair(szKey, szValue));
	}
	m_StoreFrontBackEnd->AddItem(szCollectionName, szCardName, lstMeta);
}

void ServerClientInterface::RemoveItem(
	System::String^ ahszCollectionName,
	System::String^ ahszCardNameLong,
	System::Collections::Generic::List<System::Tuple<System::String^,System::String^>^>^ ahlstMeta)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szCardName = msclr::interop::marshal_as<std::string>(ahszCardNameLong);
	std::vector<std::pair<std::string, std::string>> lstMeta;
	for (int i = 0; i < ahlstMeta->Count; i++)
	{
		std::string szKey = msclr::interop::marshal_as<std::string>(ahlstMeta[i]->Item1);
		std::string szValue = msclr::interop::marshal_as<std::string>(ahlstMeta[i]->Item2);
		lstMeta.push_back(std::make_pair(szKey, szValue));
	}
	m_StoreFrontBackEnd->RemoveItem(szCollectionName, szCardName, lstMeta);
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

void
ServerClientInterface::LoadBulkChanges(
   System::String^ ahszCollectionName,
   System::Collections::Generic::List<System::String^>^ ahlstChanges)
{
   std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
   std::vector<std::string> lstChanges = stringListToVector(ahlstChanges);
   m_StoreFrontBackEnd->LoadBulkChanges(szCollectionName, lstChanges);
}

System::Collections::Generic::Dictionary<System::String^, System::String^>^ ServerClientInterface::GetCopyLocations(System::String^ ahszCollectionName, System::String^ ahszLongCardName)
{
	System::Collections::Generic::Dictionary<System::String^, System::String^>^ hlstRetval = gcnew System::Collections::Generic::Dictionary<System::String^, System::String^>();
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongCardName);
	/*
	std::vector<std::string> lstLocations = m_StoreFrontBackEnd->GetCardLocations(szCollectionName, szLongCardName);
	std::vector<std::string>::iterator iter_Locations = lstLocations.begin();
	for (; iter_Locations != lstLocations.end(); ++iter_Locations)
	{
		System::String^ hszLocations = gcnew System::String(iter_Locations->c_str());
		hlstRetval->Add(hszLocations);
	}
	*/
	return hlstRetval;
}

System::Collections::Generic::List<System::String^>^ 
ServerClientInterface::GetCollectionList(System::String^ ahszCollectionName)
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

System::Collections::Generic::List<
	System::Tuple<
	System::String^, System::String^>^>^ ServerClientInterface::GetCardAttributes(System::String^ hszCardNameLong)
{
	std::string szCardNameLong = msclr::interop::marshal_as<std::string>(hszCardNameLong);
	std::vector<std::pair<std::string, std::string>> LstAttrs = m_StoreFrontBackEnd->GetCardAttributes(szCardNameLong);
	System::Collections::Generic::List<
		System::Tuple<
		System::String^, System::String^>^>^ hlstRetVal = gcnew
		System::Collections::Generic::List<
		System::Tuple<
		System::String^, System::String^>^>();
	std::vector<std::pair<std::string, std::string>>::iterator iter_Attrs = LstAttrs.begin();
	for (; iter_Attrs != LstAttrs.end(); ++iter_Attrs)
	{
		System::String^ hszKey = gcnew System::String(iter_Attrs->first.c_str());
		System::String^ hszVal = gcnew System::String(iter_Attrs->second.c_str());

		System::Tuple<System::String^, System::String^>^ hTuple = gcnew
			System::Tuple<System::String^, System::String^>(hszKey, hszVal);
		hlstRetVal->Add(hTuple);
	}
	return hlstRetVal;
}

// [ { card name - long, [ <tags> ] }, ... }
System::Collections::Generic::List<
	System::Tuple<
		System::String^,
		System::Collections::Generic::List<
			System::Tuple<
				System::String^,
				System::String^>^>^>^>^
ServerClientInterface::GetCollectionListWithMeta(System::String^ ahszCollectionName)
{
	System::Collections::Generic::List<
		System::Tuple<
		System::String^,
		System::Collections::Generic::List<
		System::Tuple<
		System::String^,
		System::String^>^>^>^>^
		
		hlstRetval = gcnew System::Collections::Generic::List<

		System::Tuple<
		System::String^,
		System::Collections::Generic::List<
		System::Tuple<
		System::String^,
		System::String^>^>^>^>();

	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>> 
		lstColList = m_StoreFrontBackEnd->GetCollectionListWithMeta(szCollectionName);
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>::iterator 
		iter_colCards = lstColList.begin();
	for (; iter_colCards != lstColList.end(); ++iter_colCards)
	{
		System::String^ hszCard = gcnew System::String(iter_colCards->first.c_str());
	
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^
			hlstMetaTags = gcnew
			System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>();
		std::vector<std::pair<std::string, std::string>> lstTags = iter_colCards->second;
		std::vector<std::pair<std::string, std::string>>::iterator iter_Tags = lstTags.begin();
		for (; iter_Tags != lstTags.end(); ++iter_Tags)
		{
			System::String^ hszKey = gcnew System::String(iter_Tags->first.c_str());
			System::String^ hszVal = gcnew System::String(iter_Tags->second.c_str());
			System::Tuple<System::String^, System::String^>^ hPair =
				gcnew System::Tuple<System::String^, System::String^>(hszKey, hszVal);
			hlstMetaTags->Add(hPair);
		}
		
		System::Tuple<
			System::String^,
			System::Collections::Generic::List<
			System::Tuple<
			System::String^,
			System::String^>^>^>^ hNameTagPair =
			gcnew System::Tuple<
			System::String^,
			System::Collections::Generic::List<
			System::Tuple<
			System::String^,
			System::String^>^>^>(hszCard, hlstMetaTags);

		hlstRetval->Add(hNameTagPair);

	}

	return hlstRetval;
}

System::Collections::Generic::List<System::String^>^ 
ServerClientInterface::GetCardAttributeRestrictions(System::String^ ahszLongName, System::String^ ahszKey)
{
	std::string szKey = msclr::interop::marshal_as<std::string>(ahszKey);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);
	return stringVectorToList(m_StoreFrontBackEnd->GetCardAttributeRestriction(szLongCardName, szKey));
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

System::Collections::Generic::List<System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^>^
ServerClientInterface::GetMetaTags(System::String^ ahszCollectionName, System::String^ ahszLongName)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);
	std::vector<std::vector<std::pair<std::string, std::string>>> lstTagListList = m_StoreFrontBackEnd->GetMetaTags(szCollectionName, szLongCardName);
	System::Collections::Generic::List<System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^>^
		hlstRetVal = gcnew
		System::Collections::Generic::List<System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^>();
	std::vector<std::vector<std::pair<std::string, std::string>>>::iterator iter_MetaTags = lstTagListList.begin();
	for (; iter_MetaTags != lstTagListList.end(); ++iter_MetaTags)
	{
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^
			hlstMetaTags = gcnew
			System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>();
		std::vector<std::pair<std::string, std::string>>::iterator iter_MetaTagList = iter_MetaTags->begin();
		for (; iter_MetaTagList != iter_MetaTags->end(); ++iter_MetaTagList)
		{
			System::String^ hszKey = gcnew System::String(iter_MetaTagList->first.c_str());
			System::String^ hszVal = gcnew System::String(iter_MetaTagList->second.c_str());
			System::Tuple<System::String^, System::String^>^ hPair =
				gcnew System::Tuple<System::String^, System::String^>(hszKey, hszVal);
			hlstMetaTags->Add(hPair);
		}
		hlstRetVal->Add(hlstMetaTags);
	}

	return hlstRetVal;
}

void ServerClientInterface::AddMetaTag(System::String^ ahszCollectionName, System::String^ ahszLongName, System::String^ ahszKey, System::String^ ahszVal,
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);
	std::string szKey = msclr::interop::marshal_as<std::string>(ahszKey);
	std::string szVal = msclr::interop::marshal_as<std::string>(ahszVal);
	std::vector<std::pair<std::string, std::string>> lstMetaTagPairs;
	for (int i = 0; i < hlstMetaTags->Count; i++)
	{
		std::pair<std::string, std::string> pair;
		std::string szFirst = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item1);
		std::string szSecond = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item2);
		pair.first = szFirst;
		pair.second = szSecond;
		lstMetaTagPairs.push_back(pair);
	}

	m_StoreFrontBackEnd->AddMetaTag(szCollectionName, szLongCardName, szKey, szVal, lstMetaTagPairs);
}

void ServerClientInterface::SetMetaTags(System::String^ ahszCollectionName,
   System::String^ ahszLongName,
   System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewTags,
   System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
   std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
   std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);

   std::vector<std::pair<std::string, std::string>> lstNewTags = tupleListToVector(hlstNewTags);

   std::vector<std::pair<std::string, std::string>> lstMetaTagPairs;
   for (int i = 0; i < hlstMetaTags->Count; i++)
   {
      std::pair<std::string, std::string> pair;
      std::string szFirst = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item1);
      std::string szSecond = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item2);
      pair.first = szFirst;
      pair.second = szSecond;
      lstMetaTagPairs.push_back(pair);
   }

   m_StoreFrontBackEnd->SetMetaTags(szCollectionName, szLongCardName, lstNewTags, lstMetaTagPairs);
}

void ServerClientInterface::AddMetaTag(
	System::String^ ahszCollectionName,
	System::String^ ahszLongName,
	System::String^ ahszKey,
	System::String^ ahszSubKey,
	System::String^ ahszVal,
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);
	std::string szKey = msclr::interop::marshal_as<std::string>(ahszKey);
	std::string szSubKey = msclr::interop::marshal_as<std::string>(ahszSubKey);
	std::string szVal = msclr::interop::marshal_as<std::string>(ahszVal);
	std::vector<std::pair<std::string, std::string>> lstMetaTagPairs;
	for (int i = 0; i < hlstMetaTags->Count; i++)
	{
		std::pair<std::string, std::string> pair;
		std::string szFirst = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item1);
		std::string szSecond = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item2);
		pair.first = szFirst;
		pair.second = szSecond;
		lstMetaTagPairs.push_back(pair);
	}

	m_StoreFrontBackEnd->AddMetaTag(szCollectionName, szLongCardName, szKey, szSubKey, szVal, lstMetaTagPairs);
}

void ServerClientInterface::RemoveMetaTag(System::String^ ahszCollectionName,
	System::String^ ahszLongName,
	System::String^ ahszKey,
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
	std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
	std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);
	std::string szKey = msclr::interop::marshal_as<std::string>(ahszKey);
	std::vector<std::pair<std::string, std::string>> lstMetaTagPairs;
	for (int i = 0; i < hlstMetaTags->Count; i++)
	{
		std::pair<std::string, std::string> pair;
		std::string szFirst = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item1);
		std::string szSecond = msclr::interop::marshal_as<std::string>(hlstMetaTags[i]->Item2);
		pair.first = szFirst;
		pair.second = szSecond;
		lstMetaTagPairs.push_back(pair);
	}

	m_StoreFrontBackEnd->RemoveMetaTag(szCollectionName, szLongCardName, szKey, lstMetaTagPairs);
}

void ServerClientInterface::SetFeatures(System::String^ ahszCollectionName,
   System::String^ ahszLongName,
   System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewTags,
   System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewAttsr,
   System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
   std::string szCollectionName = msclr::interop::marshal_as<std::string>(ahszCollectionName);
   std::string szLongCardName = msclr::interop::marshal_as<std::string>(ahszLongName);

   std::vector<std::pair<std::string, std::string>> lstNewTags = tupleListToVector(hlstNewTags);

   std::vector<std::pair<std::string, std::string>> lstMetaTagPairs = tupleListToVector(hlstMetaTags);

   std::vector<std::pair<std::string, std::string>> lstNewAttrs = tupleListToVector(hlstNewAttsr);

   m_StoreFrontBackEnd->SetFeatures(szCollectionName, szLongCardName, lstNewTags, lstNewAttrs, lstMetaTagPairs);
}

System::Boolean ServerClientInterface::IsSameIdentity(System::String^ aszLongNameOne, System::String^ aszLongNameTwo)
{
	std::string szLongOne = msclr::interop::marshal_as<std::string>(aszLongNameOne);
	std::string szLongTwo = msclr::interop::marshal_as<std::string>(aszLongNameTwo);
	System::Boolean hbRetVal = System::Boolean(m_StoreFrontBackEnd->IsSameCard(szLongOne, szLongTwo));
	return hbRetVal;
}

System::Boolean ServerClientInterface::IsSameMetaTags(
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsOne,
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsTwo)
{
	std::vector<std::pair<std::string, std::string>> lstMetaOne = tupleListToVector(hlstMetaTagsOne);
	std::vector<std::pair<std::string, std::string>> lstMetaTwo = tupleListToVector(hlstMetaTagsTwo);

	System::Boolean hbRetVal = System::Boolean(m_StoreFrontBackEnd->IsSameMetaTags(lstMetaOne, lstMetaTwo));
	return hbRetVal;
}

void ServerClientInterface::ImportCollection()
{
	m_StoreFrontBackEnd->ImportCollection();
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

std::vector<std::pair<std::string, std::string>> ServerClientInterface::tupleListToVector(
	System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags)
{
	std::vector<std::pair<std::string, std::string>> lstMetaTagPairs;
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