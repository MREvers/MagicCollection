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
using System.IO;
using System.Windows.Markup;
using System.Xml;

namespace CollectorsFrontEnd
{

    /// <summary>
    /// Interaction logic for CollectionViewer.xaml
    /// </summary>
    public partial class CollectionViewer : UserControl
    {
        private string ActiveCollection = "";
        private MainWindow m_Container;
        private ItemInterchanger m_CurrentAddItemWindow;
        private ItemAmountInterchanger m_CurrentAmountInterchangerWindow;
        public List<CollectionViewGeneralization> ListGeneralizations { get; set; } = new List<CollectionViewGeneralization>();

        public CollectionViewer(MainWindow aMW, string aszCollection)
        {
            InitializeComponent();
            m_Container = aMW;
            BtnAddItem.Click += eAddItemWindowButton;
            BtnSaveCollection.Click += eSaveCollection;
            ActiveCollection = aszCollection;

            SPItemsControl.ItemsSource = ListGeneralizations;

            RefreshCollectionView();

        }

        public void RefreshCollectionView()
        {
            // THis gives the long name
            List<Tuple<string, List<Tuple<string, string>>>> lstCards = MainWindow.SCI.GetCollectionListWithMeta(ActiveCollection);
            Dictionary<string, CollectionViewGeneralization> genMap = new Dictionary<string, CollectionViewGeneralization>();
            ListGeneralizations.Clear();
            foreach (var otCardTagPair in lstCards)
            {
                Dictionary<string, List<Tuple<string, List<Tuple<string, string>>>>> GeneralizationsListOfCards
                    = new Dictionary<string, List<Tuple<string, List<Tuple<string, string>>>>>();
                GeneralizationsListOfCards["Main"] = new List<Tuple<string, List<Tuple<string, string>>>>();
                //Find the generalization it belongs to
                bool bFoundGen = false;
                foreach (var szTag in otCardTagPair.Item2)
                {
                    if (szTag.Item1 == "Generalization")
                    {
                        if (!GeneralizationsListOfCards.Keys.Contains(szTag.Item2))
                        {
                            GeneralizationsListOfCards[szTag.Item2] = new List<Tuple<string, List<Tuple<string, string>>>>()
                            {
                                otCardTagPair
                            };
                        }
                        else
                        {
                            GeneralizationsListOfCards[szTag.Item2].Add(otCardTagPair);
                        }
                        bFoundGen = true;
                        break;
                    }
                }

                if (!bFoundGen)
                {
                    GeneralizationsListOfCards["Main"].Add(otCardTagPair);
                }

                foreach (string szGen in GeneralizationsListOfCards.Keys)
                {
                    // Create or get the generalization view.
                    CollectionViewGeneralization CVG;
                    if (genMap.ContainsKey(szGen))
                    {
                        CVG = genMap[szGen];
                        
                    }
                    else
                    {
                        CVG = new CollectionViewGeneralization();
                        //CVG.GVColumns.Columns.Clear();
                        for (int i = 0; i < CObjectListDisplay.ColumnCount; i++)
                        {
                            GridViewColumn GVC = new GridViewColumn()
                            {
                                Header = CObjectListDisplay.ListColumnHeaders[i],
                                DisplayMemberBinding = new Binding("ListColumnItems[" + i + "]")
                            };

                            CVG.GVColumns.Columns.Add(GVC);
                        }
                        StringReader sR = new StringReader(
                            @"<DataTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"">" +
                            "<ContentControl Content=\"{Binding}\"/>" +
                            "</DataTemplate>"
                            );
                        XmlReader xR = XmlReader.Create(sR);
                        DataTemplate DT = XamlReader.Load(xR) as DataTemplate;
                        GridViewColumn GVCBind = new GridViewColumn()
                        {
                            Header = "",
                            CellTemplate = DT
                        };

                        CVG.GVColumns.Columns.Add(GVCBind);
                        genMap[szGen] = CVG;
                        CVG.Name = szGen;
                        ListGeneralizations.Add(CVG);
                        SPItemsControl.Items.Refresh();
                    }

                    foreach (var oCardTemp in GeneralizationsListOfCards[szGen])
                    {
                        CObjectListDisplay COListDisplay = new CObjectListDisplay();

                        COListDisplay.SetCard(oCardTemp.Item1, oCardTemp.Item2);

                        COListDisplay.ListColumnItems[0] = COListDisplay.iCount.ToString();

                        COListDisplay.OpenInterchanger += eAmountInterchangeWindowOpen;

                        CVG.Items.Add(COListDisplay);
                    }
                }
            }

        }

        #region AddItem Window
        public void eAddItemWindowButton(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                ItemInterchanger ITI = new ItemInterchanger();
                ITI.BtnAddCard.Click += eAddItem;
                ITI.BtnCancel.Click += eAddItemWindowCancelButton;
                m_CurrentAddItemWindow = ITI;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                SPItemsControl.IsEnabled = false;
            }
        }

        public void eAddItemWindowClose()
        {
            m_CurrentAddItemWindow.BtnAddCard.Click -= eAddItem;
            m_CurrentAddItemWindow.BtnCancel.Click -= eAddItemWindowCancelButton;
            CenterPanel.Children.Remove(m_CurrentAddItemWindow);
            m_CurrentAddItemWindow = null;
            SPItemsControl.IsEnabled = true;
        }

        public void eAddItem(object sender, RoutedEventArgs e)
        {
            MainWindow.SCI.AddItem(ActiveCollection, m_CurrentAddItemWindow.ComboText);

            eAddItemWindowClose();

            RefreshCollectionView();
        }

        public void eAddItemWindowCancelButton(object sender, RoutedEventArgs e)
        {
            eAddItemWindowClose();
        }
        #endregion AddItem Window

        #region AmountChanger Window
        public void eAmountInterchangeWindowOpen(CObjectListDisplay aCOLD)
        {
            ItemAmountInterchanger IAI = new ItemAmountInterchanger();

            IAI.SetCard(ActiveCollection, aCOLD.CardStringLong, ListGeneralizations);
            IAI.BtnOK.Click += eAIOK;
            //Btn add and btn remove
            IAI.BtnOK.Click += eAIOK;
            IAI.BtnCancel.Click += eAIClose;
            m_CurrentAmountInterchangerWindow = IAI;
            Panel.SetZIndex(CenterPanel, 2);
            CenterPanel.Children.Add(IAI);
            SPItemsControl.IsEnabled = false;
        }

        public void eAIOK(object sender, RoutedEventArgs e)
        {
            // Go through each generalization and see if the current count is different from start
            List<AmountInterchanger> ListChanges = m_CurrentAmountInterchangerWindow.ListInterchangers;
            foreach(AmountInterchanger AI in ListChanges)
            {
                int iChangeCount = 0;
                if ((iChangeCount = (AI.iStartingCount - AI.iCurrentCount)) > 0)
                {
                    for(int i = 0; i < iChangeCount; i++)
                    {
                        MainWindow.SCI.AddItem(ActiveCollection, m_CurrentAmountInterchangerWindow.ActiveCardLong);
                    }
                    
                }
                else
                {
                    iChangeCount = -iChangeCount;
                    for (int i = 0; i < iChangeCount; i++)
                    {
                        MainWindow.SCI.RemoveItem(ActiveCollection, m_CurrentAmountInterchangerWindow.ActiveCardLong);
                    }
                }
            }
            eAIClose(null, null);
        }

        public void eAIClose(object sender, RoutedEventArgs e)
        {
            CenterPanel.Children.Remove(m_CurrentAmountInterchangerWindow);
            m_CurrentAmountInterchangerWindow = null;
            SPItemsControl.IsEnabled = true;
        }

        #endregion

        public void eSaveCollection(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                MainWindow.SCI.SaveCollection(ActiveCollection);
            }

        }

        private void BtnBulkEdit_Click(object sender, RoutedEventArgs e)
        {
            string szName = "1x Thraben Inspector";
            MainWindow.SCI.AddMetaTag(ActiveCollection, szName, "Generalization", "Sideboard", new List<Tuple<string, string>>());
            RefreshCollectionView();
        }
    }
}
