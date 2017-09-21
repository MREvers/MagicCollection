#include "TestClass.h"
#include "Addresser.h"
#include<string>
#include<iostream>

using namespace std;

TestClass::TestClass()
{
}


TestClass::~TestClass()
{
}

void 
TestClass::RunTests()
{
   Addresser addresser;
   string szAddr = "IDTest";

   Address tAddr(szAddr);

   if( tAddr.GetFullAddress() != "IDTest-1" )
   {
      cout << "Address Parser Failing" << endl;
   }
   else
   {
      cout << "Address Parser Pass" << endl;
   }

}
