using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionsOverview
{
    class VCICollectionsOverview : IViewComponentInterface
    {
        // Const strings for each event that is unhandled by the VM.
        public const string AddCollection = "AddCollection";
        public const string ViewCollection = "ViewCollection";
        public const string LoadCollection = "LoadCollection";

        private Action<CollectionModel> m_ViewCollectionRelay;
        private Action<string> m_AddCollectionRelay;
        private Action m_LoadCollection;

        public VCICollectionsOverview(Action<CollectionModel> ViewCollection, Action<string> AddCollection, Action LoadCollection)
        {
            m_ViewCollectionRelay = ViewCollection;
            m_AddCollectionRelay = AddCollection;
            m_LoadCollection = LoadCollection;
        }

        public Type GetInterfaceType()
        {
            return typeof(VMCollectionsOverview);
        }

        public bool TryInvoke(string Key, params object[] args)
        {
            if (Key == AddCollection)
            {
                if (args?[0] is string)
                {
                    string paramOne = args[0] as string;
                    m_AddCollectionRelay?.Invoke(paramOne);
                    return true;
                }
            }
            else if (Key == ViewCollection)
            {
                if (args?[0] is CollectionModel)
                {
                    CollectionModel paramOne = args[0] as CollectionModel;
                    m_ViewCollectionRelay?.Invoke(paramOne);
                    return true;
                }
            }
            else if (Key == LoadCollection)
            {
                m_LoadCollection?.Invoke();
                return true;
            }

            return false;
        }
    }
}
