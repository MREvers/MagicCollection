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

        private Func<object, Action> m_ChangeRelay;

        public VCIMultiDisplay(Func<object,Action> Change)
        {
            m_ChangeRelay = Change;
        }

        public Type GetInterfaceType()
        {
            return typeof(MultiDisplay);
        }

        public bool TryInvoke(object Caller, string Key, object[] args)
        {
            if (Key == Change)
            {
                m_ChangeRelay?.Invoke(Caller).Invoke();
                return true;
            }
            return false;
        }
    }
}
