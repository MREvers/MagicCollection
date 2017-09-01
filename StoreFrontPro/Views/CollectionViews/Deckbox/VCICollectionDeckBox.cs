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

      private Func<object, Action<CollectionModel>> m_SwitchToCubeRelay;

      public VCICollectionDeckBox(Func<object, Action<CollectionModel>> SwitchToCube)
      {
         m_SwitchToCubeRelay = SwitchToCube;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMCollectionDeckBox);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if (Key == SwitchToCube)
         {
            if (args?[0] is CollectionModel)
            {
               CollectionModel paramOne = args[0] as CollectionModel;
               m_SwitchToCubeRelay?.Invoke(Caller).Invoke(paramOne);
               return true;
            }
         }

         return false;
      }
   }
}
