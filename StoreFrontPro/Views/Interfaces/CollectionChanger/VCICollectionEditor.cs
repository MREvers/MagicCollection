using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class VCICollectionEditor : IViewComponentInterface
    {
        public const string Accept = "Accept";
        public const string Cancel = "Cancel";

        private Action m_AcceptRelay;
        private Action m_CancelRelay;

        public VCICollectionEditor(Action Accept, Action Cancel)
        {
            m_AcceptRelay = Accept;
            m_CancelRelay = Cancel;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMCollectionEditor);
        }

        public bool TryInvoke(string Key, object[] args)
        {
            if (Key == Accept)
            {
                m_AcceptRelay?.Invoke();
            }
            else
            {
                m_CancelRelay?.Invoke();
            }

            return true;
        }
    }
}
