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
using System.Windows.Shapes;

namespace StoreFrontPro
{
    /// <summary>
    /// Interaction logic for StoreFrontMain.xaml
    /// </summary>
    public partial class StoreFrontMain : Window
    {
        /// <summary>
        /// This is the entry point of the program. This is required by wpf... I think.
        /// </summary>
        public StoreFrontMain()
        {
            InitializeComponent();

            // The main program.
            StoreFront programModel = new StoreFront();
            DataContext = programModel.StoreFrontVM;
        }
    }
}
