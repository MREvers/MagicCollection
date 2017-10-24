using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CardInterface
{
   class VCICardChanger: IViewComponentInterface
   {
      public const string PreviewChange = "Preview";
      public const string SubmitChange = "Submit";

      // There are no arguments for these functions because the owner
      // of this component should already have the cardmodel
      private Func<object, Action> m_PreviewRelay;
      private Func<object, Action> m_SubmitRelay;

      public VCICardChanger(Func<object, Action> Preview, Func<object, Action> Submit)
      {
         m_PreviewRelay = Preview;
         m_SubmitRelay = Submit;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMCardChanger);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if( Key == PreviewChange )
         {
            m_PreviewRelay?.Invoke(Caller).Invoke();
            return true;
         }
         else if( Key == SubmitChange )
         {
            m_SubmitRelay?.Invoke(Caller).Invoke();
            return true;
         }

         return false;
      }
   
   }
}
