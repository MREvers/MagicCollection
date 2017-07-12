using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
    class VCISuggestionsSearchBox : IViewComponentInterface
    {
        public const string OK = "OK";

        private Action m_OKRelay;

        public VCISuggestionsSearchBox(Action OK)
        {
            m_OKRelay = OK;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMSuggestionsSearchBox);
        }

        public bool TryInvoke(string Key, object[] args)
        {
            if (Key == OK)
            {
                m_OKRelay?.Invoke();
                return true;
            }
            return false;
        }
    }
}
