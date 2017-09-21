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

   Addresser addr;
   if (lstUIandPF.size() == 1)
   {
      addr.AddSubAddress(*this, 1);
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
            addr.AddSubAddress(*this, iSubAddress);
         }
      }
   }

   Main = lstUIandPF[0];
}

Address::Address(std::string aszColBase, unsigned int aiSubAddress)
{
   Addresser addr;
   Main = aszColBase;
   addr.AddSubAddress(*this, aiSubAddress);
}

string
Address::GetFullAddress() const
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

bool 
Address::IsEmpty() const
{
   return Main == "";
}

// Returns true if any of this' main-subAddress pairs
// reside withing aLocation.
// (A location is an Address with one subAddress)
bool 
Address::IsResidentIn( const Address& aLocation,
                       Address& rMatchChain ) const
{
   Addresser addr;
   return addr.DoesAddressIncludeLocation(*this, aLocation, rMatchChain);
}

bool 
Address::operator==(const Address& rhs) const
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

// Returns the number of the lowest value prime. The number
// of the prime is the index of the prime in the list of all primes.
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

int 
Addresser::GetPrime( unsigned int aiComposite )
{
   return Primes[GetPrimeIndex(aiComposite)];
}

// Returns the number of the highest value prime. The number
// of the prime is the index of the prime in the list of all primes.
int
Addresser::GetHighPrimeIndex(unsigned int aiComposite)
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

int Addresser::GetHighPrime( unsigned int aiComposite )
{
   return Primes[GetHighPrimeIndex(aiComposite)];
}

int Addresser::GetRandom()
{
   ms_iRandom+=rand();
   srand(time(0) - ms_iRandom);
   return rand();
}

// a.k.a. DoesAddressDesignateLocation
// Returns true if any of the the 'address-subaddress' pairs of aAddress
//  reside within aContainingLocation.
// Also Returns the address points that caused this location to be included.
// (A location is an address with only one subaddress)
// i.e. if aAddress = "Add-2,9,15" and aTestAddress = "Add-3"
//  then this returns true because Add-9 (and Add-15) is within Add-3.
// Returns the Address with each subaddress that matched; in the
//  given example, returns Add-9,15.
bool 
Addresser::DoesAddressIncludeLocation( const Address& aAddress,
                                       const Address& aContainingLocation,
                                       Address& rAddrIn )
{
   bool bIsResident = true;
   rAddrIn = Address(aAddress.Main);
   rAddrIn.SubAddresses.clear();

   if (aAddress.Main != aContainingLocation.Main) { return false; }

   bool bFoundSubAddressMatch = false;
   for each (unsigned int iSubAddress in aAddress.SubAddresses)
   {
      // Since the subAddress is the MOST specific location,
      // it will be the LARGER than iSubAddress, if
      // iSubAddress contains any item in subAddress.
      bool bIsSuper = isSuperSet(aContainingLocation.SubAddresses[0], iSubAddress);
      if( bIsSuper ) 
      {
         AddSubAddress(rAddrIn, iSubAddress);
      } 
      bFoundSubAddressMatch |= bIsSuper;
   }

   return bIsResident &= bFoundSubAddressMatch;
}

// A location is an address with only 1 subaddress.
bool 
Addresser::PitheLocation(Address& aAddress, const Address& aPitheLocation)
{
   Addresser addr;
   bool bDidPithe = false;
   Address PithedAddress;

   if (DoesAddressIncludeLocation(aAddress, aPitheLocation, PithedAddress))
   {
      bDidPithe = true;
      for each (unsigned int iRemoveSubAddress in PithedAddress.SubAddresses)
      {
         int iPithingAddress = aPitheLocation.SubAddresses[0];
         int iSAChild = ListHelper::List_Find(iRemoveSubAddress,
                                              aAddress.SubAddresses);

         // Pull out all primes greater than or equal to the highest power of the highest prime.
         // Pull out all primes greater than the highest power of the highest prime in iPithingAddress
         int iPithingPrime = addr.GetHighPrime(iPithingAddress);
         while( addr.GetHighPrime( iRemoveSubAddress ) > iPithingPrime )
         {
            iRemoveSubAddress /= addr.GetHighPrime(iRemoveSubAddress);
         }

         // Now we should only differ by factors of the pithingprime.
         while( iRemoveSubAddress > iPithingAddress )
         {
            iRemoveSubAddress /= iPithingPrime;
         }

         // Divide by the pithing prime to put the address at one above.
         iRemoveSubAddress /= iPithingPrime;

         bool bRemoveIfOne = ( ( 1 == iRemoveSubAddress ) &&
                               ( iPithingAddress == 1 || aAddress.SubAddresses.size() > 1 ) );
         bool bRemoveIfDuplicate = -1 != ListHelper::List_Find(iRemoveSubAddress, aAddress.SubAddresses);
         if ( ( bRemoveIfOne ) || ( bRemoveIfDuplicate ) )
         {
            aAddress.SubAddresses.erase(aAddress.SubAddresses.begin() + iSAChild);
         }
         else
         {
            aAddress.SubAddresses[iSAChild] = iRemoveSubAddress;
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

// Returns true if aNewLocation could be included in aAddress.
// e.g. If aAddress = "Add-3" and aNewLocation = "Add-6"
// the outcome would be "Add-3,6"
// e.g. aAddress = "Add-3" and aNewLocation  = "Add-9" the
// outcome would be "Add-9"
bool 
Addresser::InceptLocation(Address& aAddress, const Address& aNewLocation)
{
   Addresser addr;
   bool bDidAdd = false;
   Address InceptAddress;

   if( aAddress.Main == aNewLocation.Main )
   {
      int iNewAddress = aNewLocation.SubAddresses[0];
      for( int i = 0; i < aAddress.SubAddresses.size(); i++ )
      {
         unsigned int subAddress = aAddress.SubAddresses.at(i);
         if( isSuperSet( subAddress, iNewAddress ) )
         {
            // e.g. Include Add-18 in Add-6, get Add-18
            bDidAdd = true;
            aAddress.SubAddresses[i] = iNewAddress;
            break;
         }
         else if( isSuperSet( iNewAddress, subAddress ) )
         {
            // This location is already included in the address.
            // e.g. Include Add-6 in Add-18, get Add-18
            bDidAdd = true;
            break;
         }
      }

      // if we haven't added yet, add it here.
      if( !bDidAdd )
      {
         // e.g. Include Add-18 in Add-4, get Add-4, 18
         // These should really only ever 1 subaddress in the inceptaddress.
         AddSubAddress(aAddress, iNewAddress);
      }
      
      bDidAdd = true;
   }

   return bDidAdd;
}

// Returns true if
// 1. The values are equal.
// 2. Superset divides Subset AND the smallest remaining prime factor
//    is larger than the largest prime factor in the super set.
//    e.g. 6 (superset), and 150 (subset). 150 / 6 = 25 (Divides), min(5, 5) >= max(2, 3, 5, 5)
//
// The reason for the second requirement (the smallest remaining prime
//  factor is smaller than the largest prime factor) is due to the fact that divides
//  does not filter all non-parent branches. Take 6 and 12 for example.
//  2*3 and 2*2*3. They are not on the same branch. See circled Below.
// ===========================================
// 1---------2---------4---------8---------16
//  \-3-9-27  \-(2*3)   \-(4*3)
//     \-3*5     \-2*3*5
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

// Inserts items into a subaddress list, in a sorted manner.
void 
Addresser::AddSubAddress( Address& aAddress, const Address& aNewLocation )
{
   AddSubAddress(aAddress, aNewLocation.SubAddresses[0]);
}

void 
Addresser::AddSubAddress( Address& aAddress, unsigned int aNewSub )
{
   int iInsert = aNewSub;
   std::vector<unsigned int>::iterator iter = aAddress.SubAddresses.begin();
   for (; iter != aAddress.SubAddresses.end(); ++iter)
   {
      int iCmp = compareSubAddress(iInsert, *iter);
      if (iCmp <= 0) { break; }
   }
   
   aAddress.SubAddresses.insert(iter, iInsert);
}

int
Addresser::compareSubAddress( unsigned int aiSOne,
                              unsigned int aiSTwo )
{
   unsigned int sOne = aiSOne;
   unsigned int sTwo = aiSTwo;
   Addresser addr;
   bool bDone = false;
   while( !bDone )
   {
      int lowPrimeOne = addr.GetPrime(sOne);
      int lowPrimeTwo = addr.GetPrime(sTwo);
      if( lowPrimeOne < lowPrimeTwo )
      {
         return -1;
      }
      else if( lowPrimeTwo < lowPrimeOne )
      {
         return 1;
      }
      else
      {
         int iSmallestCount = 0;
         while( addr.GetPrime( sOne ) == lowPrimeOne && sOne != 1)
         {
            sOne /= lowPrimeOne;
            iSmallestCount++;
         }

         while( addr.GetPrime( sTwo ) == lowPrimeTwo && sTwo != 1 )
         {
            sTwo /= lowPrimeTwo;
            iSmallestCount--;
         }

         if( iSmallestCount > 0 )
         {
            return -1;
         }
         else if( iSmallestCount < 0 )
         {
            return 1;
         }
      }

      bDone = sOne == sTwo;
   }

   return 0;
}