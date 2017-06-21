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
using System.Windows.Threading;
using System.Threading;

namespace CollectorsFrontEnd.Views
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

        #region Private Functions
        public void switchToCollectionOverview()
        {
            OperationWindow.SetNewDisplay(
                Name: "CollectionOverview",
                NewDisplay: new ViewCollectionsMenu());
            OperationWindow.DisplayEvent += eMultiDisplayEventHandler;
        }

        public void switchToCubeView(CollectionModel aoCollectionModel)
        {
            if (aoCollectionModel == null) { return; }

            OperationWindow.SetNewDisplay(
                Name: "CubeView."+aoCollectionModel.CollectionName,
                NewDisplay: new ViewCube(aoCollectionModel));
            OperationWindow.DisplayEvent += eMultiDisplayEventHandler;
        }

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

        #region Event Handlers
        private void eMultiDisplayEventHandler(object aoSource, DisplayEventArgs aoEvent)
        {
            if (aoEvent.Source == "VCCollectionsMenuList")
            {
                eCollectionsMenuListEventHandler(aoSource, aoEvent);
            }
        }

        private void eCollectionsMenuListEventHandler(object aoSource, DisplayEventArgs aoEvent)
        {
            if (aoEvent.Property == "ViewCollectionButton")
            {
                if (aoEvent.Event == "Clicked")
                {
                    eViewCollectionCubeView_Click(aoSource, aoEvent);
                }
            }
        }
        #endregion

        #region UI Event Handlers
        private void eClose_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void eViewCollectionCubeView_Click(object aoSource, DisplayEventArgs aoEvent)
        {
            string szNewSet = aoEvent.Get("Selection")as String;
            ServerInterface.Server.GetCollectionModel(
                CollectionName: szNewSet, 
                Callback: switchToCubeView,
                UICallback: true);
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

            ServerInterface.Server.ImportJSONCollection();
        }
        #endregion
    }
}
