using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CardInterface
{
   class VMAttributeEditorItem : ViewModel<MAttributeEditorItem>, IViewComponent
   {
      #region Binding
      public string AttributeName
      {
         get { return Model.Name; }
         set { Model.Name = value; OnPropertyChanged(); }
      }

      public string Set
      {
         get { return Model.SelectedOption; }
         set { Model.SelectedOption = value; OnPropertyChanged(); }
      }

      public ObservableCollection<string> PossibleValues { get; set; }
         = new ObservableCollection<string>();
      #endregion

      public VMAttributeEditorItem(MAttributeEditorItem Model, string RoutingName) 
         : base(Model, RoutingName)
      {
         Model.Register(this);

         // Do this just to notify property changed.
         AttributeName = Model.Name;
         Set = Model.SelectedOption;
         PossibleValues = new ObservableCollection<string>(Model.Options);
      }
      
      public override void ModelUpdated()
      {
         DisplayEventArgs eventArgs = 
            new DisplayEventArgs(VCIAttributeEditorItem.SelectionChanged, RoutingName, Set);
         DisplayEvent?.Invoke(this, eventArgs);
      }

      #region IViewComponent
      public event DisplayEventHandler DisplayEvent;

      public List<StoreFrontMenuItem> GetMenuItems()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
