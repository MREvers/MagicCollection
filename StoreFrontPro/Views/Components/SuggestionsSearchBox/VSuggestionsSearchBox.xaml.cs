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

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
    /// <summary>
    /// Interaction logic for SuggestionsSearchBox.xaml
    /// </summary>
    public partial class VSuggestionsSearchBox : UserControl
    {
        public VSuggestionsSearchBox()
        {
            InitializeComponent();
        }

        private void ComboBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (DataContext == null) { return; }
            VMSuggestionsSearchBox context = (DataContext as VMSuggestionsSearchBox);
            if (e.Key == Key.Tab && context.ComboBoxList.Count > 0)
            {
                context.TextBoxValue = context.ComboBoxList[0];
            }
        }
    }
}
