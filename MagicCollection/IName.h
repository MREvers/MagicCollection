#pragma once
#include <string>
#include <vector>
class IName
{
   public:
      IName(std::string aszName)
      {
         m_szName = aszName;
      }

      virtual ~IName(){};

      std::string GetName()
      {
         return m_szName;
      }

      static int inline FindInsertionPoint(std::string aszInsert, const std::vector<IName*>& aLstNames)
      {
         int iIndex = 0;
         std::vector<IName*>::const_iterator iter = aLstNames.begin();
         while (iIndex < aLstNames.size())
         {
            if (aszInsert.compare((*iter)->GetName()) > 0)
            {
               iIndex++;
            }
            else
            {
               return iIndex;
            }
         }

         return 0;
      }

      static int inline FindInSortedList(std::string aszFindName, const std::vector<IName*>& aLstNames)
      {

         int iLeft = 0;
         int iRight = aLstNames.size();
         if (iRight < 1)
         {
            return -1;
         }

         while (iLeft <= iRight)
         {
            int middle = (iLeft + iRight) / 2;

            if (middle < 0 || middle >= aLstNames.size())
            {
               return -1;
            }

            if (aLstNames.at(middle)->GetName() == aszFindName)
               return middle;
            else if (aszFindName.compare(aLstNames.at(middle)->GetName()) < 0)
               iRight = middle - 1;
            else
               iLeft = middle + 1;
         }

         return -1;
      }

   private:
      std::string m_szName;
};