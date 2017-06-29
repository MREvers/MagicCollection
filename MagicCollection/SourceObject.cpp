#include "SourceObject.h"


SourceObject::SourceObject(int aiCharBufOffset)
{
   m_iCachedIndex = -1;
   m_iCharBufferOffset = aiCharBufOffset;
   m_iKeyValArraySize = 0;
   m_pLstKeyVals = new unsigned short[11];
   m_iNameIndex = 0;
}

SourceObject::~SourceObject()
{
}

unsigned int 
SourceObject::AddAttribute(std::string aszkey, std::string value, char* aplstCharBuf, unsigned int aiBufSize)
{
	std::string key = aszkey;
	std::string szFixedKey = Config::Instance()->GetKeyCode(key);
	if (szFixedKey != "")
	{
		key = szFixedKey;
	}

   unsigned short iKeyValPair = 0;
   unsigned short iSize = value.size();

   if (Config::Instance()->IsIdentifyingAttributes(Config::Instance()->GetFullKey(key)))
   {
	   iKeyValPair = 1 << 15;
   }

   iKeyValPair = (iKeyValPair | iSize);

   for (unsigned int i = 0; i < 3; i++)
   {
      aplstCharBuf[aiBufSize + i] = key[i];
   }
   
   for (unsigned int i = 0; i < value.size(); i++)
   {
      aplstCharBuf[aiBufSize + i + 3] = value[i];
   }

   m_pLstKeyVals[m_iKeyValArraySize] = iKeyValPair;
   
   m_iKeyValArraySize++;

   return 3 + value.size();
}

std::string 
SourceObject::GetName(char* aiSearchBuffer)
{
   return GetAttribute("nam", aiSearchBuffer);
}

std::string 
SourceObject::GetAttribute(std::string aszKey, char* aiSearchBuffer)
{
   unsigned short iLoopBufferOffset = 0;
   for (int i = 0; i < m_iKeyValArraySize; i++)
   {
      unsigned short iCurrentKVPair = m_pLstKeyVals[i];
      unsigned short iValueSize = extractSize(iCurrentKVPair);
      std::string szKeyCode(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset, 3);
      if (szKeyCode == aszKey)
      {
         std::string szValue(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset + 3, iValueSize);
         return szValue;
      }
      else
      {
         iLoopBufferOffset += 3 + iValueSize;
      }
   }

   return "";
}

// Only returns unique attrs
std::vector<std::pair<std::string,std::string>> 
SourceObject::GetAttributes(char* aiSearchBuffer)
{
   std::vector<std::pair<std::string, std::string>> lstRetVal;

   unsigned short iLoopBufferOffset = 0;
   for (int i = 0; i < m_iKeyValArraySize; i++)
   {
      unsigned short iCurrentKVPair = m_pLstKeyVals[i];
      unsigned short iValueSize = extractSize(iCurrentKVPair);
      if (!isNonUniqueFlag(iCurrentKVPair))
      {
         std::string szKeyCode(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset, 3);
         std::string szValue(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset + 3, iValueSize);
         
         lstRetVal.push_back(std::make_pair(szKeyCode, szValue));
      }
      iLoopBufferOffset += 3 + iValueSize;
   }

   return lstRetVal;
}

std::map<std::string, std::vector<std::string>> SourceObject::GetNonUniqueAttributeRestrictions(char* aiSearchBuffer)
{
   std::map<std::string, std::vector<std::string>> mapNonUAttrs;

   unsigned short iLoopBufferOffset = 0;
   for (int i = 0; i < m_iKeyValArraySize; i++)
   {
      unsigned short iCurrentKVPair = m_pLstKeyVals[i];
      unsigned short iValueSize = extractSize(iCurrentKVPair);
      if (isNonUniqueFlag(iCurrentKVPair))
      {
         std::string szKeyCode(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset, 3);
         std::string szValue(aiSearchBuffer + m_iCharBufferOffset + iLoopBufferOffset + 3, iValueSize);

         std::vector<std::string> lstNewlst = StringHelper::Str_Split(szValue, "::");

         mapNonUAttrs[szKeyCode] = lstNewlst;
      }
      iLoopBufferOffset += 3 + iValueSize;
   }

   return mapNonUAttrs;
}

int SourceObject::GetCacheIndex()
{
   return m_iCachedIndex;
}

void SourceObject::Cache(unsigned short aiCacheIndex)
{
   m_iCachedIndex = aiCacheIndex;
}

void SourceObject::FinalizeSize()
{
	unsigned short* newList = new unsigned short[m_iKeyValArraySize];
	for (int i = 0; i < m_iKeyValArraySize; i++)
	{
		newList[i] = m_pLstKeyVals[i];
	}
	delete[] m_pLstKeyVals;

	m_pLstKeyVals = newList;
}

bool SourceObject::isNonUniqueFlag(short aiCheck)
{
   return (aiCheck & (1 << 15)) != 0;
}

unsigned short SourceObject::extractSize(short aiCheck)
{
   return (aiCheck & 0x7FFF);
}