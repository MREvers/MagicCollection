#pragma once
#include<string>
#include<vector>

class Addresser;
class Address
{
public:
   std::string Main;
   std::vector<unsigned int> SubAddresses;

   Address();
   Address(std::string aszColAddress);
   Address(std::string aszColBase, unsigned int aiSubAddress);

   std::string GetFullAddress() const;
   bool IsEmpty() const;
   bool IsResidentIn( const Address& aLocation,
                      Address& rMatchChain = Address() ) const;

   bool operator==(const Address& rhs) const;
   bool operator<(const Address& rhs) const;
};

class Addresser
{
public:
   Addresser();
   ~Addresser();

   int GetPrimeIndex(unsigned int aiComposite);
   int GetPrime(unsigned int aiComposite);
   int GetHighPrimeIndex(unsigned int aiComposite);
   int GetHighPrime(unsigned int aiComposite);
   
   int GetRandom();

   bool DoesAddressIncludeLocation( const Address& aAddress,
                                    const Address& aTestLocation,
                                    Address& rAddrIn );

   bool PitheLocation(Address& aAddress, const Address& aPitheLocation);
   bool InceptLocation(Address& aAddress, const Address& aNewLocation);

   void AddSubAddress( Address& aAddress, const Address& aNewLocation );
   void AddSubAddress( Address& aAddress, unsigned int aNewSub );

public:
   static const std::vector<int> Primes;

private: 
   static unsigned int ms_iRandom;

   static bool isSuperSet( unsigned int aiSuperSet, 
                           unsigned int aiSubSet );

   static int compareSubAddress( unsigned int aiSOne,
                                 unsigned int aiSTwo );
};

