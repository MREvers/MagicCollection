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

        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += MainWindow_Loaded;
            FileMICollectionsOverview.Click += MI_CollectionOverview_Click;
            ImportMTGCards.Click += MI_Import_Click;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            SCI = new ServerClientInterface();

            SCI.LoadCollection("Primary.txt");
            SCI.LoadCollection("Main.txt");

            CollectionOverviewer cv = new CollectionOverviewer(this);
            cv.SetValue(Grid.RowProperty, 1);
            GrdMain.Children.Add(cv);
        }

        #region Event Handler Wrappers
        public void MI_CollectionOverview_Click(object sender, RoutedEventArgs e)
        {
            eGoToCollectionOverview();
        }
        public void MI_Import_Click(object sender, RoutedEventArgs e)
        {
            SCI.ImportCollection();
        }
        #endregion

        #region Window Interface
        // I only need event handlers here if they open different "MainWindows"
        public void eGoToViewCollection(string aszCollection)
        {
            GrdMain.Children.RemoveAt(1);
            CollectionViewer cv = new CollectionViewer(this, aszCollection);
            cv.SetValue(Grid.RowProperty, 1);
            GrdMain.Children.Add(cv);
        }

        public void eGoToCollectionOverview()
        {
            GrdMain.Children.RemoveAt(1);
            CollectionOverviewer cv = new CollectionOverviewer(this);
            cv.SetValue(Grid.RowProperty, 1);
            GrdMain.Children.Add(cv);
        }
        #endregion
    }
}
