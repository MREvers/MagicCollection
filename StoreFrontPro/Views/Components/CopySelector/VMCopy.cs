using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.CopySelector
{
   class VMCopy : ViewModel<BasicModel<string>>
   {
      #region Bindings
      private string _Specifier;
      public string Specifier
      {
         get { return _Specifier; }
         set { _Specifier = value; OnPropertyChanged(); }
      }

      private bool _Selected;
      public bool Selected
      {
         get { return _Selected; }
         set { _Selected = value; OnPropertyChanged(); }
      }
      #endregion

      #region Public Methods
      public VMCopy(BasicModel<string> Model, string RoutingName) 
         : base(Model, RoutingName)
      {
         Specifier = Model.Item;
         Selected = false;
      }

      public override void ModelUpdated()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
