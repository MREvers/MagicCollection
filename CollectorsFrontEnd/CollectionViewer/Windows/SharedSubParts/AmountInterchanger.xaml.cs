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
using System.ComponentModel;

namespace CollectorsFrontEnd
{
    /// <summary>
    /// Interaction logic for AmountInterchanger.xaml
    /// </summary>
    public partial class AmountInterchanger : UserControl
    {
        public int iStartingCount;
        public List<Tuple<string, string>> MetaTags;

        public int _iCurrentCount = 1;
        public int iCurrentCount {
            get
            {
                return _iCurrentCount;
            }
            set
            {
                _iCurrentCount = value;
                TBAMount.Text = _iCurrentCount.ToString();
            }
        }



        public AmountInterchanger()
        {
            InitializeComponent();
            BtnIncrement.Click += eIncrement;
            BtnDecrement.Click += eDecrement;
        }

        public void SetName(string aszCollection)
        {
            LblCollectionName.Content = aszCollection;
        }

        public void SetStartCount(int iCount)
        {
            iStartingCount = iCount;
            iCurrentCount = iCount;
        }

        private void eIncrement(object sender, RoutedEventArgs e)
        {
            iCurrentCount++;
        }

        private void eDecrement(object sender, RoutedEventArgs e)
        {
            iCurrentCount = Math.Max(iCurrentCount - 1, 0);
        }
    }
}
