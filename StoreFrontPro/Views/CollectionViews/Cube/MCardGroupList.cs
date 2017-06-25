using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class MCardGroupList
    {
        private List<CardModel> _ungroupedList;
        public List<CardModel> GroupedList {
            get
            {
                return getFilteredList();
            }
        }

        public string ExpectedAttribute;
        private string m_szFilteringAttribute;

        public MCardGroupList(string FilteredAttribute, string ExpectedAttributes, List<CardModel> BaseList)
        {
            _ungroupedList = BaseList;

            m_szFilteringAttribute = FilteredAttribute;
            ExpectedAttribute = ExpectedAttributes;
        }

        // Can eventually use a strategy pattern to decide how to filter. TODO
        private List<CardModel> getFilteredList()
        {
            return filterListOnIdentityAndCMC(_ungroupedList);
        }


        public List<CardModel> filterListOnIdentityAndCMC(List<CardModel> alstNew)
        {
            List<CardModel> lstRetVal = new List<CardModel>();

            foreach (CardModel oCardModel in alstNew)
            {
                string szKeyAttr = oCardModel.GetAttr(m_szFilteringAttribute);
                if (szKeyAttr.Contains(ExpectedAttribute))
                {
                    lstRetVal.Add(oCardModel);
                }
            }

            return lstRetVal;
        }

        private int CompareCMC(CardModel x, CardModel y)
        {
            int xCmc;
            int yCmc;
            string xszCmc = x.GetAttr("cmc");
            string yszCmc = y.GetAttr("cmc");

            bool xNum = int.TryParse(xszCmc, out xCmc);
            bool yNum = int.TryParse(yszCmc, out yCmc);

            if (xNum && yNum)
            {
                if (xCmc == yCmc)
                {
                    return 0;
                }
                else
                {
                    return (xCmc > yCmc) ? 1 : -1;
                }
            }
            else if (xNum && !yNum)
            {
                return 1;
            }
            else if (!xNum && yNum)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}
