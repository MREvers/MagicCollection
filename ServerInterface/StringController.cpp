#include "StringController.h"

#include "..\MagicCollection\StringInterface.h"

using namespace std;

StringController::StringController()
{
}

System::Boolean 
StringController::ParseCardString( String^ ahszInput,
                                   System::Int32% rhiCount,
                                   String^% rhszName,
                                   List<HTag^>^% rvecPairDets,
                                   List<HTag^>^% rvecPairMetas )
{
   string szName;
   unsigned int iCount;
   vector<Tag> vecDetTags, vecMetaTags;

   String^ hszITemp = ahszInput;
   string szInput =  msclr::interop::marshal_as<string>(hszITemp);

   StringInterface parser;
   bool bGoodParse = parser.ParseCardLine( szInput, iCount, szName,
                                           vecDetTags, vecMetaTags );

   if( bGoodParse )
   {
      rhiCount = iCount;
      rhszName = gcnew String(szName.c_str());
      rvecPairDets = MarshalHelper::ConvertTagVecToLst(vecDetTags);
      rvecPairMetas = MarshalHelper::ConvertTagVecToLst(vecMetaTags);
   }

   return System::Boolean(bGoodParse);
}

System::Boolean 
StringController::ParseListDelimString( String^ ahszInput,
                                        List<String^>^% rlstRetval )
{
   StringInterface parser;
   vector<string> vecItems;
   string szInput =  msclr::interop::marshal_as<string>(ahszInput);

   bool bGoodParse = parser.ParseListDelimString(szInput, vecItems);
   if( bGoodParse )
   {
      rlstRetval = MarshalHelper::ConvertStrVecToLst(vecItems);
   }

   return System::Boolean(bGoodParse);
}