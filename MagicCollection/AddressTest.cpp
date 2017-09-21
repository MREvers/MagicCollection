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

   bResult &= parseAddr.Main == szTestParse;
   bResult &= parseAddr.SubAddresses.size() == 1 && parseAddr.SubAddresses[0] == 1;

   return bResult;
}

bool 
AddressTest::ParseTestManySub()
{
   bool bResult = true;
   string szTestParse = "IDTest-2,15";

   Address parseAddr(szTestParse);

   bResult &= parseAddr.Main == "IDTest";
   bResult &= parseAddr.SubAddresses.size() == 2 && 
              parseAddr.SubAddresses[0] == 2 &&
              parseAddr.SubAddresses[1] == 15;

   return bResult;
}

bool 
AddressTest::IsResidentInTest()
{
   Addresser addr;

   bool bResult = true;
   string szParentAddress = "IDTest-3";
   string szChildAddress = "IDTest-2,9,15";

   Address parentAddr(szParentAddress);
   Address childAddr(szChildAddress);
   Address dummy;

   bResult &= addr.DoesAddressIncludeLocation(childAddr, parentAddr, dummy);
   bResult &= dummy.SubAddresses.size() == 2 &&
              dummy.SubAddresses[0] == 9 &&
              dummy.SubAddresses[1] == 15;

   bResult &= childAddr.IsResidentIn(parentAddr, dummy);
   bResult &= dummy.SubAddresses.size() == 2 &&
              dummy.SubAddresses[0] == 9 &&
              dummy.SubAddresses[1] == 15;

   return bResult;
}

bool 
AddressTest::PitheLocationTest()
{
   bool bResult = true;

   Addresser addr;

   string szLocation = "IDTest-3";
   string szAddress = "IDTest-2,9,15";

   Address locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addr.PitheLocation(addressAddr, locationAddr);
   bResult &= addressAddr.SubAddresses.size() == 1 &&
              addressAddr.SubAddresses[0] == 2;

   szLocation = "IDTest-5";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= !addr.PitheLocation(addressAddr, locationAddr);
   bResult &= addressAddr.SubAddresses.size() == 3 &&
              addressAddr.SubAddresses[0] == 2 &&
              addressAddr.SubAddresses[1] == 9 &&
              addressAddr.SubAddresses[2] == 15;

   szLocation = "IDTest-9";
   szAddress = "IDTest-2,27,45";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= addr.PitheLocation(addressAddr, locationAddr);
   bResult &= addressAddr.SubAddresses.size() == 2 &&
              addressAddr.SubAddresses[0] == 2 &&
              addressAddr.SubAddresses[1] == 3;

   return bResult;
}

bool 
AddressTest::InceptLocationTest()
{
   bool bResult = true;

   Addresser addr;

   string szLocation = "IDTest-9";
   string szAddress = "IDTest-2";

   Address locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addr.InceptLocation(addressAddr, locationAddr);
   bResult &= addressAddr.SubAddresses.size() == 2 &&
              addressAddr.SubAddresses[0] == 2 &&
              addressAddr.SubAddresses[1] == 9;

   szLocation = "IDTest-27";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= addr.InceptLocation(addressAddr, locationAddr);
   bResult &= addressAddr.SubAddresses.size() == 3 &&
              addressAddr.SubAddresses[0] == 2  &&
              addressAddr.SubAddresses[1] == 27 &&
              addressAddr.SubAddresses[2] == 15;

   return bResult;
}