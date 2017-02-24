using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubGeneralization.xaml
    /// </summary>
    public partial class CompSubGeneralization : UserControl, INotifyPropertyChanged, IComponent
    {
        #region Bindings
        private ObservableCollection<CompSubItemDisplayer> _LstCardModels = new ObservableCollection<CompSubItemDisplayer>();
        public ObservableCollection<CompSubItemDisplayer> LstCardModels
        {
            get
            {
                return _LstCardModels;
            }
        }
        #endregion

        #region Property Change Notifiers
        public event PropertyChangedEventHandler PropertyChanged;
        public event ComponentEvent UnhandledEvent;

        protected virtual void OnPropertyChanged(string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        public CompSubGeneralization(List<CardModel> alstCardModels)
        {
            InitializeComponent();
            DataContext = this;
            foreach (CardModel CM in alstCardModels)
            {
                CompSubItemDisplayer CSID = new CompSubItemDisplayer(CM);
                CSID.UnhandledEvent += RouteReceivedUnhandledEvent;
                LstCardModels.Add(CSID);
            }

            buildTable();
        }

        private void buildTable()
        {

            GridViewColumn GVC = new GridViewColumn()
            {
                Header = "Amount",
                DisplayMemberBinding = new Binding("DataModel.Amount")
            };

            GVColumns.Columns.Add(GVC);

            GVC = new GridViewColumn()
            {
                Header = "Name",
                DisplayMemberBinding = new Binding("DataModel.CardName")
            };

            GVColumns.Columns.Add(GVC);

            StringReader sR = new StringReader(
                            @"<DataTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"">" +
                            "<ContentControl Content=\"{Binding}\"/>" +
                            "</DataTemplate>"
                            );
            XmlReader xR = XmlReader.Create(sR);
            DataTemplate DT = XamlReader.Load(xR) as DataTemplate;
            GVC = new GridViewColumn()
            {
                Header = "",
                CellTemplate = DT
            };

            GVColumns.Columns.Add(GVC);
        }

        #region Event Handlers


        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            // From CompSubItemDisplayer
            if (aDataObject.GetType() == typeof(CardModel))
            {
                if (aszAction == "DeltaAmtOpen")
                {
                    // Explicitly Pass it up.
                    UnhandledEvent(aDataObject, aszAction);
                }
            }
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        #endregion
    }
}
