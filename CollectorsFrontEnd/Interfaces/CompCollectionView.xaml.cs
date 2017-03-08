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
using System.ComponentModel;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCollectionView.xaml
    /// </summary>
    public partial class CompCollectionView : UserControl, IComponent, INotifyPropertyChanged
    {
        #region DataBinding
        private UserControl _ImageComponent;
        public UserControl ImageComponent
        {
            get
            {
                return _ImageComponent;
            }

            set
            {
                _ImageComponent = value;
                OnPropertyChanged("ImageComponent");
            }
        }

        private readonly ObservableCollection<CompSubGeneralization> _LstGeneralizations = new ObservableCollection<CompSubGeneralization>();
        public ObservableCollection<CompSubGeneralization> LstGeneralizations
        {
            get
            {
                return _LstGeneralizations;
            }
        }

        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion
        #region Public Fields
        public event ComponentEvent UnhandledEvent;
        public event PropertyChangedEventHandler PropertyChanged;

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
                CompSubGeneralization CSG = new CompSubGeneralization(mapGeneralizations[szGeneralization], szGeneralization);
                CSG.UnhandledEvent += RouteReceivedUnhandledEvent;
                LstGeneralizations.Add(CSG);
            }

            updateCardDisplay(DataModel.LstCopyModels[0]);
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
            if (DataModel.CollectionName != "" && alstCardModels.Count > 0)
            {
                CompSubAmountInterchanger ITI = new CompSubAmountInterchanger(alstCardModels);
                m_OverlayControl = ITI;
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                SPItemsControl.IsEnabled = false;
            }
        }

        private void showAttrChangerWindow(CardModel aDataModel)
        {
            showMainDisplay();
            if (DataModel.CollectionName != "")
            {
                CompSubAttributeChanger ITI = new CompSubAttributeChanger(aDataModel);
                m_OverlayControl = ITI;
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                SPItemsControl.IsEnabled = false;
            }
        }

        private void updateCardDisplay(CardModel aCM)
        {
            ImageComponent = new CompSubCardDisplayer(aCM);
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
                List<Tuple<string, string>> LstIgnoreGeneralizationMetaTags = new List<Tuple<string, string>>()
                {
                    new Tuple<string, string>("_ignore","Generalization")
                };
                LstIgnoreGeneralizationMetaTags = LstIgnoreGeneralizationMetaTags.Concat(aDataObject.LstMetaTags).ToList();

                List<Tuple<string, string>> LstIgnoreGeneralizationMetaTagsTwo = new List<Tuple<string, string>>()
                {
                    new Tuple<string, string>("_ignore","Generalization")
                };
                LstIgnoreGeneralizationMetaTagsTwo = LstIgnoreGeneralizationMetaTagsTwo.Concat(CM.LstMetaTags).ToList();
                // Look for all the cards with matching "Long Name" and metatags then build an amount changer.
                if (!lstGens.Contains(CM.GetMetaTag("Generalization")) && CM.IsSameAs(aDataObject) && CM.IsSameMetaTags(LstIgnoreGeneralizationMetaTagsTwo, LstIgnoreGeneralizationMetaTags))
                {
                    lstGens.Insert(0, CM.GetMetaTag("Generalization"));
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
                    (CompSubAmountChanger.CompSubAmountChangerModel)AI.GetDataModel();
                int iChangeCount = 0;
                if ((iChangeCount = (oAmountChangerModel.CurrentAmount - oAmountChangerModel.StartAmount)) > 0)
                {
                    for (int i = 0; i < iChangeCount; i++)
                    {
                        DataModel.AddItem(aDataModel.Copies[0].CardNameLong, oAmountChangerModel.LstMetaTags);
                    }

                }
                else
                {
                    iChangeCount = -iChangeCount;
                    for (int i = 0; i < iChangeCount; i++)
                    {
                        DataModel.RemoveItem(aDataModel.Copies[0].CardNameLong, oAmountChangerModel.LstMetaTags);
                    }
                }
            }
            DataModel.Refresh();
            buildListView();
            showMainDisplay();
        }

        private void ecAmountInterchangerWindowClose()
        {
            showMainDisplay();
        }

        private void ecAttrChangerWindowOpen(CardModel aDataObject)
        {
            showAttrChangerWindow(aDataObject);
        }

        private void ecAttrChangerWindowClose()
        {
            showMainDisplay();
        }

        private void ecAttrChangerWindowAccept(CompSubAttributeChanger.CompSubAttributeChangerModel aDataModel)
        {
            // Calculate differences in meta tags
            // Calculate added tags
            List<Tuple<string, string>> LstAddedTags = new List<Tuple<string, string>>();
            foreach (var NewTup in aDataModel.LstCurrentMetaTags)
            {
                bool bFound = false;
                foreach (Tuple<string, string> Tup in aDataModel.CardModelObject.LstMetaTags)
                {
                    if (Tup.Item1 == NewTup.First && Tup.Item2 == NewTup.Second)
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    LstAddedTags.Add(new Tuple<string, string>(NewTup.First, NewTup.Second));
                }
            }

            // Calculate removed tags
            List<Tuple<string, string>> LstRemovedTags = new List<Tuple<string, string>>();
            foreach (Tuple<string, string> OldTup in aDataModel.CardModelObject.LstMetaTags)
            {
                bool bFound = false;
                foreach (var Tup in aDataModel.LstCurrentMetaTags)
                {
                    if (Tup.First == OldTup.Item1)
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    LstRemovedTags.Add(OldTup);
                }
            }

            List<Tuple<string, string>> LstChanges = new List<Tuple<string, string>>();
            foreach (Tuple<string, string> AddTag in LstAddedTags)
            {
                LstChanges.Add(AddTag);
            }

            foreach (Tuple<string, string> RemoveTag in LstRemovedTags)
            {
                // aDataModel.CardModelObject.RemoveMetaTag(RemoveTag.Item1);
                Tuple<string, string> RemoveTuple = new Tuple<string, string>(RemoveTag.Item1, "!NULL");
                LstChanges.Add(RemoveTuple);
            }

            aDataModel.CardModelObject.SubmitMetaTagChangesToServer(LstChanges);

            if (LstAddedTags.Count + LstRemovedTags.Count > 0)
            {
                DataModel.Refresh();
                buildListView();
            }

            showMainDisplay();
        }

        private void ecGeneralizationSelectionChange(CompSubGeneralization.CompSubGeneralizationModel aDataObject)
        {
            foreach (CompSubGeneralization Gen in LstGeneralizations)
            {
                if (Gen.GeneralizationName != aDataObject.GeneralizationName)
                {
                    Gen.LVItems.UnselectAll();
                }
            }

            updateCardDisplay(aDataObject.SelectedItemDisplayer.DataModel);
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
                if (aszAction == "Gen.DeltaAmtOpen")
                {
                    ecAmountInterchangerWindowOpen((CardModel)aDataObject);
                }
                else if (aszAction == "Gen.AttrChangerOpen")
                {
                    ecAttrChangerWindowOpen((CardModel)aDataObject);
                }
            }
            // From the attr changer
            else if (aDataObject.GetType() == typeof(CompSubAttributeChanger.CompSubAttributeChangerModel))
            {
                CompSubAttributeChanger.CompSubAttributeChangerModel oDataModel =
                    (CompSubAttributeChanger.CompSubAttributeChangerModel)aDataObject;
                if (aszAction == "Cancel")
                {
                    ecAttrChangerWindowClose();
                }
                else if (aszAction == "OK")
                {
                    ecAttrChangerWindowAccept(oDataModel);
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
            else if (aDataObject.GetType() == typeof(CompSubGeneralization.CompSubGeneralizationModel))
            {
                if (aszAction == "SelectionChanged")
                {
                    ecGeneralizationSelectionChange((CompSubGeneralization.CompSubGeneralizationModel)aDataObject);
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

        private void eSaveCollection_Click(object sender, RoutedEventArgs e)
        {
            DataModel.SaveCollection();
        }
    }
}
