#include "AddressTest.h"
#include "Addresser.h"
#include<string>

using namespace std;

AddressTest::AddressTest()
{
}


AddressTest::~AddressTest()
{
}

bool 
AddressTest::ParseTestSingle()
{
   bool bResult = true;
   string szTestParse = "IDTest";

   Address parseAddr(szTestParse);

   bResult &= parseAddr.GetMain() == szTestParse;

   auto vecSubAddresses = parseAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 1 && vecSubAddresses[0] == 1;

   return bResult;
}

bool 
AddressTest::ParseTestManySub()
{
   bool bResult = true;
   string szTestParse = "IDTest-2,15";

   Address parseAddr(szTestParse);

   bResult &= parseAddr.GetMain() == "IDTest";

   auto vecSubAddresses = parseAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 2 && 
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 15;

   return bResult;
}

bool 
AddressTest::IsResidentInTest()
{
   // Tests that IsResident in
   // returns the subaddress that
   // identify the location.
   Addresser addr;

   bool bResult = true;
   string szParentAddress = "IDTest-3";
   string szChildAddress = "IDTest-2,9,15";

   // Parent would be a collection.
   Location parentLocation(szParentAddress);
   Address childAddr(szChildAddress);
   Address dummy;

   bResult &= addr.DoesAddressIncludeLocation(childAddr, parentLocation, dummy);
   
   auto vecSubAddresses = dummy.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 9 &&
              vecSubAddresses[1] == 15;

   bResult &= parentLocation.IsResidentIn(childAddr, dummy);
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 9 &&
              vecSubAddresses[1] == 15;

   return bResult;
}

bool 
AddressTest::PitheLocationTest()
{
   bool bResult = true;

   Addresser addr;

   string szLocation = "IDTest-3";
   string szAddress = "IDTest-2,9,15";

   Location locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addr.PitheLocation(addressAddr, locationAddr);

   auto vecSubAddresses = addressAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 1 &&
              vecSubAddresses[0] == 2;

   szLocation = "IDTest-5";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= !addr.PitheLocation(addressAddr, locationAddr);

   vecSubAddresses = addressAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 3 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 9 &&
              vecSubAddresses[2] == 15;

   // Tests that subaddresses that collide after
   // pithing are combined.
   szLocation = "IDTest-9";
   szAddress = "IDTest-2,27,45";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   bResult &= addr.PitheLocation(addressAddr, locationAddr);
   
   vecSubAddresses = addressAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 3;

   return bResult;
}

bool 
AddressTest::InceptLocationTest()
{
   bool bResult = true;

   Addresser addr;

   string szLocation = "IDTest-9";
   string szAddress = "IDTest-2";

   Location locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addr.InceptLocation(addressAddr, locationAddr);

   auto vecSubAddresses = addressAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 9;

   szLocation = "IDTest-27";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= addr.InceptLocation(addressAddr, locationAddr);

   vecSubAddresses = addressAddr.GetSubAddresses();
   bResult &= vecSubAddresses.size() == 3 &&
              vecSubAddresses[0] == 2  &&
              vecSubAddresses[1] == 27 &&
              vecSubAddresses[2] == 15;

   return bResult;
}