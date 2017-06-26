using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class VMCollectionEditor : ViewModel<CollectionModel>
    {
        public ObservableCollection<string> TextChangesList { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> AddComboBoxList { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> RemoveComboBoxList { get; set; } = new ObservableCollection<string>();

        private string _RemoveCardDisplay;
        public string RemoveCardDisplay
        {
            get { return _RemoveCardDisplay; }
            set { _RemoveCardDisplay = value; OnPropertyChanged(); }
        }

        private string _AddCardDisplay;
        public string AddCardDisplay
        {
            get { return _AddCardDisplay; }
            set { _AddCardDisplay = value; OnPropertyChanged(); }
        }

        public RelayCommand AcceptCommand { get; set; }
        public RelayCommand CancelCommand { get; set; }
        public RelayCommand RemoveCardCommand { get; set; }
        public RelayCommand AddCardCommand { get; set; }

        private bool _AddComboBoxDropDown;
        public bool AddComboBoxDropDown
        {
            get { return _AddComboBoxDropDown; }
            set { _AddComboBoxDropDown = value; OnPropertyChanged(); }
        }

        private bool _RemoveComboBoxDropDown;
        public bool RemoveComboBoxDropDown
        {
            get { return _RemoveComboBoxDropDown; }
            set { _RemoveComboBoxDropDown = value; OnPropertyChanged(); }
        }

        private bool _AddComboBoxIsEditable = true;
        public bool AddComboBoxIsEditable
        {
            get { return _AddComboBoxIsEditable; }
            set { _AddComboBoxIsEditable = value; OnPropertyChanged(); }
        }

        private bool _RemoveComboBoxIsEditable = true;
        public bool RemoveComboBoxIsEditable
        {
            get { return _RemoveComboBoxIsEditable; }
            set { _RemoveComboBoxIsEditable = value; OnPropertyChanged(); }
        }

        private List<string> m_lstRealChanges { get; set; }
        private bool m_bTabbedAddCard;
        private bool m_bTabbedRemoveCard;

        public VMCollectionEditor(CollectionModel Model) : base(Model)
        {
            AcceptCommand = new RelayCommand(eAcceptCommand);
            CancelCommand = new RelayCommand(eCancelCommand);
            RemoveCardCommand = new RelayCommand(eRemoveCardCommand);
            AddCardCommand = new RelayCommand(eAddCardCommand);

            PropertyChanged += propertyChangedEventHandler;
        }

        private void propertyChangedEventHandler(object sender, PropertyChangedEventArgs e)
        {
            string szPropertyNamed = e.PropertyName;
            if (szPropertyNamed == "AddCardDisplay")
            {
                eAddCardTextChanged();
            }
        }

        private void eAddCardTextChanged()
        {
            string szDropDownText = AddCardDisplay;
            AddComboBoxList.Clear();
            m_bTabbedAddCard = false;
            if (szDropDownText.Length > 3)
            {
                PropertyChanged -= propertyChangedEventHandler;
                AddComboBoxIsEditable = false;
                AddComboBoxDropDown = true;

                List<string> lstCards = ServerInterface.Server.GetAllCardsStartingWith(szDropDownText.ToLower());
                lstCards.ForEach(x => AddComboBoxList.Add(x));

                AddComboBoxIsEditable = true;
                PropertyChanged += propertyChangedEventHandler;
            }
        }

        private void eRemoveCardTextChanged()
        {
            string szDropDownText = RemoveCardDisplay;
            RemoveComboBoxList.Clear();
            m_bTabbedRemoveCard = false;
            if (szDropDownText.Length > 3)
            {
                PropertyChanged -= propertyChangedEventHandler;
                RemoveComboBoxIsEditable = false;
                RemoveComboBoxDropDown = true;

                List<string> lstCards = Model.GetAllCardsStartingWith(szDropDownText.ToLower());
                lstCards.ForEach(x => AddComboBoxList.Add(x));

                RemoveComboBoxIsEditable = true;
                PropertyChanged += propertyChangedEventHandler;
            }
        }

        private void eAcceptCommand(object canExecute)
        {

        }

        private void eCancelCommand(object canExecute)
        {

        }

        private void eAddCardCommand(object canExecute)
        {
            if (m_bTabbedAddCard)
            {
                string szCmdString = "";
                szCmdString += "+ " + AddCardDisplay;
                TextChangesList.Add(szCmdString);
                m_lstRealChanges.Add(szCmdString);
                AddCardDisplay = "";
                RemoveCardDisplay = "";
            }
        }

        private void eRemoveCardCommand(object canExecute)
        {
        }

    }
}
