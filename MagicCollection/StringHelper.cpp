#include "StringHelper.h"



StringHelper::StringHelper()
{
}


StringHelper::~StringHelper()
{
}

std::string StringHelper::Str_Trim(const std::string& src, char removeChar)
{
   size_t first = src.find_first_not_of(removeChar);
   if (std::string::npos == first)
   {
      return src;
   }
   size_t last = src.find_last_not_of(removeChar);
   return src.substr(first, (last - first + 1));
}

std::string StringHelper::Str_Replace(const std::string& srz, char removeChar, char replaceChar)
{
   std::string szRetVal = "";
   for (char c : srz)
   {
      if (c != ' ')
      {
         szRetVal += c;
      }
   }
   return szRetVal;
}

std::string StringHelper::Str_Clean(const std::string& src)
{
   std::string szRetVal = "";
   for (char c : src)
   {
      if (c == 145)
      {
         szRetVal += "ae";
      }
      else if (c == 146)
      {
         szRetVal += "AE";
      }
      else if (c == '\'' || c == '\"')
      {
         // leave out
      }
      else
      {
         szRetVal += c;
      }
   }

   return szRetVal;
}
 
std::vector<std::string> 
StringHelper::Str_CmdLineParse( const std::string& srz )
{
   std::vector<std::string>  lstRetval;
   int iFirstQuote = srz.find_first_of('\"');
   if( iFirstQuote == -1 )
   {
      auto lstSplit = Str_Split( srz, " " );
      for( auto parm : lstSplit )
      {
         lstRetval.push_back(parm);
      }
   }
   else
   {
      std::string szThis = srz.substr(0, iFirstQuote);
      if( iFirstQuote > 0 )
      {
         szThis = Str_Trim( szThis, ' ' );
         auto lstSplit = Str_Split( szThis, " " );
         for( auto parm : lstSplit )
         {
            if( parm.size() > 0 )
            {
               lstRetval.push_back(parm);
            }
         }
      }

      if( iFirstQuote + 1 < srz.size() )
      {
         std::string szNext = srz.substr(iFirstQuote+1);
         int iSecondQuote = szNext.find_first_of('\"');
         if( iSecondQuote == -1 )
         {
            iSecondQuote = szNext.size() - 1;
         }
         lstRetval.push_back( szNext.substr(0, iSecondQuote) );

         if( iSecondQuote != szNext.size() - 1 )
         {
            std::string szNextParse = szNext.substr(iSecondQuote+1);
            auto lstNextParse = Str_CmdLineParse(szNextParse);
            for( auto parm : lstNextParse )
            {
               lstRetval.push_back(parm);
            }
         }
      }
   }

   return lstRetval;
}

std::vector<std::string> 
StringHelper::Str_Split(const std::string& aszSplit, const std::string& aszDelim)
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

std::vector<std::string> 
StringHelper::SplitIntoLines(const std::string& aszString)
{
   std::vector<std::string> lstLines;
   std::string szLine = "";
   for (int i = 0; i < aszString.size(); i++)
   {

      if (aszString[i] == '\n')
      {
         lstLines.push_back(szLine);
         szLine = "";
      }
      else
      {
         szLine = szLine + aszString[i];
         if (i == aszString.size() - 1)
         {
            lstLines.push_back(szLine);
            szLine = "";
         }
      }

   }
   return lstLines;
}