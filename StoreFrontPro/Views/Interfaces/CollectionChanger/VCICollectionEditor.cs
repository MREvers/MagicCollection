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

      private Func<object, Action> m_AcceptRelay;
      private Func<object, Action> m_CancelRelay;

      public VCICollectionEditor(Func<object, Action> Accept, Func<object, Action> Cancel)
      {
         m_AcceptRelay = Accept;
         m_CancelRelay = Cancel;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMCollectionEditor);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if (Key == Accept)
         {
            m_AcceptRelay?.Invoke(Caller).Invoke();
         }
         else
         {
            m_CancelRelay?.Invoke(Caller).Invoke();
         }

         return true;
      }
   }
}
