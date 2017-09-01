using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   class InterfaceRouter
   {
      Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; }

      public InterfaceRouter()
      {
         SupportedInterface = new Dictionary<Type, IViewComponentInterface>();
      }

      public void AddInterface(IViewComponentInterface aInterface)
      {
         SupportedInterface.Add(aInterface.GetInterfaceType(),aInterface);
      }

      public void Call(Type aType, object aCaller, string Key, object[] args)
      {
         if (SupportedInterface.ContainsKey(aType))
         {
            SupportedInterface?[aType]?.TryInvoke(aCaller, Key, args);
         }
      }
   }

   interface IVCISupporter
   {
      void DisplayEventHandler(object source, DisplayEventArgs e);

      // It is suggested that the implementation of this involves
      // a singleton static instance.
      InterfaceRouter GetRouter();
   }
}
