using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using StoreFrontPro.Tools;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class MCardGroupList
    {
        private ObservableCollection<CardModel> _ungroupedList;
        public ObservableCollection<CardModel> GroupedList {
            get
            {
                return getFilteredList(_ungroupedList);
            }
        }

        public string GroupName;
        public List<string> ExpectedAttributes;
        private string m_szFilteringAttribute;
        private List<string> m_szAttributeSet;

        public MCardGroupList(string FilteredAttribute, string ExpectedAttribute, List<string> AttributeSet, ObservableCollection<CardModel> BaseList)
        {

            _ungroupedList = BaseList;

            m_szFilteringAttribute = FilteredAttribute;

            // Split the expected attributes on '/'
            ExpectedAttributes = ExpectedAttribute.Split('\\').ToList();

            m_szAttributeSet = AttributeSet;

            GroupName = ExpectedAttribute;
        }

        // Can eventually use a strategy pattern to decide how to filter. TODO
        private ObservableCollection<CardModel> getFilteredList(ObservableCollection<CardModel> alstList)
        {
            return filterListOnIdentityAndCMC(alstList);
        }


        public ObservableCollection<CardModel> filterListOnIdentityAndCMC(ObservableCollection<CardModel> alstNew)
        {
            ObservableCollection<CardModel> lstRetVal = new ObservableCollection<CardModel>();

            foreach (CardModel oCardModel in alstNew)
            {
                string szKeyAttr = oCardModel.GetAttr(m_szFilteringAttribute);

                bool isNotElement = false;
                foreach(string attr in m_szAttributeSet)
                {
                    isNotElement |= ExpectedAttributes.Contains(attr) != szKeyAttr.Contains(attr);
                }

                if (!isNotElement)
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
