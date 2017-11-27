#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include<string>
#include<vector>

typedef std::pair<std::string, std::string> Tag;

typedef System::String String;
typedef System::Tuple<String^, String^> HTag;
template<typename T> using List = System::Collections::Generic::List<T>;
/*
using namespace std;

ref class MarshalHelper
{
public:
   MarshalHelper();

	List<String^>^ convertStrVecToLst(vector<string> alstTrans);
   List<HTag^>^ convertTagVecToLst(vector<Tag> alstTups);

	vector<Tag> revertTupLstToVec(List<HTag^>^ hlstMetaTagsOne);
   vector<string> revertStrLstToVec(List<String^>^ hlstChanges);
};

*/