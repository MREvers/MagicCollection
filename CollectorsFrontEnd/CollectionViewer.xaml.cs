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
    /// Interaction logic for CollectionViewer.xaml
    /// </summary>
    public partial class CollectionViewer : UserControl
    {
        private string ActiveCollection = "";

        public CollectionViewer()
        {
            InitializeComponent();
            BtnAddItem.Click += eAddItem;
            BtnSaveCollection.Click += eSaveCollection;
            BtnLoadCollection.Click += eLoadCollection;
        }

        public void RefreshCollectionView()
        {
            ClearText();
            List<string> lstCards = MainWindow.SCI.GetCollectionList(ActiveCollection);
            foreach (string szCard in lstCards)
            {
                AppendText(szCard + Environment.NewLine);
            }
        }

        public void AppendText(string aszText)
        {
            tbPrimaryTest.Text += aszText;
        }

        public void ClearText()
        {
            tbPrimaryTest.Text = "";
        }

        public void eAddItem(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                MainWindow.SCI.AddItem(ActiveCollection, "Sylvan Advocate");
                RefreshCollectionView();
            }
        }

        public void eSaveCollection(object sender, RoutedEventArgs e)
        {
            if (ActiveCollection != "")
            {
                MainWindow.SCI.SaveCollection(ActiveCollection);
            }
            
        }

        public void eLoadCollection(object sender, RoutedEventArgs e)
        {
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
                ActiveCollection = MainWindow.SCI.LoadCollection(dlg.FileName);
                RefreshCollectionView();
            }
        }
    }
}
