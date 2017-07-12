using StoreFrontPro.Views;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Support.MultiDisplay
{
    class VCIMultiDisplay : IViewComponentInterface
    {
        public const string Change = "Change";

        private Action m_ChangeRelay;

        public VCIMultiDisplay(Action Change)
        {
            m_ChangeRelay = Change;
        }

        public Type GetInterfaceType()
        {
            return typeof(MultiDisplay);
        }

        public bool TryInvoke(string Key, object[] args)
        {
            if (Key == Change)
            {
                m_ChangeRelay?.Invoke();
                return true;
            }
            return false;
        }
    }
}
