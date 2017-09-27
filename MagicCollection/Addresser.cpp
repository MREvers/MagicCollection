#include "Addresser.h"

#include <time.h>

#include "StringHelper.h"
#include "ListHelper.h"

using namespace std;

// 1 is included for search algorithms.
const vector<int> 
Addresser::Primes({ 1,2,3,5,7,11,13,17,19,23,29,31,37,41 });
unsigned int
Addresser::ms_iRandom = 0;

Addresser::Addresser()
{

}

Addresser::~Addresser()
{

}

int 
Addresser::GetPrime( unsigned int aiPrimeIndex ) const
{
   return Primes.at(aiPrimeIndex);
}

int 
Addresser::GetLowPrimeIndex( unsigned int aiComposite ) const
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
Addresser::GetLowPrime( unsigned int aiComposite ) const
{
   return Primes[GetLowPrimeIndex(aiComposite)];
}

int 
Addresser::GetHighPrimeIndex( unsigned int aiComposite ) const
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

int 
Addresser::GetHighPrime( unsigned int aiComposite ) const
{
   return Primes[GetHighPrimeIndex(aiComposite)];
}
   
int 
Addresser::GetRandom()
{
   ms_iRandom+=rand();
   srand(time(0) - ms_iRandom);
   return rand();
}

Identifier::Identifier()
{

}

Identifier::Identifier(const string& aszId)
{
   parseIdName(aszId);
}

Identifier::~Identifier()
{

}

vector<unsigned int> 
Identifier::GetAddresses() const
{

}

string 
Identifier::GetMain() const
{

}

string 
Identifier::GetFullAddress() const
{

}

Address 
Identifier::ToAddress() const
{
   return Address(GetFullAddress());
}
   
bool 
Identifier::operator==( const Identifier& rhs ) const
{

}

bool 
Identifier::operator<( const Identifier& rhs ) const
{

}

void 
Identifier::parseIdName( const string& aszID )
{
   string szSubAddress; 
   unsigned int iNumChars; 
   unsigned int iSubAddress;
   vector<string> lstUIandPF;
   vector<string> lstSubAddresses;

   // Record the family name.
   lstUIandPF = StringHelper::Str_Split(aszID, string("-"));
   m_szMain = lstUIandPF[0];

   // Add the parsed subAddresses
   m_veciSubAddresses.push_back(1);
   lstSubAddresses = StringHelper::Str_Split(lstUIandPF[1], string(","));
   size_t iSAC = lstSubAddresses.size();
   for (size_t i = 0; i < iSAC; i++)
   {
      szSubAddress = lstSubAddresses[i];
      iSubAddress = stoi(szSubAddress, &iNumChars);
      if (iNumChars > 0)
      {
         addSubAddress(m_veciSubAddresses, iSubAddress);
      }
   }
}

// Returns
// -1 := Did not add b/c it is already specified.
// 0 := Failed to Add
// 1 := Replaced Exisiting aisa
// 2 := Inserted ordered
int 
Identifier::addSubAddress(vector<unsigned int> avecSAs, unsigned int aiSA)
{
   int iAdded = 0;

   // Replace a less specific set with this more
   // general set.
   // or do nothing because this SA is less specific.
   for( int i = 0; i < avecSAs.size(); i++ )
   {
      int SA = avecSAs[i];
      if( isSuperSet( SA, aiSA ) )
      {
         avecSAs[i] = aiSA;
         iAdded = 1;
         break;
      }

      if( isSuperSet( aiSA, SA ) )
      {
         iAdded = -1;
         break;
      }
   }
   
   // Add the sub address in an ordered manner.
   if( iAdded == 0 )
   {
      auto iter = avecSAs.begin();
      for(; iter != avecSAs.end(); ++iter)
      {
         int iCmp = compareSubAddress(aiSA, *iter);
         if (iCmp <= 0) { break; }
      }
   
      if( *iter != aiSA )
      {
         avecSAs.insert(iter, aiSA);
      }

      iAdded == 2;
   }

   return iAdded;
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
bool
Identifier::isSuperSet(unsigned int aiSuperSet, unsigned int aiSubSet) const
{
   if (aiSubSet == aiSuperSet) { return true; }
   Addresser addresser;

   // The subset will have a larger code, ie 30. The superset will be, e.g. 6.
   if (aiSubSet % aiSuperSet == 0)
   {
      int iSmallPrime = addresser.GetLowPrimeIndex(aiSubSet / aiSuperSet);
      int iSuperLargePrime = addresser.GetHighPrimeIndex(aiSuperSet);

      return iSmallPrime >= iSuperLargePrime;
   }
   else
   {
      return false;
   }
}

int 
Identifier::compareSubAddress( unsigned int aiSOne,
                               unsigned int aiSTwo ) const
{
   Addresser addr;
   unsigned int sOne = aiSOne;
   unsigned int sTwo = aiSTwo;
   bool bDone = false;
   while( !bDone )
   {
      int lowPrimeOne = addr.GetLowPrime(sOne);
      int lowPrimeTwo = addr.GetLowPrime(sTwo);
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
         while( addr.GetLowPrime( sOne ) == lowPrimeOne && sOne != 1)
         {
            sOne /= lowPrimeOne;
            iSmallestCount++;
         }

         while( addr.GetLowPrime( sTwo ) == lowPrimeTwo && sTwo != 1 )
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

Address::Address()
{

}

Address::Address(const string& aszId) : Identifier(aszId)
{

}

Address::~Address()
{

}

bool 
Address::IsEmpty() const
{
   return m_veciSubAddresses.size()==0;
}

vector<unsigned int> 
Address::GetAddresses() const
{
   return m_veciSubAddresses;
}

bool 
Address::AddSubAddress( unsigned int aiSub )
{
   return addSubAddress(m_veciSubAddresses, aiSub) != 0;
}

int 
Address::RemoveSubAddress( unsigned int aiSub )
{
   return SetSubAddress(aiSub, 1);
}

// Inputting aiSub = 1 will remove that item.
// Returns
// 0 := Item doesn't exist
// 1 := Perfect Replace
// 2 := Replaced another and removed old.
// 3 := Another already specifies, removed old.
// 4 := Error
// 5 := Removed
int 
Address::SetSubAddress(unsigned int aiAlreadySub, unsigned int aiSub)
{
   int iResult = 0;
   int iFound = ListHelper::List_Find(aiSub, m_veciSubAddresses);
  
   if( iFound != -1 )
   {
      vector<unsigned int> vecAddTest(m_veciSubAddresses);
      vecAddTest.erase(vecAddTest.begin() + iFound);

      // Now we want the subaddress to be added new
      int iAdded = addSubAddress(vecAddTest, aiSub);
      if( iAdded == 2 )
      {
         // aiSub is specified by no other SA.
         m_veciSubAddresses[iFound] = aiSub;
         iResult = 1;
      }
      else if( iAdded == 1 )
      {
         // aiSub more specific than another SA.
         // (i.e. specifies MORE sets)
         m_veciSubAddresses.erase(m_veciSubAddresses.begin() + iFound);
         addSubAddress(m_veciSubAddresses, aiSub);
         iResult = 2;
      }
      else if( iAdded == -1 )
      {
         // aiSub is less specific
         // (i.e. specifies fewer sets)
         m_veciSubAddresses.erase(m_veciSubAddresses.begin() + iFound);
         iResult = 3;
      }

      iResult = 4;
   }

   return iResult;
}

bool 
Address::MergeIdentifier( const Identifier& aID )
{
   if( GetMain() != aID.GetMain() ){ return false; }

   for( auto iSub : aID.GetAddresses() )
   {
      AddSubAddress(iSub);
   }

   return true;
}

bool 
Address::ExtractIdentifier( const Identifier& aID )
{
   if( GetMain() != aID.GetMain() ){ return false; }

   for( auto iSub : aID.GetAddresses() )
   {
      RemoveSubAddress(iSub);
   }

   return true;
}

Location::Location()
{

}

Location::Location(const string& aszId) : Identifier(aszId)
{
   m_iAddress = m_veciSubAddresses.at(0);
}

Location::Location( const string& aszMain, unsigned int aiSA )
{
   m_iAddress = aiSA;
   m_szMain = aszMain;
}

Location::~Location()
{

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
Location::IsResidentIn( const Address& aAddress ) const
{
   Address rAddrIn(aAddress.GetMain());

   if (aAddress.GetMain() != GetMain()) { return false; }

   bool bFoundSubAddressMatch = false;
   auto vecSAs = aAddress.GetAddresses();
   for( auto iSub : vecSAs )
   {
      bool bIsSuper = isSuperSet(GetAddress(), iSub);
      if( bIsSuper ) 
      {
         rAddrIn.AddSubAddress(iSub);
      } 
      bFoundSubAddressMatch |= bIsSuper;
   }

   return bFoundSubAddressMatch;
}

vector<unsigned int> 
Location::GetAddresses() const
{
   return vector<unsigned int>(1, m_iAddress);
}

unsigned int 
Location::GetAddress() const
{
   return m_iAddress;
}

Address 
Location::ToAddress() const
{
   return Address(GetFullAddress());
}