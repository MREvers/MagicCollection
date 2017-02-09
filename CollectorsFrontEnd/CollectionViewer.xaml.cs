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

        public CollectionViewer(MainWindow aMW, string aszCollection)
        {
            InitializeComponent();
            m_Container = aMW;
            BtnAddItem.Click += eAddItemWindowButton;
            ActiveCollection = aszCollection;

            RefreshCollectionView();

        }

        public void RefreshCollectionView()
        {
            LVItems.ItemTemplateSelector = new CollectionViewerDataSelector();

            GVColumns.Columns.Clear();
            List<string> lstCards = MainWindow.SCI.GetCollectionList(ActiveCollection);
            for (int i = 0; i < CObjectListDisplay.ColumnCount; i++)
            {
                GridViewColumn GVC = new GridViewColumn()
                {
                    Header = CObjectListDisplay.ListColumnHeaders[i],
                    DisplayMemberBinding = new Binding("ListColumnItems[" + i + "]")
                };

                GVColumns.Columns.Add(GVC);
            }
            LVItems.Items.Clear();
            foreach (string szCard in lstCards)
            {
                CObjectListDisplay COListDisplay = new CObjectListDisplay();
                COListDisplay.SetCard(szCard);

                LVItems.Items.Add(COListDisplay);
            }
        }

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
                LVItems.IsEnabled = false;
            }
        }

        public void eAddItemWindowClose()
        {
            m_CurrentAddItemWindow.BtnAddCard.Click -= eAddItem;
            m_CurrentAddItemWindow.BtnCancel.Click -= eAddItemWindowCancelButton;
            CenterPanel.Children.Remove(m_CurrentAddItemWindow);
            m_CurrentAddItemWindow = null;
            LVItems.IsEnabled = true;
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


        public void eSaveCollection(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                MainWindow.SCI.SaveCollection(ActiveCollection);
            }
            
        }
    }
}
