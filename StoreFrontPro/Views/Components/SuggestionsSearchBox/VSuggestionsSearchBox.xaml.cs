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
            if (e.Key == Key.Tab)
            {
                e.Handled = true;

                SuggestionsButton.Focus();

                if (DataContext == null) { return; }
                VMSuggestionsSearchBox context = (DataContext as VMSuggestionsSearchBox);
                if (context.ComboBoxList.Count > 0 && !context.ComboBoxList.Contains(context.TextBoxValue))
                {
                    context.TextBoxValue = (string)context.ComboBoxList[0];
                }
            }
        }

        private void eButton_Click(object sender, RoutedEventArgs e)
        {
            SuggestionsComboBox.Focus();
        }
    }
}
