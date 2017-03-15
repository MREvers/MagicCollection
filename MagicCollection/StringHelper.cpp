#include "StringHelper.h"



StringHelper::StringHelper()
{
}


StringHelper::~StringHelper()
{
}

std::string StringHelper::Str_Trim(const std::string& src, char removeChar)
{
   size_t first = src.find_first_not_of(' ');
   if (std::string::npos == first)
   {
      return src;
   }
   size_t last = src.find_last_not_of(' ');
   return src.substr(first, (last - first + 1));
}

std::vector<std::string> StringHelper::Str_Split(std::string& aszSplit, std::string aszDelim)
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