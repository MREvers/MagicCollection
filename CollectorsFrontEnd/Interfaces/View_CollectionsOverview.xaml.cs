using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.Interfaces.Controls;
using CollectorsFrontEnd.Interfaces.Subs;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCollectionsOverview.xaml
    /// </summary>
    public partial class View_CollectionsOverview : UserControl, IMenuBarComponent, INotifyPropertyChanged
    {
        #region Data Binding
        public List<string> LstAvailableCollections
        {
            get
            {
                return DataModel.LstAvailableCollections;
            }
        }

        public List<string> LstCollectionPreview
        {
            get
            {
                return DataModel.LstCurrentCollectionPreview;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        public class CollectionsOverviewerModel : IDataModel
        {
            public CollectionsOverviewerModel(List<string> aLstAvailableCollections)
            {
                UpdateCollectionsList(aLstAvailableCollections);
            }

            public void UpdateCollectionsList(List<string> aLstAvailableCollections)
            {
                LstAvailableCollections = aLstAvailableCollections;
            }

            public List<string> LstAvailableCollections;
            public List<string> LstCurrentCollectionPreview = new List<string>();
            public string SelectedCollection = "";
        }

        #region Public Properties

        public Component_OverviewList OverlayPanel
        {
            get
            {
                return (Component_OverviewList) m_Overlay.MainControl;
            }
        }

        #endregion

        #region Public Fields

        public CollectionsOverviewerModel DataModel;

        #endregion 

        #region Private Fields

        private Control_OverlayPanel m_Overlay = null;

        #endregion

        public event ComponentEvent UnhandledEvent;

        #region Public Functions

        public View_CollectionsOverview(List<string> alstAvailableCollections)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new CollectionsOverviewerModel(alstAvailableCollections);

            LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;

            Component_OverviewList nCmp = new Component_OverviewList();
            nCmp.LstBoxLoadedCollections.SelectionChanged += eLstBoxLoadedCollections_SelectionChanged;
            nCmp.BtnAddCollection.Click += eAddCol_Click;
            nCmp.BtnLoadCollection.Click += eLoadCol_Click;
            nCmp.BtnViewCollection.Click += eViewCollection_Click;
            nCmp.LstBoxLoadedCollections.ItemsSource = DataModel.LstAvailableCollections;

            m_Overlay = new Control_OverlayPanel();
            m_Overlay.SetMainControl(nCmp);

            OverviewPanel.Children.Add(m_Overlay);
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            return new List<Tuple<string, MenuAction>>();//throw new NotImplementedException();
        }

        // This gets subscribed to the unhandled event of its components
        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(Module_EnterText.CompSubEnterTextDataModel))
            {
                Module_EnterText.CompSubEnterTextDataModel DM = (Module_EnterText.CompSubEnterTextDataModel)aDataObject;
                if (aszAction == "Cancel")
                {
                    showMainDisplay();
                }
                else if (aszAction == "OK")
                {
                    addCollection(DM.Text);
                    buildAvailableCollectionsList();
                    showMainDisplay();
                }
            }
        }

        #endregion

        #region Private Functions

        private void buildAvailableCollectionsList()
        {
            // Get Loaded Collections
            List<string> lstAvailableCollections = ServerInterfaceModel.GetLoadedCollectionList();
            DataModel.UpdateCollectionsList(lstAvailableCollections);
            OverlayPanel.LstBoxLoadedCollections.ItemsSource = DataModel.LstAvailableCollections;
            LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
        }

        // This is unhandled
        private void eLstBoxLoadedCollections_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string szCollectionSelected = OverlayPanel.LstBoxLoadedCollections.SelectedValue.ToString();
            DataModel.SelectedCollection = szCollectionSelected;
            CollectionModel ColM = ServerInterfaceModel.GetCollectionModel(szCollectionSelected);
            if (!(ColM == null))
            {
                DataModel.LstCurrentCollectionPreview = ColM.LstCopyModels.Select(x => x.GetIdealIdentifier()).ToList();
                LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
            }
        }

 

        private void addCollection(string aszNewName)
        {
            ServerInterfaceModel.CreateCollection(aszNewName);
        }

        private void showMainDisplay()
        {
            m_Overlay.ShowMain();
        }

        private void showEnterTextWindow()
        {
            Module_EnterText ITI = new Module_EnterText();
            ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
            m_Overlay.ShowOverlay(ITI);
        }

        #endregion

        #region UI Event Handlers

        private void eViewCollection_Click(object sender, RoutedEventArgs e)
        {
            if (OverlayPanel.LstBoxLoadedCollections.SelectedValue != null)
            {
                string szCollectionSelected = OverlayPanel.LstBoxLoadedCollections.SelectedValue.ToString();
                if (!string.IsNullOrEmpty(szCollectionSelected))
                {
                    UnhandledEvent(DataModel, "ViewCollection");
                }
            }

        }

        private void eLoadCol_Click(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog 
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            // Set filter for file extension and default file extension 
            dlg.DefaultExt = ".txt";
            dlg.Filter = "Text Files (*.txt)|*.txt";


            // Display OpenFileDialog by calling ShowDialog method 
            Nullable<bool> result = dlg.ShowDialog();

            // Get the selected file name and display in a TextBox 
            if (result == true)
            {
                // Open document 
                string filename = dlg.FileName;
                if (ServerInterfaceModel.GenerateCollectionModel(filename) != null)
                {
                    buildAvailableCollectionsList();
                }
            }

        }

        private void eAddCol_Click(object sender, RoutedEventArgs e)
        {
            showEnterTextWindow();
        }

        #endregion
    }
}
