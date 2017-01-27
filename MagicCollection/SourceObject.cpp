#include "SourceObject.h"


SourceObject::SourceObject(std::string aszName)
{
   AddAttribute("name", aszName);
   m_iCachedIndex = -1;
   m_szName = aszName;
}

SourceObject::~SourceObject()
{
}

bool SourceObject::AddAttribute(std::string key, std::string value)
{
   Attributes.insert(std::make_pair(key, value));
   return false;
}

bool SourceObject::AddNonUniqueAttribute(std::string aszKey, std::string aszVal)
{
   if (NonUniqueAttributes.find(aszKey) != NonUniqueAttributes.end())
   {
      NonUniqueAttributes[aszKey].push_back(aszVal);
   }
   else
   {
      std::vector<std::string> lstNewlst = Str_Split(aszVal, "::");
      NonUniqueAttributes[aszKey] = lstNewlst;
   }
   return false;
}

std::string SourceObject::GetName()
{
   return m_szName;
}

int SourceObject::GetCacheIndex()
{
   return m_iCachedIndex;
}

void SourceObject::Cache(int aiCacheIndex)
{
   m_iCachedIndex = aiCacheIndex;
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
      int iDelimSize =  aszDelim.size();
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

         if (i + iDelimSize == iSplitSize)
         {
            szBefore += szFocus;

            lstSZs.push_back(szBefore);
            break;
         }

         i++;
      }

      return lstSZs;
   }
}