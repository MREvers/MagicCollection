using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
    class VMSuggestionsSearchBox: ViewModel<MSuggestionsSearchBox>, IViewComponent
    {
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

        public event DisplayEventHandler DisplayEvent;

        public VMSuggestionsSearchBox(MSuggestionsSearchBox Model) : base(Model)
        {
            OKCommand = new RelayCommand(eOKCommand);
            PropertyChanged += propertyChangedEventHandler;
            ActionName = Model.ActionName;
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            return new List<StoreFrontMenuItem>();
        }

        public void eOKCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs("VMSuggestionsSearchBox", "OKCommand", "Execute");
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
            string szDropDownText = TextBoxValue;
            ComboBoxList.Clear();
            //m_bTabbedAddCard = false;
            if (szDropDownText.Length > 3)
            {
                PropertyChanged -= propertyChangedEventHandler;
                ComboBoxIsEditable = false;
                ComboBoxDropDown = true;

                List<string> lstCards = Model.GetMatchingCollectionItems(szDropDownText.ToLower());
                lstCards.ForEach(x => ComboBoxList.Add(x));

                ComboBoxIsEditable = true;
                PropertyChanged += propertyChangedEventHandler;
            }

        }
    }
}
