using CollectorsFrontEnd.InterfaceModels;
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

        UserControl m_OverlayControl = null;

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

        public CollectionsOverviewerModel DataModel;
        public event ComponentEvent UnhandledEvent;

        public View_CollectionsOverview(List<string> alstAvailableCollections)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new CollectionsOverviewerModel(alstAvailableCollections);
            LstBoxLoadedCollections.ItemsSource = DataModel.LstAvailableCollections;
            LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
        }

        private void buildAvailableCollectionsList()
        {
            // Get Loaded Collections
            List<string> lstAvailableCollections = ServerInterfaceModel.GetLoadedCollectionList();
            DataModel.UpdateCollectionsList(lstAvailableCollections);
            LstBoxLoadedCollections.ItemsSource = DataModel.LstAvailableCollections;
            LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
        }

        // This is unhandled
        private void eLstBoxLoadedCollections_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string szCollectionSelected = LstBoxLoadedCollections.SelectedValue.ToString();
            DataModel.SelectedCollection = szCollectionSelected;
            CollectionModel ColM = ServerInterfaceModel.GetCollectionModel(szCollectionSelected);
            if (!(ColM == null))
            {
                DataModel.LstCurrentCollectionPreview = ColM.LstCopyModels.Select(x => x.GetIdealIdentifier()).ToList();
                LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
            }
        }

        // This gets subscribed to the unhandled event of its components
        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(CompSubEnterText.CompSubEnterTextDataModel))
            {
                CompSubEnterText.CompSubEnterTextDataModel DM = (CompSubEnterText.CompSubEnterTextDataModel) aDataObject;
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

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            return new List<Tuple<string, MenuAction>>();//throw new NotImplementedException();
        }

        private void addCollection(string aszNewName)
        {
            ServerInterfaceModel.CreateCollection(aszNewName);
        }

        private void showMainDisplay()
        {
            if (m_OverlayControl != null)
            {
                CenterPanel.Children.Remove(m_OverlayControl);
            }
            
            m_OverlayControl = null;
            ControlPanel.IsEnabled = true;
        }

        private void showEnterTextWindow()
        {
            showMainDisplay();

            CompSubEnterText ITI = new CompSubEnterText();
            m_OverlayControl = ITI;
            ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
            Panel.SetZIndex(CenterPanel, 2);
            CenterPanel.Children.Add(ITI);
            ControlPanel.IsEnabled = false;

        }

        private void eBtnViewCollection_Click(object sender, RoutedEventArgs e)
        {
            if (LstBoxLoadedCollections.SelectedValue != null)
            {
                string szCollectionSelected = LstBoxLoadedCollections.SelectedValue.ToString();
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
    }
}
