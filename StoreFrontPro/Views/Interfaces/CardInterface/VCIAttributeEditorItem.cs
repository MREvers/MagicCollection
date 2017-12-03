using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CardInterface
{
   class VCIAttributeEditorItem : IViewComponentInterface
   {
      public const string SelectionChanged = "SelectionChanged";

      private Func<object, Action<string,string>> m_SelectionChangeRelay;

      public VCIAttributeEditorItem(Func<object, Action<string,string>> SelectionChanged)
      {
         m_SelectionChangeRelay = SelectionChanged;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMAttributeEditorItem);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if( Key == SelectionChanged )
         {
            if( args.Count() > 1 )
            {
               string szRoutingName = (string)args[0];
               string szChangeTo = (string)args[1];
               m_SelectionChangeRelay?.Invoke(Caller).Invoke(szRoutingName, szChangeTo);
               return true;
            }
         }

         return false;
      }
   }
}
