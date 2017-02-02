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
        public CollectionViewer()
        {
            InitializeComponent();
            BtnAddItem.Click += eAddItem;
            BtnSaveCollection.Click += eSaveCollection;
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
            MainWindow.SCI.AddItem("Primary", "Sylvan Advocate");
            ClearText();
            List<string> lstCards = MainWindow.SCI.GetCollectionList("Primary");
            foreach (string szCard in lstCards)
            {
                AppendText(szCard + Environment.NewLine);
            }
        }

        public void eSaveCollection(object sender, RoutedEventArgs e)
        {
            MainWindow.SCI.SaveCollection("Primary");
        }
    }
}
