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
    /// Interaction logic for CollectionViewGeneralization.xaml
    /// </summary>
    public partial class CollectionViewGeneralization : UserControl
    {
        public List<CObjectListDisplay> Items = new List<CObjectListDisplay>();

        public CollectionViewGeneralization()
        {
            InitializeComponent();
            LVItems.ItemsSource = Items;
        }
    }
}
