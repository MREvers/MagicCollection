using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
   class VCISuggestionsSearchBox : IViewComponentInterface
   {
      public const string OK = "OK";

      private Func<object, Action<string>> m_OKRelay;

      public VCISuggestionsSearchBox(Func<object, Action<string>> OK)
      {
         m_OKRelay = OK;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMSuggestionsSearchBox);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if (Key == OK)
         {
            string szRoutingName = args?[0] as string ?? "";
            m_OKRelay?.Invoke(Caller).Invoke(szRoutingName);
            return true;
         }
         return false;
      }
   }
}
