using CollectorsFrontEnd.InterfaceModels;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubItemDisplayer.xaml
    /// </summary>
    public partial class CompSubItemDisplayer : UserControl
    {
        public CardModel DataModel { get; set; }

        public CompSubItemDisplayer(CardModel aCM)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = aCM;
        }
    }
}
