#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

class ListHelper
{
public:
   ListHelper();
   ~ListHelper();

 
   static std::vector<std::pair<std::string, std::string>>
   ConvertMapToList(const std::map<std::string, std::string>& aMap);

   static bool CompareKeyValPairList(const std::vector<std::pair<std::string, std::string>>& alstFirst,
                                     const std::vector<std::pair<std::string, std::string>>& alstSecond);

   template<typename T> static
   int List_Find(const T& aFind, const std::vector<T>& alstFindList)
   {
      std::vector<T>::const_iterator iter_list = alstFindList.cbegin();
      int index = 0;
      for (; iter_list != alstFindList.cend(); iter_list++)
      {
         if (*iter_list == aFind)
         {
            return index;
         }
         index++;
      }
      return -1;
   }

   template<class T, class R> static
   int List_Find( const T& aiFind, 
                  const std::vector<R>& alstFindList, 
                  const std::function<T(const R&)>& afuncExtractor )
   {
      std::vector<R>::const_iterator iter_list = alstFindList.cbegin();
      int index = 0;
      for (; iter_list != alstFindList.cend(); iter_list++)
      {
         T tRetval = afuncExtractor(*iter_list);
         if (aiFind == tRetval)
         {
            return index;
         }
         index++;
      }
      return -1;
   }

   template<class T> static
   void List_Insert( const T& aInsert, 
                     std::vector<T>& alstInsertList, 
                     const std::function<int(const T&,T&)>& afuncComparer )
   {
      std::vector<T>::iterator iter = alstInsertList.begin();
      for (; iter != alstInsertList.end(); ++iter)
      {
         int iCmp = afuncComparer(aInsert, *iter);
         if (iCmp >= 0) { break; }
      }

      alstInsertList.insert(iter, aInsert);
   }
};
