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

        private Action<string> m_AcceptRelay;
        private Action m_CancelRelay;

        public VCIRequestText(Action<string> Accept, Action Cancel)
        {
            m_AcceptRelay = Accept;
            m_CancelRelay = Cancel;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMRequestText);
        }

        public bool TryInvoke(string Key, params object[] args)
        {
            if (Key == Accept)
            {
                if (args?[0] is string)
                {
                    string paramOne = args[0] as string;
                    m_AcceptRelay?.Invoke(paramOne);
                    return true;
                }
            }
            else if (Key == Cancel)
            {
                m_CancelRelay?.Invoke();
                return true;
            }

            return false;
        }
    }
}
