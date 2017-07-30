#include "ListHelper.h"

ListHelper::ListHelper()
{
}

ListHelper::~ListHelper()
{
}

std::vector<std::pair<std::string, std::string>> ListHelper::ConvertMapToList(std::map<std::string, std::string>  aMap)
{
   std::vector<std::pair<std::string, std::string>> lstRetVal;
   std::map<std::string, std::string>::iterator iter_Map = aMap.begin();
   for (; iter_Map != aMap.end(); ++iter_Map)
   {
      lstRetVal.push_back(std::make_pair(iter_Map->first, iter_Map->second));
   }
   return lstRetVal;
}

bool ListHelper::CompareKeyValPairList(std::vector<std::pair<std::string, std::string>> alstFirst,
   std::vector<std::pair<std::string, std::string>> alstSecond)
{
   bool bMatch = true;

   if (bMatch = (alstFirst.size() == alstSecond.size()))
   {
      std::vector<std::pair<std::string, std::string>>::iterator iter_First = alstFirst.begin();
      std::vector<std::pair<std::string, std::string>>::iterator iter_Second = alstSecond.begin();

      for (; iter_First != alstFirst.end(); ++iter_First)
      {
         bool bFoundMatch = false;
         for (; iter_Second != alstSecond.end(); ++iter_Second)
         {
            if (iter_First->first == iter_Second->first)
            {
               if (iter_First->second == iter_Second->second)
               {
                  bFoundMatch = true;
                  break;
               }
            }
         }

         if (!bFoundMatch)
         {
            bMatch = false;
            break;
         }
      }
   }
   return bMatch;
}
