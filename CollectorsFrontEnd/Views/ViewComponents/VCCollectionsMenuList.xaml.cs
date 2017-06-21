using CollectorsFrontEnd.StoreFrontSupport;
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

namespace CollectorsFrontEnd.Views.ViewComponents
{
    /// <summary>
    /// View Component (VC) Data Context is usually set by the constructing object.
    /// Interaction logic for VCCollectionsMenuList.xaml
    /// </summary>
    public partial class VCCollectionsMenuList : UserControl, IView
    {
        public event DisplayEventHandler DisplayEvent;

        public VCCollectionsMenuList()
        {
            InitializeComponent();
        }

        public List<object> GetViewHook(ViewComponentHandles ViewHandle)
        {
            switch (ViewHandle)
            {
                case ViewComponentHandles.Buttons:
                    return new List<object>() { DisplayEvent };
                default:
                    return null;
            }
        }

        private void eCollectionSelection_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            DisplayEventArgs eventDetails = new DisplayEventArgs(
                Source: "VCCollectionsMenuList",
                Property: "CollectionSelection",
                Event: "SelectionChanged");
            DisplayEvent(Source: this, Event: eventDetails);
        }

        private void eViewCollection_Click(object sender, RoutedEventArgs e)
        {
            DisplayEventArgs eventDetails = new DisplayEventArgs(
                Source: "VCCollectionsMenuList",
                Property: "ViewCollectionButton",
                Event: "Clicked");
            DisplayEvent(Source: this, Event: eventDetails);
        }
    }
}
