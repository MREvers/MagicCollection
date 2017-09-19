#pragma once
#pragma message ("Starting SourceObject.h")

#include <map>
#include <vector>
#include <memory>

#include "StringHelper.h"
#include "Config.h"

class CollectionObject;

template< typename T >
struct array_deleter
{
  void operator ()( T const * p)
  { 
    delete[] p; 
  }
};

// This stores a list of ints, specifying key val pairs as indicated in the colsource
// This stores an array of ints (Like above) int*
// Short Cache index
// Short name index and array size
class SourceObject
{
public:
   SourceObject(int aiCharBufOffset);
   ~SourceObject();

   //std::vector<std::pair<std::string, std::string>> Attribs;
   unsigned int AddAttribute(std::string key,
      std::string value,
      char* aplstCharBuf,
      unsigned int aiBufSize);

   // Used in loading from source DB.
   std::string GetName(char* aiSearchBuffer);

   // Requires the 3 char keycode
   std::string GetAttribute(std::string aszKeyCode, char* aiSearchBuffer);
   std::vector<std::pair<std::string, std::string>> GetAttributes(char* aiSearchBuffer);

   std::map<std::string, std::vector<std::string>> GetNonUniqueAttributeRestrictions(char* aiSearchBuffer);
   int GetCacheIndex();
   void Cache(unsigned short aiCacheIndex);

private:
   unsigned short m_pLstKeyVals[11];
   unsigned int m_iCharBufferOffset;
   short m_iCachedIndex;
   unsigned char m_iNameIndex;
   unsigned char m_iKeyValArraySize;

   bool isNonUniqueFlag(short aiCheck);
   unsigned short extractSize(short aiCheck);

};

#pragma message ("Finish SourceObject.h")