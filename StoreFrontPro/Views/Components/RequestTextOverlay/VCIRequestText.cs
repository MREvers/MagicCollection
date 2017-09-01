using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.RequestTextOverlay
{
    class VCIRequestText : IViewComponentInterface
    {
        public const string Accept = "Accept";
        public const string Cancel = "Cancel";

        private Func<object, Action<string>> m_AcceptRelay;
        private Func<object, Action> m_CancelRelay;

        public VCIRequestText(Func<object,Action<string>> Accept, Func<object,Action> Cancel)
        {
            m_AcceptRelay = Accept;
            m_CancelRelay = Cancel;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMRequestText);
        }

        public bool TryInvoke(object Caller, string Key, params object[] args)
        {
            if (Key == Accept)
            {
                if (args?[0] is string)
                {
                    string paramOne = args[0] as string;
                    m_AcceptRelay?.Invoke(Caller).Invoke(paramOne);
                    return true;
                }
            }
            else if (Key == Cancel)
            {
                m_CancelRelay?.Invoke(Caller).Invoke();
                return true;
            }

            return false;
        }
    }
}
