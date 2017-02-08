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
using System.Windows.Controls.Primitives;

namespace CollectorsFrontEnd
{
    /// <summary>
    /// Interaction logic for ItemInterchanger.xaml
    /// </summary>
    public partial class ItemInterchanger : UserControl
    {
        public ItemInterchanger()
        {
            InitializeComponent();

        }

        private void BtnRemoveCard_Click(object sender, RoutedEventArgs e)
        {
            if ((this.Parent as Window) != null)
            {
                (this.Parent as Window).Close();
            }
        }

        private void eTBTextChanged(object sender, TextChangedEventArgs e)
        {
            string szTypedText = TbAddCard.Text;

            List<string> lstSuggestions = new List<string>() { "Dog" };

            Popup codePopup = new Popup();
            TextBlock popupText = new TextBlock();
            popupText.Text = "Popup Text";
            popupText.Background = Brushes.LightBlue;
            popupText.Foreground = Brushes.Blue;
            codePopup.Child = popupText;
            codePopup.Visibility = Visibility.Visible;
            Grd.Children.Add(codePopup);
        }
    }
}
