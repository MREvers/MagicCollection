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
    /// <summary>
    /// Interaction logic for CollectionOverviewer.xaml
    /// </summary>
    public partial class CollectionOverviewer : UserControl
    {
        private MainWindow m_Container;
        public CollectionOverviewer(MainWindow aMW)
        {
            InitializeComponent();
            m_Container = aMW;
            MI_SetLoadedCollectionList(MainWindow.SCI.GetLoadedCollections());
            BtnLoadCollection.Click += eLoadCollection;
            LstBoxLoadedCollections.SelectionChanged += eSelectionChange;
            BtnViewCollection.Click += eViewCollection;

            eSelectionChange(null, null);
        }

        #region MainInterface
        public void MI_SetLoadedCollectionList(List<string> alstCollections)
        {
            LstBoxLoadedCollections.Items.Clear();
            foreach (string szCollection in alstCollections)
            {
                LstBoxLoadedCollections.Items.Add(szCollection);
            }
            LstBoxLoadedCollections.SelectedIndex = 0;
        }
        #endregion MI

        #region Event Handlers
        public void eLoadCollection(object sender, RoutedEventArgs e)
        {
            if (LstBoxLoadedCollections.SelectedIndex > -1)
            {
                string szCollectionToLoad = LstBoxLoadedCollections.SelectedItem.ToString();
                // Create OpenFileDialog 
                Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

                // Set filter for file extension and default file extension 
                dlg.DefaultExt = ".txt";
                dlg.Filter = "Text Files (*.txt)|*.txt";

                // Display OpenFileDialog by calling ShowDialog method 
                Nullable<bool> result = dlg.ShowDialog();


                // Get the selected file name and display in a TextBox 
                if (result == true)
                {
                    // Open document 
                    MainWindow.SCI.LoadCollection(dlg.FileName);
                    MI_SetLoadedCollectionList(MainWindow.SCI.GetLoadedCollections());
                }
            }
        }

        public void eViewCollection(object sender, RoutedEventArgs e)
        {
            if (LstBoxLoadedCollections.SelectedIndex > -1)
            {
                string szCollectionToLoad = LstBoxLoadedCollections.SelectedItem.ToString();
                m_Container.eGoToViewCollection(szCollectionToLoad);
            }
        }

        public void eSelectionChange(object sender, RoutedEventArgs e)
        {
            if (LstBoxLoadedCollections.SelectedIndex > -1)
            {
                string szCollectionToLoad = LstBoxLoadedCollections.SelectedItem.ToString();

                List<string> lstCollectionList = MainWindow.SCI.GetCollectionList(szCollectionToLoad);

                LstBoxCollectionPreview.Items.Clear();
                foreach(string szItem in lstCollectionList)
                {
                    LstBoxCollectionPreview.Items.Add(szItem);
                }
            }
        }
        #endregion
    }
}
