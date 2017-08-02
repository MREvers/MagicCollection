#pragma once
#include <string>
#include "Addresser.h"

class CollectionDetails
{
public:
   CollectionDetails();
   ~CollectionDetails();

   void SetName(std::string aszName);
   std::string GetName();

   void SetFileName(std::string aszFileName);
   std::string GetFileName();

   void SetTimeStamp(unsigned long aulTimeStamp);
   unsigned long GetTimeStamp();

   void SetChildrenCount(unsigned int auiChildCount);
   unsigned int GetChildrenCount();
   void IncrementChildCount();

   void SetAddress(const Address& aAddress);
   void AssignAddress(std::string aszStringAddress);
   Address* GetAddress();

private:
   std::string m_szName;
   std::string m_szFileName;
   unsigned long m_ulTimeStamp;
   unsigned int m_iChildrenCount;
   Address* m_ptrAddress;
};

