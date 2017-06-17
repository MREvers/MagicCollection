using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Xml;
using System.IO;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Net;
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
using CollectorsFrontEnd.StoreFrontSupport;
using CollectorsFrontEnd.Views;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for StoreFrontMain.xaml
    /// </summary>
    public partial class StoreFrontMain : Window, INotifyPropertyChanged
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

        #region Public Functions
        public StoreFrontMain()
        {
            InitializeComponent();
            DataContext = this;

            loadStartupCollections();

            switchToCollectionOverview();
        }
        #endregion

        #region Public Methods

        public void switchToCollectionOverview()
        {
            OperationWindow.SetNewDisplay(
                Name: "CollectionOverview", 
                NewDisplay: new ViewCollectionsMenu());
        }

        // In the hierarchy of usercontrols and windows, the default functions are accessed
        //  by events fired from children components.
        /*
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

*/

        #endregion Default Functions (Component Functionality)

        #region Private FUnctions
        /*
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
                //LstViewOptions.Add(mI);
            }
            else
            {
                //LstViewOptions.Add(new Separator());
            }


        }
        if (LstActions.Count == 0)
        {
            MenuItem mI = new MenuItem();
            mI.Header = "Empty";
            //LstViewOptions.Add(mI);
        }
    }
    */
        private void loadStartupCollections()
        {
            XmlReader reader = XmlReader.Create(".\\Config\\Config.xml");
            string szCollection = "";
            // This is bad. Clean this in the future.
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name == "Startup")
                    {
                        break;
                    }
                    else if (reader.Name == "CollectionFolder")
                    {
                        reader.Read();
                        szCollection = reader.Value;
                    }
                }
            }

            List<string> lstStartup = new List<string>();

            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name == "FileName")
                    {
                        reader.Read();
                        lstStartup.Add(reader.Value);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (szCollection != "")
            {
                foreach (string fileName in lstStartup)
                {
                    ServerInterface.Server.GenerateCollectionModel(".\\" + szCollection + "\\" +fileName + ".txt");
                }
            }
            
        }
        
        #endregion

        #region UI Event Handlers
        private void eClose_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void eViewCollectionsOverview_Click(object sender, RoutedEventArgs e)
        {
            //ecSwitchToCollectionOverview();
        }

        private void eImportCards_Click(object sender, RoutedEventArgs e)
        {
            string szZipPath = AppDomain.CurrentDomain.BaseDirectory + @"\Config\Source\AllSets.json.zip";
            string szExtractPath = AppDomain.CurrentDomain.BaseDirectory + @"\Config\Source";

            if (Directory.Exists(szExtractPath))
            {
                foreach(var file in Directory.EnumerateFiles(szExtractPath))
                {
                    File.Delete(file);
                }
            }
            else
            {
                Directory.CreateDirectory(szExtractPath);
            }

            using (var client = new WebClient())
            {
                client.DownloadFile("https://mtgjson.com/json/AllSets.json.zip", szZipPath);
            }

            System.IO.Compression.ZipFile.ExtractToDirectory(szZipPath, szExtractPath);

            ServerInterface.ServerIFace.ImportJSONCollection();
        }
        #endregion
    }
}
