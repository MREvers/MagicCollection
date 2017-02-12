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
    /// Interaction logic for ItemAmountInterchanger.xaml
    /// </summary>
    public partial class ItemAmountInterchanger : UserControl
    {
        public string ActiveCard { get; set; }
        public string ActiveCollection { get; set; }

        public ItemAmountInterchanger()
        {
            InitializeComponent();
        }

        public void SetCard(string aszCollection, string aszCard)
        {
            LblCardName.Content = aszCard;

            ActiveCard = aszCard;
            ActiveCollection = aszCollection;
            PopulateColCounts();

        }

        public void PopulateColCounts()
        {
            Dictionary<string, int> GeneralizationsList = new Dictionary<string, int>();

            // Add code here to fetch generalization amounts. Until then...
            
        }
    }
}
