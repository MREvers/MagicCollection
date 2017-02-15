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
using System.Collections.ObjectModel;
using System.Windows.Shapes;

namespace CollectorsFrontEnd
{
    /// <summary>
    /// Interaction logic for ItemAttributesChanger.xaml
    /// </summary>
    public partial class ItemAttributesChanger : UserControl
    {
        public class ItemState
        {
            public string Key { get; set; }
            public string Starting { get; set; }
            public string Value { get; set; }
        }
        public List<ItemState> LstAttrs { get; set; } = new List<ItemState>();
        public List<ItemState> LstMetas { get; set; } = new List<ItemState>();

        public string CardNameShort { get; set; }
        public string ActiveCardLong { get; set; }
        public string ActiveCollection { get; set; }
        public List<Tuple<string, string>> MetaTags;


        public ItemAttributesChanger()
        {
            InitializeComponent();
            DataContext = this;
            LstAttrs.Add(new ItemState { Key = "Key1", Starting = "Start", Value = "Val" });
        }
    }
}
