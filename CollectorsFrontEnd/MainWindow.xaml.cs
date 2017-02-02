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
using System.Diagnostics;
using System.Threading;

namespace CollectorsFrontEnd
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static public ServerClientInterface SCI;

        CollectionViewer m_ColViewer;

        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += MainWindow_Loaded;

            m_ColViewer = new CollectionViewer();

            grdPrimaryView.Children.Add(m_ColViewer);
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            SCI = new ServerClientInterface();

            SCI.LoadCollection("TestCollectionTwo.txt");

            List<string> lstCards = MainWindow.SCI.GetCollectionList("Primary");
            foreach (string szCard in lstCards)
            {
                m_ColViewer.AppendText(szCard + Environment.NewLine);
            }
        }


    }
}
