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
        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += MainWindow_Loaded;
            

            //Client.ServerInterface WSI = new Client.ServerInterface();
            
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            Ttt.MouseEnter += Ttt_MouseEnter;
            ServerClientInterface SCI = new ServerClientInterface();
            SCI.LoadCollection("TestCollection.txt");
            SCI.LoadCollection("TestCollectionTwo.txt");
            tbPrimaryTest.Text = "";
            List<string> lstCards = SCI.GetCollectionList("Primary");
            foreach (string szCard in lstCards)
            {
                tbPrimaryTest.Text += szCard + Environment.NewLine;
            }

        }

        private void Ttt_MouseEnter(object sender, MouseEventArgs e)
        {
            Ttt.Text = "YoDDog";
        }
    }
}
