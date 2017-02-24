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

        public IDataModel GetDataModel()
        {
            return DataModel;
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
                CompSubGeneralization CSG = new CompSubGeneralization(mapGeneralizations[szGeneralization]);
                CSG.UnhandledEvent += RouteReceivedUnhandledEvent;
                LstGeneralizations.Add(CSG);
            }
        }

        private void showAddItemWindow()
        {
            showMainDisplay();
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

        private void showItemInterchangerWindow(List<CardModel> alstCardModels)
        {
            showMainDisplay();
            if (DataModel.CollectionName != "")
            {
                CompSubAmountInterchanger ITI = new CompSubAmountInterchanger(alstCardModels);
                m_OverlayControl = ITI;
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                SPItemsControl.IsEnabled = false;
            }
        }

        private void showMainDisplay()
        {
            CenterPanel.Children.Remove(m_OverlayControl);
            m_OverlayControl = null;
            SPItemsControl.IsEnabled = true;
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
            showMainDisplay();
        }

        private void ecAmountInterchangerWindowOpen(CardModel aDataObject)
        {
            List<CardModel> lstCards = new List<CardModel>();
            List<string> lstGens = new List<string>();
            foreach (CardModel CM in DataModel.LstCopyModels)
            {
                // Look for all the cards with matching "Long Name" then build an amount changer.
                if (!lstGens.Contains(CM.GetMetaTag("Generalization")) && CM.IsSameAs(aDataObject))
                {
                    lstGens.Add(CM.GetMetaTag("Generalization"));
                    lstCards.Add(CM);
                }
            }
            showItemInterchangerWindow(lstCards);
        }

        private void ecAmountInterchangerWindowAccept(CompSubAmountInterchanger.AmountInterchangerModel aDataModel)
        {
            List<CompSubAmountChanger> ListChanges = aDataModel.LstGeneralizations;
            
            foreach (CompSubAmountChanger AI in ListChanges)
            {
                CompSubAmountChanger.CompSubAmountChangerModel oAmountChangerModel =
                    (CompSubAmountChanger.CompSubAmountChangerModel) AI.GetDataModel();
                int iChangeCount = 0;
                if ((iChangeCount = (oAmountChangerModel.EndAmount - oAmountChangerModel.StartAmount)) > 0)
                {
                    if (aDataModel.Copies[0].LstMetaTags.Count == 0 && oAmountChangerModel.Title != "Main")
                    {
                        aDataModel.Copies[0].LstMetaTags.Add(new Tuple<string, string>("Generalization", oAmountChangerModel.Title));
                    }
                    for (int i = 0; i < iChangeCount; i++)
                    {
                        DataModel.AddItem(aDataModel.Copies[0].CardNameLong, aDataModel.Copies[0].LstMetaTags);
                    }

                }
                else
                {
                    iChangeCount = -iChangeCount;
                    for (int i = 0; i < iChangeCount; i++)
                    {
                        DataModel.RemoveItem(aDataModel.Copies[0].CardNameLong, aDataModel.Copies[0].LstMetaTags);
                    }
                }
            }
            showMainDisplay();
        }

        private void ecAmountInterchangerWindowClose()
        {
            showMainDisplay();
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
            // From One of the Entries in a Generalization (ItemDisplayer)
            else if (aDataObject.GetType() == typeof(CardModel))
            {
                if (aszAction == "DeltaAmtOpen")
                {
                    ecAmountInterchangerWindowOpen((CardModel)aDataObject);
                }
            }
            // From The AmountInterchanger
            else if (aDataObject.GetType() == typeof(CompSubAmountInterchanger.AmountInterchangerModel))
            {
                if (aszAction == "Close")
                {
                    ecAmountInterchangerWindowClose();
                }
                else if (aszAction == "OK")
                {
                    ecAmountInterchangerWindowAccept((CompSubAmountInterchanger.AmountInterchangerModel)aDataObject);
                }
            }
        }
        #endregion

        #region UI Event Handlers
        public void eAddItemWindowButton_Click(object sender, RoutedEventArgs e)
        {
            showAddItemWindow();
        }
        #endregion
    }
}
