using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CollectorsFrontEnd.InterfaceModels;
using System.Collections.ObjectModel;
using CollectorsFrontEnd.Interfaces.Subs;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCollectionView.xaml
    /// </summary>
    public partial class CompCollectionView : UserControl, IComponent
    {
        private readonly ObservableCollection<CompSubGeneralization> _LstGeneralizations = new ObservableCollection<CompSubGeneralization>();
        public ObservableCollection<CompSubGeneralization> LstGeneralizations
        {
            get
            {
                return _LstGeneralizations;
            }
        }
        #region Public Fields
        public event ComponentEvent UnhandledEvent;

        public CollectionModel DataModel;
        #endregion

        #region Private Fields
        private UserControl m_OverlayControl;
        #endregion

        #region Public Functions
        public CompCollectionView(string aszCollectionName)
        {
            InitializeComponent();
            DataContext = this;
            if ((DataModel = ServerInterfaceModel.GetCollectionModel(aszCollectionName)) == null)
            {
                DataModel = ServerInterfaceModel.GenerateCollectionModel(aszCollectionName);
            }

            buildListView();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(CompSubAddItemWindow.AddItemDataModel))
            {
                CompSubAddItemWindow.AddItemDataModel oDataModel =
                    (CompSubAddItemWindow.AddItemDataModel)aDataObject;
                if (aszAction == "AddItem")
                {
                    ecAddItem(oDataModel);
                }
                else if (aszAction == "Cancel")
                {
                    ecAddItemWindowClose();
                }
            }
        }
        #endregion

        #region Private Functions
        private void buildListView()
        {
            LstGeneralizations.Clear();
            Dictionary<string, List<CardModel>> mapGeneralizations = new Dictionary<string, List<CardModel>>();
            mapGeneralizations.Add("Main", new List<CardModel>());
            foreach (CardModel CM in DataModel.LstCopyModels)
            {
                string szGeneralization = CM.GetMetaTag("Generalization");
                if (szGeneralization != "")
                {
                    if (mapGeneralizations.ContainsKey(szGeneralization))
                    {
                        mapGeneralizations[szGeneralization].Add(CM);
                    }
                    else
                    {
                        mapGeneralizations.Add(szGeneralization, new List<CardModel>() { CM });
                    }
                }
                else
                {
                    mapGeneralizations["Main"].Add(CM);
                }
            }

            foreach (string szGeneralization in mapGeneralizations.Keys)
            {
                LstGeneralizations.Add(new CompSubGeneralization(mapGeneralizations[szGeneralization]));
            }
        }

        #endregion Private Functions

        #region Event Handlers
        private void ecAddItem(CompSubAddItemWindow.AddItemDataModel aDataModel)
        {
            //MainWindow.SCI.AddItem(ActiveCollection, m_CurrentAddItemWindow.ComboText, new List<Tuple<string, string>>());
            DataModel.AddItem(aDataModel.ComboBoxText, new List<Tuple<string, string>>());
            DataModel.Refresh();
            buildListView();
            ecAddItemWindowClose();
        }

        private void ecAddItemWindowClose()
        {
            CenterPanel.Children.Remove(m_OverlayControl);
            m_OverlayControl = null;
            SPItemsControl.IsEnabled = true;
        }
        #endregion

        #region UI Event Handlers
        public void eAddItemWindowButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataModel.CollectionName != "")
            {
                CompSubAddItemWindow ITI = new CompSubAddItemWindow();
                m_OverlayControl = ITI;
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                SPItemsControl.IsEnabled = false;
            }
        }
        #endregion
    }
}
