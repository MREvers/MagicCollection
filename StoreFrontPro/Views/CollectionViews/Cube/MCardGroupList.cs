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
            List<CardModel> lstRetVal = new List<CardModel>();

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

            ObservableCollection<CardModel> lstRealRetVal = new ObservableCollection<CardModel>();
            foreach (CardModel cm in lstRetVal.OrderBy(x => CMC(x)).ThenBy(x => x.GetIdealIdentifier())) { lstRealRetVal.Add(cm); }

            return lstRealRetVal;
        }

        private int CMC(CardModel x)
        {
            int iCMC;
            string szMC = x.GetAttr("manaCost");
            int iSpecStart = Math.Max(szMC.IndexOf('}'), 0);
            if (!int.TryParse(szMC.Substring(1, iSpecStart-1), out iCMC))
            {
                iCMC = 0;
            }

            int iNumSpecs = szMC.Count((c) => c == '{')-1;
            return iCMC + iNumSpecs;
        }
    }
}
