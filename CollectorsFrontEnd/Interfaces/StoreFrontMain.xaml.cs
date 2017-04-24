using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
        private IMenuBarComponent _OperationWindow;
        public IMenuBarComponent OperationWindow
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

        private ObservableCollection<object> _LstViewOptions;
        public ObservableCollection<object> LstViewOptions
        {
            get
            {
                return _LstViewOptions;
            }
            set
            {
                _LstViewOptions = value;
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

            LstViewOptions = new ObservableCollection<object>();
            ecSwitchToCollectionOverview();
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }
        #endregion

        #region Default Functions (Component Functionality)
        // In the hierarchy of usercontrols and windows, the default functions are accessed
        //  by events fired from children components.

        public void ecSwitchToCollectionOverview()
        {
            OperationWindow = new View_CollectionsOverview(ServerInterfaceModel.GetLoadedCollectionList());
            OperationWindow.UnhandledEvent += RouteReceivedUnhandledEvent;
            getViewOptions();
        }

        public void ecSwitchToCollectionView(string aszCollectionName)
        {
            OperationWindow = new View_Cube(aszCollectionName);
            // OperationWindow = new CompCollectionView(aszCollectionName);
            OperationWindow.UnhandledEvent += RouteReceivedUnhandledEvent;
            getViewOptions();
        }

        public void ecSwitchToCubeView(string aszCollectionName)
        {
            OperationWindow = new View_Cube(aszCollectionName);
            OperationWindow.UnhandledEvent += RouteReceivedUnhandledEvent;
            getViewOptions();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(View_CollectionsOverview.CollectionsOverviewerModel))
            {
                View_CollectionsOverview.CollectionsOverviewerModel CCM =
                        (View_CollectionsOverview.CollectionsOverviewerModel)aDataObject;
                if (aszAction == "ViewCollection")
                {
                    ecSwitchToCollectionView(CCM.SelectedCollection);
                }
            }
        }



        #endregion Default Functions (Component Functionality)

        #region Private FUnctions
        private void getViewOptions()
        {
            LstViewOptions.Clear();
            List<Tuple<string, MenuAction>> LstActions = new List<Tuple<string, MenuAction>>();
            LstActions = OperationWindow.GetMenuActions();
            foreach (var act in LstActions)
            {
                if (act.Item1 != "Separator")
                {
                    MenuItem mI = new MenuItem();
                    mI.Header = act.Item1;
                    mI.Click += (o, e) => { act.Item2(); };
                    LstViewOptions.Add(mI);
                }
                else
                {
                    LstViewOptions.Add(new Separator());
                }


            }
            if (LstActions.Count == 0)
            {
                MenuItem mI = new MenuItem();
                mI.Header = "Empty";
                LstViewOptions.Add(mI);
            }
        }
        #endregion

        private void eClose_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void eViewCollectionsOverview_Click(object sender, RoutedEventArgs e)
        {
            ecSwitchToCollectionOverview();
        }

        private void eImportCards_Click(object sender, RoutedEventArgs e)
        {
            ServerInterfaceModel.ImportJSONCollection();
        }
    }
}
