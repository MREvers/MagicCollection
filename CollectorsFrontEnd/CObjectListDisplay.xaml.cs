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
    /// Interaction logic for CObjectListDisplay.xaml
    /// </summary>
    public partial class CObjectListDisplay : UserControl
    {
        public class COListItem
        {
            public int Amount { get; set; }
            public string Name { get; set; }
            public string Field_3 { get; set; }
        }

        public static int ColumnCount = 2;
        public static List<string> ListColumnHeaders { get; set; } = new List<string>() { "Amount", "Name" };
        public List<string> ListColumnItems { get; set; } = new List<string>();
        public string CardString;
        public int iCount = 0;

        public CObjectListDisplay()
        {
            InitializeComponent();
        }

        public void SetCard(string aszCollectionObjectString)
        {
            CardString = aszCollectionObjectString;
            MCopyObject oParsed = MainWindow.SCI.ConvertItemToCopyObject(aszCollectionObjectString);
            if (oParsed.Attributes != null && oParsed.Name != null)
            {
                try
                {
                    iCount = oParsed.Amount;
                    ListColumnItems.Add(oParsed.Amount.ToString());
                    
                }
                catch {}
                ListColumnItems.Add(oParsed.Name.ToString());
            }
            
        }

        public void LstViewMain_OnPreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            e.Handled = true;
        }
    }
}
