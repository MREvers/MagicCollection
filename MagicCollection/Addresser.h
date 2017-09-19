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

   bool IsResidentIn( const Address& aAddress,
                      const Address& aTestLocation,
                      Address& rAddrIn );
   bool DoesLocationContain( const Address& aLocation,
                             const Address& aAddress,
                             Address& rAddrIn );

   bool PitheLocation(Address& aAddress, const Address& aPitheLocation);
   bool InceptLocation(Address& aAddress, const Address& aNewLocation);
private:
   bool isSuperSet( unsigned int aiSuperSet, 
                    unsigned int aiSubSet );

public:
   static const std::vector<int> Primes;

private: 
   static unsigned int ms_iRandom;
};

