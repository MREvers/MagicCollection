#include "SourceObject.h"


SourceObject::SourceObject(int aiCharBufOffset)
{
   m_iCachedIndex = -1;
   m_iCharBufferOffset = aiCharBufOffset;
   m_iKeyValArraySize = 0;
   m_pLstKeyVals = new unsigned short[9];
   m_iNameIndex = 0;
}

SourceObject::~SourceObject()
{
}

unsigned int SourceObject::AddAttribute(std::string key, std::string value, char* aplstCharBuf, unsigned int aiBufSize)
{
   unsigned short iKeyValPair = 0;
   unsigned short iSize = value.size();
   iKeyValPair = (iKeyValPair | iSize);

   for (int i = 0; i < 3; i++)
   {
      aplstCharBuf[aiBufSize + i] = key[i];
   }
   
   for (int i = 0; i < value.size(); i++)
   {
      aplstCharBuf[aiBufSize + i + 3] = value[i];
   }

   m_pLstKeyVals[m_iKeyValArraySize] = iKeyValPair;
   
   m_iKeyValArraySize++;

   return 3 + value.size();
}

unsigned int SourceObject::AddNonUniqueAttribute(std::string key, std::string value, char* aplstCharBuf, unsigned int aiBufSize)
{
   unsigned short iKeyValPair;
   iKeyValPair = 1 << 15;
   unsigned short iSize = value.size();

   iKeyValPair = iKeyValPair | iSize;

   for (int i = 0; i < 3; i++)
   {
      aplstCharBuf[aiBufSize + i] = key[i];
   }

   for (int i = 0; i < value.size(); i++)
   {
      aplstCharBuf[aiBufSize + i + 3] = value[i];
   }

   m_pLstKeyVals[m_iKeyValArraySize] = iKeyValPair;

   m_iKeyValArraySize++;

   return 3 + value.size();
}

std::string SourceObject::GetName(char* aiSearchBuffer)
{
   return GetAttribute("nam", aiSearchBuffer);
}

std::string SourceObject::GetAttribute(std::string aszKey, char* aiSearchBuffer)
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
std::vector<std::pair<std::string,std::string>> SourceObject::GetAttributes(char* aiSearchBuffer)
{
   std::vector<std::pair<std::string, std::string>> lstRetVal;

   unsigned short iLoopBufferOffset = 0;
   for (int i = 0; i < m_iKeyValArraySize; i++)
   {
      unsigned short iCurrentKVPair = m_pLstKeyVals[i];
      unsigned short iValueSize = extractSize(iCurrentKVPair);
      if (true)
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

         std::vector<std::string> lstNewlst = Str_Split(szValue, "::");

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
		newList[i] == m_pLstKeyVals[i];
	}
	delete[] m_pLstKeyVals;

	m_pLstKeyVals = newList;
}

std::vector<std::string> SourceObject::Str_Split(std::string aszSplit, std::string aszDelim)
{
   if (aszSplit.size() < aszDelim.size())
   {
      std::vector<std::string> lstSZs;
      lstSZs.push_back(aszSplit);
      return lstSZs;
   }
   else
   {
      int iDelimSize = aszDelim.size();
      int iSplitSize = aszSplit.size();
      std::vector<std::string> lstSZs;
      std::string szBefore = "";
      std::string szFocus = "";

      int i = 0;
      while (i < iSplitSize)
      {
         if (szFocus.size() >= iDelimSize)
         {
            szBefore += szFocus[0];

            for (int t = 0; t < iDelimSize - 1; t++)
            {
               szFocus[t] = szFocus[t + 1];
            }

            szFocus[aszDelim.size() - 1] = aszSplit[i];
         }

         if (szFocus.size() < iDelimSize)
         {
            szFocus += aszSplit[i];
         }


         if (szFocus == aszDelim)
         {
            lstSZs.push_back(szBefore);
            szBefore = "";
            szFocus = "";
         }

         // The Delimiter couldn't possibly be in the remaining chars because there aren't
         // enough. So Add the remaining chars.
         if (i + iDelimSize == iSplitSize)
         {
            szBefore += szFocus;

            for (int t = 1; t < iDelimSize; t++)
            {
               szBefore += aszSplit[i + t];
            }

            lstSZs.push_back(szBefore);
            break;
         }

         i++;
      }

      return lstSZs;
   }
}

int SourceObject::List_Find(std::string aszFind, std::vector<std::string> alstFindList)
{
   std::vector<std::string>::iterator iter_list = alstFindList.begin();
   int index = 0;
   for (; iter_list != alstFindList.end(); iter_list++)
   {
      if (*iter_list == aszFind)
      {
         return index;
      }
      index++;
   }
   return -1;
}

int SourceObject::List_Find(std::string aszFind, std::vector<std::pair<std::string, std::string>> alstFindList)
{
   std::vector<std::pair<std::string, std::string>>::iterator iter_list = alstFindList.begin();
   int index = 0;
   for (; iter_list != alstFindList.end(); iter_list++)
   {
      if (iter_list->first == aszFind)
      {
         return index;
      }
      index++;
   }
   return -1;
}

bool SourceObject::isNonUniqueFlag(short aiCheck)
{
   return aiCheck & (1 << 15);
}

unsigned short SourceObject::extractSize(short aiCheck)
{
   return (aiCheck & 0x7FFF);
}