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

   std::string GetFullAddress();

   bool operator==(const Address& rhs) const;
   bool operator<(const Address& rhs) const;
};

class Addresser
{
public:
   Addresser();
   ~Addresser();

   int GetPrimeIndex(unsigned int aiComposite);
   int GetHighPrimeIndex(unsigned int aiComposite);
   int GetRandom();

   bool IsResidentIn( Address aAddress,
                      Address aTestLocation,
                      Address& rAddrIn );
   bool DoesLocationContain( Address aLocation,
                             Address aAddress,
                             Address& rAddrIn );

   bool PitheLocation(Address& aAddress, Address& aPitheLocation);
   bool InceptLocation(Address& aAddress, Address& aNewLocation);

   static const std::vector<int> Primes;
private:
   static unsigned int ms_iRandom;

   bool isSuperSet( unsigned int aiSuperSet, 
                    unsigned int aiSubSet );
};

