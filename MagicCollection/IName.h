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

      std::string inline GetName()
      {
         return m_szName;
      }

      static int inline FindInsertionPoint(std::string aszInsert, const std::vector<IName*>& aLstNames)
      {
         int iSize = aLstNames.size();
         int iIndex = 0;
         while (iIndex < iSize)
         {
            std::string szOther = aLstNames.at(iIndex)->GetName();
            if (szOther.compare(aszInsert) < 0)
            {
               iIndex++;
            }
            else
            {
               break;
            }
         }

         return iIndex;
      }

      static int inline FindInSortedList(std::string aszFindName, const std::vector<IName*>& aLstNames)
      {
         int iSize = aLstNames.size();
         int iLeft = 0;
         int iRight = iSize;
         if (iRight < 1)
         {
            return -1;
         }

         while (iLeft <= iRight)
         {
            int middle = (iLeft + iRight) / 2;

            if (middle < 0 || middle >= iSize)
            {
               return -1;
            }
            std::string szName = aLstNames.at(middle)->GetName();
            if (szName == aszFindName)
               return middle;
            else if (aszFindName.compare(szName) < 0)
               iRight = middle - 1;
            else
               iLeft = middle + 1;
         }

         return -1;
      }

   private:
      std::string m_szName;
};