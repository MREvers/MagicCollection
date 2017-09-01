using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
   class VMSuggestionsSearchBox : ViewModel<MSuggestionsSearchBox>, IViewComponent
   {
      #region Data Binding
      public ObservableCollection<string> ComboBoxList { get; set; } = new ObservableCollection<string>();

      private string _ActionName;
      public string ActionName
      {
         get { return _ActionName; }
         set { _ActionName = value; OnPropertyChanged(); }
      }

      private string _TextBoxValue;
      public string TextBoxValue
      {
         get { return _TextBoxValue; }
         set { _TextBoxValue = value; OnPropertyChanged(); }
      }

      private int _SelectedIndex;
      public int SelectedIndex
      {
         get { return _SelectedIndex; }
         set { _SelectedIndex = value; OnPropertyChanged(); }
      }

      private bool _ComboBoxDropDown = false;
      public bool ComboBoxDropDown
      {
         get { return _ComboBoxDropDown; }
         set { _ComboBoxDropDown = value; OnPropertyChanged(); }
      }

      private bool _ComboBoxIsEditable = true;
      public bool ComboBoxIsEditable
      {
         get { return _ComboBoxIsEditable; }
         set { _ComboBoxIsEditable = value; OnPropertyChanged(); }
      }

      public RelayCommand OKCommand { get; set; }
      #endregion

      public event DisplayEventHandler DisplayEvent;

      public VMSuggestionsSearchBox(MSuggestionsSearchBox Model, string RoutingName) : base(Model, RoutingName)
      {
         OKCommand = new RelayCommand(eOKCommand);
         PropertyChanged += propertyChangedEventHandler;

         Model.Register(this);
         Model.NotifyViewModel();
      }

      public void UpdateSearchBoxAndList(string aszSearch)
      {
         // These need to be out here because otherwise, when the down arrow is used to select from the dropdown,
         // entries get duplicated. I don't know why.
         PropertyChanged -= propertyChangedEventHandler;

         if (aszSearch.Length > 3)
         {
            ComboBoxIsEditable = false;
            ComboBoxDropDown = true;

            // This line is needed. When the comboboxlist is cleared, it causes problems with the text in the textbox.
            TextBoxValue = aszSearch;

            // If there combobox contains the text value exactly, then the user is using the up/down keys to select.
            // This is because the combobox default behavior is to populate the text field with a preview of the
            // current selection.
            if (!ComboBoxList.Contains(TextBoxValue))
            {
               ComboBoxList.Clear();
               List<string> lstCards = Model.GetMatchingCollectionItems(aszSearch.ToLower());
               lstCards.ForEach(x => ComboBoxList.Add(x));
            }

            ComboBoxIsEditable = true;
         }
         else { ComboBoxList.Clear(); ComboBoxDropDown = false; }

         PropertyChanged += propertyChangedEventHandler;
      }

      #region Event Handlers
      private void eOKCommand(object canExecute)
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCISuggestionsSearchBox.OK, RoutingName);
         DisplayEvent(this, eventArgs);
      }

      private void propertyChangedEventHandler(object sender, PropertyChangedEventArgs e)
      {
         string szPropertyNamed = e.PropertyName;
         if (szPropertyNamed == "TextBoxValue")
         {
            eTextBoxValueChanged();
         }
      }

      private void eTextBoxValueChanged()
      {
         Model.CurrentSearchName = TextBoxValue;
      }
      #endregion

      #region IViewComponent
      public List<StoreFrontMenuItem> GetMenuItems()
      {
         return new List<StoreFrontMenuItem>();
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {
         ActionName = Model.ActionButtonName;
         UpdateSearchBoxAndList(Model.CurrentSearchName);
      }
      #endregion
   }
}
