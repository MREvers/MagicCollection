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

using namespace std;

public ref class MarshalHelper
{
public:
   MarshalHelper();

   static List<String^>^ ConvertStrVecToLst(vector<string> alstTrans);
   static List<HTag^>^ ConvertTagVecToLst(vector<Tag> alstTups);

   static vector<string> RevertStrLstToVec(List<String^>^ hlstChanges);
   static vector<Tag> RevertTupLstToVec(List<HTag^>^ hlstMetaTagsOne);
};
