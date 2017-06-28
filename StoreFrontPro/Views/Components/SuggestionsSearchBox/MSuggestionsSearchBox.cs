using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
    class MSuggestionsSearchBox
    {
        public string ActionName;

        private IEnumerable<string> m_lstCollection = null;
        private Func<string, List<string>> m_fnSearchCollection = null;

        // IEnumerable stores a reference to the underlying data used to create the enumerable. THus
        // changes made to that list will affect this Enumerable.
        public MSuggestionsSearchBox(string ActionName, IEnumerable<string> Collection)
        {
            m_lstCollection = Collection;
            this.ActionName = ActionName;
        }

        public MSuggestionsSearchBox(string ActionName, Func<string, List<string>> SearchCollection)
        {
            m_fnSearchCollection = SearchCollection;
            this.ActionName = ActionName;
        }

        public List<string> GetMatchingCollectionItems(string aszMatch)
        {
            if (m_fnSearchCollection == null)
            {
                List<string> lstRetVal = new List<string>();
                List<string> lstHoldVals = new List<string>();
                foreach (string item in m_lstCollection)
                {
                    int iInd = item.ToLower().IndexOf(aszMatch);
                    if (iInd == 0)
                    {
                        lstRetVal.Add(item);
                    }
                    else if (iInd > 0)
                    {
                        lstHoldVals.Add(item);
                    }
                }

                lstRetVal = lstRetVal.Concat(lstHoldVals).ToList();
                return lstRetVal;
            }
            else
            {
                return m_fnSearchCollection(aszMatch);
            }
        }
    }
}
