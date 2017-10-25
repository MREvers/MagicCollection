using StoreFrontPro.Views.Components.VCardImageDock;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.CardImageDock
{
   class VCICardImageDock: IViewComponentInterface
   {
      public const string SubmitChange = "Submit";

      // There are no arguments for these functions because the owner
      // of this component should already have the cardmodel
      private Func<object, Action> m_SubmitRelay;

      public VCICardImageDock(Func<object, Action> ServerChange)
      {
         m_SubmitRelay = ServerChange;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMCardImageDock);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if( Key == SubmitChange )
         {
            m_SubmitRelay?.Invoke(Caller).Invoke();
            return true;
         }

         return false;
      }
   
   }
}
