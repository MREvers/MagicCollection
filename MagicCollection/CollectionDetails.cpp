#include "CollectionDetails.h"



CollectionDetails::CollectionDetails()
{
   m_ptrAddress = new Address();
}


CollectionDetails::~CollectionDetails()
{
   delete m_ptrAddress;
}

void 
CollectionDetails::SetName(std::string aszName)
{
   m_szName = aszName;
}

std::string 
CollectionDetails::GetName()
{
   return m_szName;
}

void 
CollectionDetails::SetFileName(std::string aszFileName)
{
   m_szFileName = aszFileName;
}

std::string 
CollectionDetails::GetFileName()
{
   return m_szFileName;
}

void 
CollectionDetails::SetTimeStamp(unsigned long aulTimeStamp)
{
   m_ulTimeStamp = aulTimeStamp;
}

unsigned long 
CollectionDetails::GetTimeStamp()
{
   return m_ulTimeStamp;
}

void 
CollectionDetails::SetChildrenCount(unsigned int auiChildCount)
{
   m_iChildrenCount = auiChildCount;
}

unsigned int 
CollectionDetails::GetChildrenCount()
{
   return m_iChildrenCount;
}

void 
CollectionDetails::IncrementChildCount()
{
   m_iChildrenCount++;
}

void 
CollectionDetails::SetAddress(const Address& aAddress)
{
   delete m_ptrAddress;
   m_ptrAddress = new Address(aAddress);
}

void 
CollectionDetails::AssignAddress(std::string aszStringAddress)
{
   delete m_ptrAddress;
   m_ptrAddress = new Address(aszStringAddress);
}

Address* 
CollectionDetails::GetAddress()
{
   return m_ptrAddress;
}