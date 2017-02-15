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
        public List<Tuple<string, string, string>> ListKeyInitCurrentAttrs { get; set; } = new List<Tuple<string, string, string>>();
        public List<Tuple<string, string, string>> ListKeyInitCurrentMetas { get; set; } = new List<Tuple<string, string, string>>();
        public IEnumerable<string> ListCurrentAttrs {
            get
            {
                return ListKeyInitCurrentAttrs.Select(x => x.Item3);
            }
        }
        public IEnumerable<string> ListCurrentNames
        {
            get
            {
                return ListKeyInitCurrentAttrs.Select(x => x.Item1);
            }
        }
        public IEnumerable<string> ListCurrentMetas
        {
            get
            {
                return ListKeyInitCurrentMetas.Select(x => x.Item3);
            }
        }
        public IEnumerable<string> ListCurrentMetaNames
        {
            get
            {
                return ListKeyInitCurrentMetas.Select(x => x.Item1);
            }
        }
        public ItemAttributesChanger()
        {
            InitializeComponent();
            DataContext = this;
            //SPAttributes.ItemsSource = ListKeyInitCurrentAttrs;
            ListKeyInitCurrentAttrs.Add(new Tuple<string, string, string>("Cat", "Dog", "Meow"));
        }
    }
}
