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
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
   class VMCardGroupList : ViewModel<MCardGroupList>
   {
      private const string cszCardImageDisplayerName = "CIDN";

      public string GroupName { get; set; }
      public ObservableCollection<ListViewItem> CategoryList { get; set; } =
         new ObservableCollection<ListViewItem>();

      private VCardImageDisplayer _ToolTipDisplay = null;
      public VCardImageDisplayer ToolTipDisplay
      {
         get { return _ToolTipDisplay; }
         set { _ToolTipDisplay = value; OnPropertyChanged(); }
      }

      public bool IsFullSize { get; set; } = false;

      public VMCardGroupList(MCardGroupList Model, string RoutingName, bool IsFullSize = false)
         : base(Model, RoutingName)
      {
         Model.Register(this);
         this.IsFullSize = IsFullSize;
         GroupName = Model.GroupName;

         ModelUpdated();
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
         CategoryList.ForEach(x => lstTemp.Add(x));

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

         if(!isToolTipDisplayed(szItemText))
         {
            CardModel model = getCardModel(szItemText);
            if (model != null)
            {
               ViewClass displayerClass = ViewFactory.CreateCardImageViewer(model, cszCardImageDisplayerName);
               ToolTipDisplay = (VCardImageDisplayer)displayerClass.View;
            }
         }
      }

      /// <summary>
      /// Returns whether a tooltip is displayed for the input item.
      /// </summary>
      /// <param name="aszItem"></param>
      /// <returns></returns>
      private bool isToolTipDisplayed(string aszItem)
      {
         VMCardImageDisplayer tooltip = (ToolTipDisplay?.DataContext as VMCardImageDisplayer);
         return ( ( tooltip != null ) &&                               // if Tooltip is already displayed
                  ( aszItem == tooltip.Model.GetIdealIdentifier() ) && // and Tooltip is displaying the same item
                  ( tooltip.CardImage != null ) );                     // and Tooltip is displaying an image.
      }

      private CardModel getCardModel(string aszItem)
      {
         return Model.GroupedList.Where(x => x.GetIdealIdentifier() == aszItem).FirstOrDefault();
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         Action actWrapper = () => { ModelUpdated(); };
         if (!Application.Current.Dispatcher.CheckAccess())
         {
            Application.Current.Dispatcher.BeginInvoke(actWrapper);
            return;
         }

         removeAllItems();
         Model.GroupedList.ForEach(x => addItemToCategoryList(x.GetIdealIdentifier()));
      }
      #endregion

      #region ViewModel
      public override void FreeModel()
      {
         base.FreeModel();

         // Since this model wraps another model, it is necessary to
         // also release the base model.
         Model.FreeModel();
      }
      #endregion
   }
}
