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
    public class CollectionViewerDataSelector : DataTemplateSelector
    {

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            FrameworkElement elemnt = container as FrameworkElement;
            if (item is CObjectListDisplay)
            {
                return elemnt.FindResource("COLDisplayTemplate") as DataTemplate;
            }
            else
            {
                return elemnt.FindResource("SeparatorTemplate") as DataTemplate;
            }
        }
    }

    /// <summary>
    /// Interaction logic for CollectionViewer.xaml
    /// </summary>
    public partial class CollectionViewer : UserControl
    {
        private string ActiveCollection = "";
        private MainWindow m_Container;
        private ItemInterchanger m_CurrentAddItemWindow;
        private ItemAmountInterchanger m_CurrentAmountInterchangerWindow;

        public CollectionViewer(MainWindow aMW, string aszCollection)
        {
            InitializeComponent();
            m_Container = aMW;
            BtnAddItem.Click += eAddItemWindowButton;
            BtnSaveCollection.Click += eSaveCollection;
            ActiveCollection = aszCollection;

            RefreshCollectionView();

        }

        public void RefreshCollectionView()
        {
            // THis gives the long name
            List<string> lstCards = MainWindow.SCI.GetCollectionList(ActiveCollection);
            Dictionary<string, CollectionViewGeneralization> genMap = new Dictionary<string, CollectionViewGeneralization>();
            SPDisplay.Children.Clear();
            foreach (string szCard in lstCards)
            {
                Dictionary<string, int> GeneralizationsList = new Dictionary<string, int>();
                
                List<List<Tuple<string,string>>> lstTags = MainWindow.SCI.GetMetaTags(ActiveCollection, szCard);
                int iCopyCount = 0;
                // Go through and look for generalizations on this copy.
                foreach(List<Tuple<string,string>> TagList in lstTags)
                {
                    foreach (Tuple<string,string> Tuple in TagList)
                    {
                        if (Tuple.Item1 == "Generalization")
                        {
                            if (GeneralizationsList.ContainsKey(Tuple.Item2))
                            {
                                GeneralizationsList[Tuple.Item2] += 1;
                            }
                            else
                            {
                                GeneralizationsList[Tuple.Item2] = 1;
                            }
                            if (Tuple.Item2.ToLower() != "main")
                            {
                                iCopyCount++;
                            }
                            
                        }
                    }
                } // End Sort generalizations
                CObjectListDisplay COCardParse = new CObjectListDisplay();
                COCardParse.SetCard(szCard);

                GeneralizationsList["Main"] = COCardParse.iCount - iCopyCount;

                foreach (string szGeneralization in GeneralizationsList.Keys)
                {
                    if (GeneralizationsList[szGeneralization] > 0)
                    {
                        CollectionViewGeneralization CVG;
                        if (genMap.ContainsKey(szGeneralization))
                        {
                            CVG = genMap[szGeneralization];
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
                                @"<DataTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"">"+
                                "<ContentControl Content=\"{Binding}\"/>"+
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
                            SPDisplay.Children.Add(CVG);
                            genMap[szGeneralization] = CVG;
                        }

                        CObjectListDisplay COListDisplay = new CObjectListDisplay();
                        
                        COListDisplay.SetCard(szCard);

                        COListDisplay.ListColumnItems[0] = GeneralizationsList[szGeneralization].ToString();

                        COListDisplay.BtnAmountInterchanger.Click += eAmountInterchangeWindowOpen;

                        CVG.LVItems.Items.Add(COListDisplay);
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
                SPDisplay.IsEnabled = false;
            }
        }

        public void eAddItemWindowClose()
        {
            m_CurrentAddItemWindow.BtnAddCard.Click -= eAddItem;
            m_CurrentAddItemWindow.BtnCancel.Click -= eAddItemWindowCancelButton;
            CenterPanel.Children.Remove(m_CurrentAddItemWindow);
            m_CurrentAddItemWindow = null;
            SPDisplay.IsEnabled = true;
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
        public void eAmountInterchangeWindowOpen(object sender, RoutedEventArgs e)
        {
            ItemAmountInterchanger IAI = new ItemAmountInterchanger();
            CObjectListDisplay Source = (CObjectListDisplay)sender;
            IAI.SetCard(ActiveCollection, Source.CardString);
            //ITI.BtnAddCard.Click += eAddItem;
            //ITI.BtnCancel.Click += eAddItemWindowCancelButton;
            m_CurrentAmountInterchangerWindow = IAI;
            Panel.SetZIndex(CenterPanel, 2);
            CenterPanel.Children.Add(IAI);
            SPDisplay.IsEnabled = false;
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
