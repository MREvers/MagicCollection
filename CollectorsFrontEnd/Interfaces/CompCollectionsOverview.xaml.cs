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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCollectionsOverview.xaml
    /// </summary>
    public partial class CompCollectionsOverview : UserControl, IMenuBarComponent, INotifyPropertyChanged
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
                LstAvailableCollections = aLstAvailableCollections;
            }
            public List<string> LstAvailableCollections;
            public List<string> LstCurrentCollectionPreview = new List<string>();
            public string SelectedCollection = "";
        }

        public CollectionsOverviewerModel DataModel;
        public event ComponentEvent UnhandledEvent;

        public CompCollectionsOverview(List<string> alstAvailableCollections)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new CollectionsOverviewerModel(alstAvailableCollections);
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
                DataModel.LstCurrentCollectionPreview = ColM.LstCopyModels.Select(x => x.CardNameLong).ToList();
                LstBoxCollectionPreview.ItemsSource = DataModel.LstCurrentCollectionPreview;
            }
            
            /* THis is view selection code
            string szCollectionSelected = LstBoxLoadedCollections.SelectedValue.ToString();
            if (!string.IsNullOrEmpty(szCollectionSelected))
            {
                UnhandledEvent(DataModel);
            }
            */
        }

        // This gets subscribed to the unhandled event of its components
        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            return new List<Tuple<string, MenuAction>>();//throw new NotImplementedException();
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


    }
}
