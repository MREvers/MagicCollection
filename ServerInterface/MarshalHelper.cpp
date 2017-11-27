#include "MarshalHelper.h"



MarshalHelper::MarshalHelper()
{
}


List<String^>^
MarshalHelper::ConvertStrVecToLst(vector<string> alstTrans)
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
MarshalHelper::ConvertTagVecToLst( vector<pair<string, string>> alstTups )
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
MarshalHelper::RevertTupLstToVec(List<HTag^>^ hlstMetaTags)
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
MarshalHelper::RevertStrLstToVec(List<String^>^ hlstChanges)
{
   vector<string> lstStrings;
   for (int i = 0; i < hlstChanges->Count; i++)
   {
      string szFirst = msclr::interop::marshal_as<string>(hlstChanges[i]);
      lstStrings.push_back(szFirst);
   }
   return lstStrings;
}
