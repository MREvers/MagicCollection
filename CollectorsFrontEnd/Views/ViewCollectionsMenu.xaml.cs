using CollectorsFrontEnd.StoreFrontSupport;
using CollectorsFrontEnd.Views.ViewComponents;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
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
using System.Collections.ObjectModel;

namespace CollectorsFrontEnd.Views
{
    /// <summary>
    /// Interaction logic for ViewCollectionsMenu.xaml
    /// </summary>
    public partial class ViewCollectionsMenu : UserControl, INotifyPropertyChanged
    {
        #region Bindings
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get
            {
                return _OperationWindow;
            }
            set
            {
                _OperationWindow = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Properties
        
        public VMCollectionsMenu VMCollectionsMenu { get; set; }

        #endregion

        #region Public Methods
        public ViewCollectionsMenu()
        {
            InitializeComponent();
            VMCollectionsMenu = new VMCollectionsMenu(Model: this);
            DataContext = VMCollectionsMenu;

            OperationWindow.SetNewDisplay(
                    Name: "AvailableCollections",
                    NewDisplay: new VCCollectionsMenuList(),
                    DataContext: this.DataContext,
                    Persist: false);
            // Call the server function which takes the callback and whatever args.
            // The server queues up that function and the call back.
            ServerInterface.Server.GetLoadedCollectionList(refreshCollectionListDisplay, UICallback: true);
        }
        #endregion

        #region Private Methods

        private void refreshCollectionListDisplay(List<string> alstCollList)
        {
            VMCollectionsMenu.AvailableCollections.Clear();
            foreach(string szCollection in alstCollList)
            {
                VMCollectionsMenu.AvailableCollections.Add(szCollection);
            }
        }

        #endregion
    }
}
