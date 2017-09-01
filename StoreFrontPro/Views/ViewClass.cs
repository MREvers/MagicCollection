using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro.Views
{
   class ViewClass
   {
      public IModel Model;
      public IViewModel ViewModel;
      public UserControl View;

      public ViewClass(IModel Model, IViewModel ViewModel, UserControl View)
      {
         this.Model = Model;
         this.ViewModel = ViewModel;
         this.View = View;
      }

      public bool HookDisplayEvent(DisplayEventHandler displayEventHandler)
      {
         if( ViewModel is IViewComponent )
         {
            IViewComponent VC = ViewModel as IViewComponent;
            VC.DisplayEvent += displayEventHandler;
            return true;
         }

         return false;
      }
   }
}
