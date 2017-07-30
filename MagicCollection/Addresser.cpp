#include "Addresser.h"
#include "StringHelper.h"
#include "ListHelper.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

Address::Address() : Main("") { }

// Expects Address-#,#,#
Address::Address(std::string aszColAddress)
{
   vector<unsigned int> lstSubAddressNums; 
   vector<string> lstUIandPF;

   lstUIandPF = StringHelper::Str_Split(aszColAddress, string("-"));

   if (lstUIandPF.size() == 1)
   {
      lstSubAddressNums.push_back(1);
   }
   else
   {
      string szSubAddress; unsigned int iNumChars; unsigned int iSubAddress;
      vector<string> lstSubAddresses;
      lstSubAddresses = StringHelper::Str_Split(lstUIandPF[1], string(","));

      size_t iSAC = lstSubAddresses.size();
      for (size_t i = 0; i < iSAC; i++)
      {
         szSubAddress = lstSubAddresses[i];
         iSubAddress = stoi(szSubAddress, &iNumChars);
         if (iNumChars > 0)
         {
            lstSubAddressNums.push_back(iSubAddress);
         }
      }
   }

   Main = lstUIandPF[0];
   SubAddresses = lstSubAddressNums;
}

Address::Address(std::string aszColBase, unsigned int aiSubAddress)
{
   Main = aszColBase;
   SubAddresses.push_back(aiSubAddress);
}

string
Address::GetFullAddress()
{
   std::string szFullString = Main;
   bool first = true;
   for each (unsigned int subAddr in SubAddresses)
   {
      if (!first) { szFullString += ","; }
      else { szFullString += "-"; }
      szFullString += to_string(subAddr);
      first = false;
   }

   return szFullString;
}

bool Address::operator==(const Address& rhs) const
{
   if (this->Main != rhs.Main) { return false; }

   vector<unsigned int> lstLHS(this->SubAddresses.begin(), this->SubAddresses.end());
   vector<unsigned int> lstRHS(rhs.SubAddresses.begin(), rhs.SubAddresses.end());
   for (size_t i = 0; i < lstLHS.size(); i++)
   {
      unsigned int iLHS = lstLHS.at(i);
      int iFound = ListHelper::List_Find(iLHS, lstRHS);
      if (-1 == iFound)
      {
         return false;
      }
      else
      {
         lstRHS.erase(lstRHS.begin() + iFound);
         lstLHS.erase(lstLHS.begin() + i);
      }
   }

   return lstLHS.size() == 0 && lstRHS.size() == 0;
}

bool Address::operator<(const Address& rhs) const
{
   return false;
}

// 1 is included for search algorithms.
const vector<int> 
Addresser::Primes({ 1,2,3,5,7,11,13,17,19,23,29,31,37,41 });
unsigned int 
Addresser::ms_iRandom=0;


Addresser::Addresser()
{
}


Addresser::~Addresser()
{
}

int Addresser::GetPrimeIndex(unsigned int aiComposite)
{
   size_t iPC = Primes.size();
   for (size_t i = 1; i < iPC; i++)
   {
      if ( aiComposite % Primes[i] == 0)
      {
         return i;
      }
   }

   return 0;
}

int Addresser::GetHighPrimeIndex(unsigned int aiComposite)
{
   size_t iPC = Primes.size();
   for (size_t i = iPC; i > 1; i--)
   {
      if (aiComposite % Primes[i-1] == 0)
      {
         return i-1;
      }
   }

   return 0;
}

int Addresser::GetRandom()
{
   ms_iRandom++;
   srand(time(0) - ms_iRandom);
   return rand();
}

bool 
Addresser::IsResidentIn(Address aAddress,
                        Address aTestAddress,
                        Address& rAddrIn)
{
   bool bIsResident = true;
   rAddrIn = Address(aAddress.Main);
   rAddrIn.SubAddresses.clear();

   if (aAddress.Main != aTestAddress.Main) { return false; }

   bool bFoundSubAddressMatch = false;
   for each (unsigned int iSubAddress in aAddress.SubAddresses)
   {
      // Since the subAddress is the MOST specific location,
      // it will be the LARGER than iSubAddress, if
      // iSubAddress contains any item in subAddress.
      bFoundSubAddressMatch |= isSuperSet(aTestAddress.SubAddresses[0], iSubAddress);
      if (bFoundSubAddressMatch) 
      {
         rAddrIn.SubAddresses.push_back(iSubAddress);
      } 
   }

   return bIsResident &= bFoundSubAddressMatch;
}

bool 
Addresser::DoesLocationContain( Address aLocation,
                                Address aAddress,
                                Address& rAddrIn )
{
   bool bIsResident = true;
   rAddrIn = Address(aAddress.Main);
   rAddrIn.SubAddresses.clear();

   if (aAddress.Main != aLocation.Main) { return false; }

   bool bFoundSubAddressMatch = false;
   for each (unsigned int iSubAddress in aAddress.SubAddresses)
   {
      // Since the subAddress is the MOST specific location,
      // it will be the LARGER than iSubAddress, if
      // iSubAddress contains any item in subAddress.
      bFoundSubAddressMatch |= isSuperSet(iSubAddress, aLocation.SubAddresses[0]);
      if (bFoundSubAddressMatch) 
      {
         rAddrIn.SubAddresses.push_back(iSubAddress);
      } 
   }

   return bIsResident &= bFoundSubAddressMatch;
}

bool 
Addresser::PitheLocation(Address& aAddress, Address& aPitheLocation)
{
   Addresser addr;
   bool bDidPithe = false;
   Address PithedAddress;

   if (IsResidentIn(aAddress, aPitheLocation, PithedAddress))
   {
      bDidPithe = true;
      for each (unsigned int iRemoveSubAddress in PithedAddress.SubAddresses)
      {
         int iSAChild = ListHelper::List_Find(iRemoveSubAddress,
                                              aAddress.SubAddresses);

         int iPrimeIndex = addr.GetHighPrimeIndex(iRemoveSubAddress);
         int iHighPrime = Addresser::Primes[iPrimeIndex];
         int iNewSubAddress = iRemoveSubAddress / iHighPrime;

         // This will only happen if they are == 1.
         if (iNewSubAddress == iRemoveSubAddress)
         {
            aAddress.SubAddresses.erase(aAddress.SubAddresses.begin() + iSAChild);
         }
         else
         {
            aAddress.SubAddresses[iSAChild] = iNewSubAddress;
         }

         if (aAddress.SubAddresses.size() == 0)
         {
            aAddress.Main = "";
            break;
         }
      }
   }
   return bDidPithe;
}

bool Addresser::InceptLocation(Address& aAddress, Address& aNewLocation)
{
   Addresser addr;
   bool bDidAdd = false;
   Address InceptAddress;

   if (DoesLocationContain(aNewLocation, aAddress, InceptAddress))
   {
      unsigned int iContainedInLocAddr = InceptAddress.SubAddresses[0];
      int iSAChild = ListHelper::List_Find(iContainedInLocAddr,
                                           aAddress.SubAddresses);

      aAddress.SubAddresses[iSAChild] = aNewLocation.SubAddresses[0];
      bDidAdd = true;
   }

   return bDidAdd;
}

bool Addresser::isSuperSet(unsigned int aiSuperSet, unsigned int aiSubSet)
{
   if (aiSubSet == aiSuperSet) { return true; }
   Addresser addr;

   // The subset will have a larger code, ie 30. The superset will be, e.g. 6.
   if (aiSubSet % aiSuperSet == 0)
   {
      int iSmallPrime = addr.GetPrimeIndex(aiSubSet / aiSuperSet);
      int iSuperLargePrime = addr.GetHighPrimeIndex(aiSuperSet);

      return iSmallPrime >= iSuperLargePrime;
   }
   else
   {
      return false;
   }
}