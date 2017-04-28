using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubAmountInterchanger.xaml
    /// </summary>
    public partial class Module_AmountInterchanger : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Data Binding
        public ObservableCollection<Module_AmountChanger> LstGeneralizations
        {
            get
            {
                return DataModel.OCGeneralizations;
            }
        }

        private string _CardName;
        public string CardName
        {
            get
            {
                return _CardName;
            }
            set
            {
                _CardName = value;
                OnPropertyChanged("CardName");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Nested Types
        public class AmountInterchangerModel : IDataModel
        {
            public List<CardModel> Copies;
            public ObservableCollection<Module_AmountChanger> OCGeneralizations
                = new ObservableCollection<Module_AmountChanger>();
            public List<Module_AmountChanger> LstGeneralizations
            {
                get
                {
                    List<Module_AmountChanger> rLst = new List<Module_AmountChanger>();
                    foreach (Module_AmountChanger CSAC in OCGeneralizations)
                    {
                        rLst.Add(CSAC);
                    }
                    return rLst;
                }
            }

        }
        #endregion

        #region Public Events
        public event ComponentEvent UnhandledEvent;
        #endregion

        #region Public Fields
        public AmountInterchangerModel DataModel = new AmountInterchangerModel();
        #endregion

        #region Public Functions
        // Expects a list of IDENTICAL CARDS (Not included metatags)
        public Module_AmountInterchanger(List<CardModel> aLstCopies)
        {
            InitializeComponent();
            DataContext = this;
            DataModel.Copies = aLstCopies;
            CardName = aLstCopies[0].CardName;
            buildList();
        }

        public void buildList()
        {
            foreach (CardModel CM in DataModel.Copies)
            {
                string szGeneralization = CM.GetMetaTag("Generalization");
                szGeneralization = (szGeneralization == "" ? "Main" : szGeneralization);

                Module_AmountChanger oTemp = new Module_AmountChanger(szGeneralization, CM.Amount, CM.LstMetaTags);
                DataModel.LstGeneralizations.Add(oTemp);

            }
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }
        #endregion

        #region UI Event Handlers
        private void eBtnCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Close");
        }

        private void eBtnOK_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "OK");
        }
        #endregion
    }
}
