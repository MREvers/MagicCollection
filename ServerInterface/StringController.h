#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

typedef std::pair<std::string, std::string> Tag;

typedef System::String String;
typedef System::Tuple<String^, String^> HTag;
template<typename T> using List = System::Collections::Generic::List<T>;

public ref class StringController
{
public:
   StringController();

   System::Boolean ParseCardString( String^% ahszInput,
                                    System::Int32% rhiCount,
                                    String^% rhszName,
                                    List<HTag^>^% rvecPairDets,
                                    List<HTag^>^% rvecPairMetas );
};

