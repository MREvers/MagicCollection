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

namespace CollectorsFrontEnd.Interfaces.Controls
{
    /// <summary>
    /// Interaction logic for Control_OverlayPanel.xaml
    /// </summary>
    public partial class Control_OverlayPanel : UserControl
    {
        public UserControl MainControl;
        public UserControl Overlay;

        public Control_OverlayPanel()
        {
            InitializeComponent();
        }

        public void SetMainControl(UserControl aoControl)
        {
            MainControl = aoControl;
            CenterPanel.Children.Add(MainControl);
        }

        public void ShowOverlay(UserControl aoOverlay)
        {
            Overlay = aoOverlay;
            CenterPanel.Children.Add(Overlay);
            MainControl.IsEnabled = false;
            Panel.SetZIndex(Overlay, 2);
        }

        public void ShowMain()
        {
            if (Overlay != null)
            {
                CenterPanel.Children.Remove(Overlay);
                Panel.SetZIndex(Overlay, 0);
                MainControl.IsEnabled = true;
            }
        }

    }
}
