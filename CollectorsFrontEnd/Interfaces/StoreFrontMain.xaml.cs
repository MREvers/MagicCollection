using CollectorsFrontEnd.InterfaceModels;
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
using System.Windows.Shapes;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for StoreFrontMain.xaml
    /// </summary>
    public partial class StoreFrontMain : Window, INotifyPropertyChanged, IComponent
    {
        #region Bindings
        private IComponent _OperationWindow;
        public IComponent OperationWindow {
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
        #endregion

        #region Property Change Notifiers
        public event PropertyChangedEventHandler PropertyChanged;
        public event ComponentEvent UnhandledEvent;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Data Model
        // This must contain all data necessary for all windows and user controls up the chain
        //  to perform any needed functions.

        #endregion Data Model

        #region Public Functions
        public StoreFrontMain()
        {
            InitializeComponent();
            DataContext = this;

            ServerInterfaceModel.GenerateCollectionModel("Primary.txt");
            ServerInterfaceModel.GenerateCollectionModel("Main.txt");

            ecSwitchToCollectionOverview();
        }
        #endregion

        #region Default Functions (Component Functionality)
        // In the hierarchy of usercontrols and windows, the default functions are accessed
        //  by events fired from children components.

        public void ecSwitchToCollectionOverview()
        {
            OperationWindow = new CompCollectionsOverview(ServerInterfaceModel.GetLoadedCollectionList());
            OperationWindow.UnhandledEvent += RouteReceivedUnhandledEvent;
        }

        public void ecSwitchToCollectionView(string aszCollectionName)
        {
            OperationWindow = new CompCollectionView(aszCollectionName);
            OperationWindow.UnhandledEvent += RouteReceivedUnhandledEvent;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(CompCollectionsOverview.CollectionsOverviewerModel))
            {
                CompCollectionsOverview.CollectionsOverviewerModel CCM =
                        (CompCollectionsOverview.CollectionsOverviewerModel)aDataObject;
                if (aszAction == "ViewCollection")
                {
                    ecSwitchToCollectionView(CCM.SelectedCollection);
                }
            }
        }

        #endregion Default Functions (Component Functionality)

    }
}
