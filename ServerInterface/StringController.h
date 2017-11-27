#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "MarshalHelper.h"

public ref class StringController
{
public:
   StringController();

   System::Boolean ParseCardString( String^ ahszInput,
                                    System::Int32% rhiCount,
                                    String^% rhszName,
                                    List<HTag^>^% rvecPairDets,
                                    List<HTag^>^% rvecPairMetas );
};

