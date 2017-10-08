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
   unsigned int AddAttribute(
      const std::string& key,
      const std::string& value,
      char* aplstCharBuf,
      unsigned int aiMaxBuffSize);

   // Used in loading from source DB.
   std::string GetName(char* aiSearchBuffer);

   // Requires the 3 char keycode
   std::string GetAttribute(std::string aszKeyCode, char* aiSearchBuffer);
   std::vector<std::pair<std::string, std::string>> GetAttributes(char* aiSearchBuffer);

   std::map<std::string, std::vector<std::string>> GetNonUniqueAttributeRestrictions(char* aiSearchBuffer);

   unsigned int GetBufferOffset();
   void SetBufferOffset(unsigned int aiOffset);
   unsigned short GetCacheIndex();
   void SetCacheIndex(unsigned short aiIndex);

private:
   static const unsigned char ms_iNumKeys = 11;
   unsigned short m_pLstKeyVals[ms_iNumKeys];

   // Stores the buffer offset in the first 23 bits and the
   // cache index in the last 14. 40 bits total. 2 unused.
   unsigned char m_pOffAndInd[5];

   void setSize( unsigned short& riSize,
                 unsigned short aiValue ) const;
   void setFieldCode( unsigned short& riFC,
                      unsigned short aiValue ) const;
   unsigned short extractSize(unsigned short aiCheck) const;
   unsigned char extractFieldCode(unsigned short aiCheck) const;

   unsigned int insertAttribute( const std::string& value,
                                 char* aplstCharBuf, 
                                 unsigned int aiMaxBuffSize,
                                 char acLocation );

   unsigned int getObjectSize(int aiTruncate = -1) const;
   unsigned int getKeyListEnd() const;

   // Assign numerical values to each possible key. And store
   // it in a short with the field length
   // (4 bits for the code 12 bits for the length for each keyval)
   // If the values are all greater than 0, then we can
   // initialize the keyval array to all 0, then we know
   // that slot is not used. This allows for 15 possible keys. And
   // lengths up to 4096. Enumerate the key vals in the config.
   // Eliminates keyvalarraysize; saves 8 bits (from the array size)
   // and an additional 24 bits per keyval.
   //
   // Always put name as the first value. (Shift others if needed)
   // Elmininates nameindex; saves 8 bits
   //
   // Need 38 bits for cache index and buffer offset. Buffer offset is 23 bits
   // cache index is 15 bits.
   // Use an array of 5 chars to create 40 bits. First 23 bits are buffer offset,
   // last 15 are char index.
   // Saves 8 Bits
   //
   // Eliminate the nonunique flag attribute, rely on config for that.
};

#pragma message ("Finish SourceObject.h")