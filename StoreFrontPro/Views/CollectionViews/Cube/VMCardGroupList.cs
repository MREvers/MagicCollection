using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.CardImageDisplayer;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using StoreFrontPro.Tools;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
   class VMCardGroupList : ViewModel<MCardGroupList>
   {
      private const string cszCardImageDisplayerName = "CIDN";

      public string GroupName { get; set; }
      public ObservableCollection<ListViewItem> CategoryList { get; set; } = new ObservableCollection<ListViewItem>() { };

      private VCardImageDisplayer _ToolTipDisplay = null;
      public VCardImageDisplayer ToolTipDisplay
      {
         get { return _ToolTipDisplay; }
         set { _ToolTipDisplay = value; OnPropertyChanged(); }
      }

      public bool IsFullSize { get; set; } = false;

      public VMCardGroupList(MCardGroupList Model, string RoutingName, bool IsFullSize = false) : base(Model, RoutingName)
      {
         GroupName = Model.GroupName;
         SyncWithModel();
         this.IsFullSize = IsFullSize;
      }

      public void SyncWithModel()
      {
         removeAllItems();
         Model.GroupedList.ForEach(x => addItemToCategoryList(x.GetIdealIdentifier()));
      }

      private void addItemToCategoryList(string aszItem)
      {
         ListViewItem newItem = new ListViewItem();
         newItem.Content = aszItem;
         newItem.MouseMove += displayToolTip;

         CategoryList.Add(newItem);
      }

      private void removeAllItems()
      {
         List<ListViewItem> lstTemp = new List<ListViewItem>();
         foreach (ListViewItem lvi in CategoryList)
         {
            lstTemp.Add(lvi);
         }

         lstTemp.ForEach(x => removeItemFromCategoryList(x));
      }

      private void removeItemFromCategoryList(ListViewItem aoItem)
      {
         aoItem.MouseMove -= displayToolTip;

         CategoryList.Remove(aoItem);
      }

      private void displayToolTip(object source, MouseEventArgs e)
      {
         ListViewItem item = (source as ListViewItem);
         string szItemText = item.Content.ToString();

         if ((ToolTipDisplay?.DataContext != null) &&
             (szItemText == (ToolTipDisplay.DataContext as VMCardImageDisplayer).Model.GetIdealIdentifier()) &&
             ((ToolTipDisplay.DataContext as VMCardImageDisplayer).CardImage != null))
         {
            return;
         }

         CardModel model = Model.GroupedList.Where(x => x.GetIdealIdentifier() == szItemText).FirstOrDefault();
         if (model != null)
         {
            VMCardImageDisplayer cardImageDisplayerVM = new VMCardImageDisplayer(model, cszCardImageDisplayerName);
            VCardImageDisplayer cardImageDisplayerV = new VCardImageDisplayer();
            cardImageDisplayerV.DataContext = cardImageDisplayerVM;

            ToolTipDisplay = cardImageDisplayerV;
         }
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
