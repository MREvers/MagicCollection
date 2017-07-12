using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
    class VCICollectionDeckBox : IViewComponentInterface
    {
        public const string SwitchToCube = "SwitchToCube";

        private Action<CollectionModel> m_SwitchToCubeRelay;

        public VCICollectionDeckBox(Action<CollectionModel> SwitchToCube)
        {
            m_SwitchToCubeRelay = SwitchToCube;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMCollectionDeckBox);
        }

        public bool TryInvoke(string Key, object[] args)
        {
            if (Key == SwitchToCube)
            {
                if (args?[0] is CollectionModel)
                {
                    CollectionModel paramOne = args[0] as CollectionModel;
                    m_SwitchToCubeRelay?.Invoke(paramOne);
                    return true;
                }
            }

            return false;
        }
    }
}
