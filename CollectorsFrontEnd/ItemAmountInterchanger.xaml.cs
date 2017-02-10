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
        public string ActiveCard {get; set;}
        public int ICountOf = 0;

        public ItemAmountInterchanger()
        {
            InitializeComponent();
        }

        public void SetCard(string aszCard, int iCount)
        {
            LblCardName.Content = aszCard;
            
            ActiveCard = aszCard;
            ICountOf = iCount;
            PopulateColCounts();
        }

        public void PopulateColCounts()
        {
            // Add code here to fetch generalization amounts. Until then...
            AmountInterchanger AI = new AmountInterchanger();
            AI.SetName( "Main");
            AI.TBAMount.Text = ICountOf.ToString();
            SPCollections.Children.Add(AI);
        }
    }
}
