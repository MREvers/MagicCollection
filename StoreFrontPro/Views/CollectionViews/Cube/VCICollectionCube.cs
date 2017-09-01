using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VCICollectionCube : IViewComponentInterface
    {
        public const string SwitchToDeckBox = "SwitchToDeckBox";

        private Func<object, Action<CollectionModel>> m_SwitchToDeckBoxRelay;

        public VCICollectionCube(Func<object,Action<CollectionModel>> SwitchToDeckBox)
        {
            m_SwitchToDeckBoxRelay = SwitchToDeckBox;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMCollectionCube);
        }

        public bool TryInvoke(object Caller, string Key, object[] args)
        {
            if (Key == SwitchToDeckBox)
            {
                if (args?[0] is CollectionModel)
                {
                    CollectionModel paramOne = args[0] as CollectionModel;
                    m_SwitchToDeckBoxRelay?.Invoke(Caller).Invoke(paramOne);
                    return true;
                }
            }

            return false;
        }
    }
}
