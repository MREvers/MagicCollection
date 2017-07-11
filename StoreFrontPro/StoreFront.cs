using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Xml;

namespace StoreFrontPro
{
    /// <summary>
    /// This class controls the main window essentially. Switches sub-views and displays other windows.
    /// </summary>
    class StoreFront
    {
        public ObservableCollection<CollectionModel> Collections { get; private set; }
        public CollectionModel ActiveCollection { get; private set; }

        private Window m_ucMainWindow;

        // Maintain a reference to the ViewModel.
        public VMStoreFront StoreFrontVM { get; private set; }

        public StoreFront(Window MainWindow)
        {
            m_ucMainWindow = MainWindow;
            Collections = new ObservableCollection<CollectionModel>();
            ActiveCollection = null;
            StoreFrontVM = new VMStoreFront(Model: this);
            
            initializeStoreFront();
        }

        public void CloseApplication()
        {
            m_ucMainWindow.Close();
        }

        public void SyncCollections()
        {
            ServerInterface.Server.GetCollectionModels(
                Callback: (alstColMos) => { Collections.Clear(); alstColMos.ForEach((x) => { Collections.Add(x); }); StoreFrontVM.Notify(); },
                UICallback: true);
        }

        private void initializeStoreFront()
        {
            ServerInterface.Server.Start();

            loadStartupCollections();

            SyncCollections();
        }

        private void loadStartupCollections()
        {
            // There should be a config object at some point in the future!!!!

            if (!File.Exists(".\\Config\\Config.xml")) { return; }

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
                    ServerInterface.Server.LoadCollection(".\\" + szCollection + "\\" + fileName + ".txt");
                }
            }

        }

        private void downloadLatestMTGJson()
        {
            string szZipPath = AppDomain.CurrentDomain.BaseDirectory + @"\Config\Source\AllSets.json.zip";
            string szExtractPath = AppDomain.CurrentDomain.BaseDirectory + @"\Config\Source";

            if (Directory.Exists(szExtractPath))
            {
                foreach (var file in Directory.EnumerateFiles(szExtractPath))
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
    }
}
