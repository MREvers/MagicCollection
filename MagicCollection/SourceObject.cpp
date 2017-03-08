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

         // The Delimiter couldn't possibly be in the remaining chars because there aren't
         // enough. So Add the remaining chars.
         if (i + iDelimSize == iSplitSize)
         {
            szBefore += szFocus;

            for (int t = 1; t < iDelimSize; t++)
            {
               szBefore += aszSplit[i+t];
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

int SourceObject::List_Find(std::string aszFind, std::vector<std::pair<std::string,std::string>> alstFindList)
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