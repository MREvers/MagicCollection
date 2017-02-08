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

        public CollectionViewer(MainWindow aMW, string aszCollection)
        {
            InitializeComponent();
            m_Container = aMW;
            BtnAddItem.Click += eAddItem;
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

        public void eAddItem(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                ItemInterchanger ITI = new ItemInterchanger();
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                //RefreshCollectionView();
                LVItems.IsEnabled = false;
            }
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
