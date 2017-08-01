using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views;
using StoreFrontPro.Views.CollectionsOverview;
using StoreFrontPro.Views.CollectionViews.Cube;
using StoreFrontPro.Views.CollectionViews.Deckbox;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro
{
    class VMStoreFront : ViewModel<StoreFront>, IVCISupporter
    {
        public RelayCommand CloseCommand { get; set; }
        public RelayCommand CollectionsOverviewCommand { get; set; }
        public RelayCommand DownloadSetsCommand { get; set; }

        public ObservableCollection<MenuItem> ViewOptions { get; set; } = new ObservableCollection<MenuItem>();

        private MultiDisplay _OperationWindow;
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();

        public VMStoreFront(StoreFront Model) : base(Model)
        {
            
            OperationWindow = new MultiDisplay();
            OperationWindow.DisplayEvent += DisplayEventHandler;
            
            VCICollectionDeckBox CDBIS = new VCICollectionDeckBox(
                SwitchToCube: showCollectionCubeView,
                ChildCreated: eSyncCollections);
            VCICollectionCube CCIS = new VCICollectionCube(
                SwitchToDeckBox: showCollectionDeckBox);
            VCICollectionsOverview COIS = new VCICollectionsOverview(
                ViewCollection: showCollectionDeckBox,
                AddCollection: eAddNewCollectionClick,
                LoadCollection: eLoadCollectionFromFile);
            VCIMultiDisplay MDIS = new VCIMultiDisplay(
                Change: eDisplayNewViewOptions);
            SupportedInterface.Add(CDBIS.GetInterfaceType(), CDBIS);
            SupportedInterface.Add(CCIS.GetInterfaceType(), CCIS);
            SupportedInterface.Add(COIS.GetInterfaceType(), COIS);
            SupportedInterface.Add(MDIS.GetInterfaceType(), MDIS);
            
            CloseCommand = new RelayCommand(eCloseCommand);
            CollectionsOverviewCommand = new RelayCommand((o) => { showCollectionsOverview(); });
            DownloadSetsCommand = new RelayCommand(eDownloadSetsCommand);

            showCollectionsOverview();
        }

        public void Notify()
        {
            // do other updates here
            notifyMultiDisplay();
        }

        public void DisplayEventHandler(object source, DisplayEventArgs e)
        {
            if (SupportedInterface.ContainsKey(source.GetType()))
            {
                SupportedInterface?[source.GetType()]?.TryInvoke(e.Key, e.Args);
            }
        }

        private void showCollectionsOverview()
        {
            VMCollectionsOverview collectionsOverviewVM = new VMCollectionsOverview(Model.Collections);
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCollectionsOverview(),
                DataContext: collectionsOverviewVM,
                Persist: false);
        }

        private void showCollectionCubeView(CollectionModel CollectionModel)
        {

            VMCollectionCube collectionCubeVM = new VMCollectionCube(CollectionModel);
            OperationWindow.SetNewDisplay(
                            Name: "Overview",
                            NewDisplay: new VCollectionCube(),
                            DataContext: collectionCubeVM,
                            Persist: false);
        }

        private void showCollectionDeckBox(CollectionModel CollectionModel)
        {
            VMCollectionDeckBox collectionFancyVM = new VMCollectionDeckBox(CollectionModel);
            OperationWindow.SetNewDisplay(
                            Name: "Fancy",
                            NewDisplay: new VCollectionDeckBox(),
                            DataContext: collectionFancyVM,
                            Persist: false);
        }

        private void notifyMultiDisplay()
        {
            UserControl viewMode = OperationWindow.Display;

            // Determine what display mode the multidisplay is in.
            if (viewMode.GetType() == typeof(VCollectionsOverview))
            {
                VMCollectionsOverview collectionsOverviewVM = OperationWindow.DisplayViewModel as VMCollectionsOverview;
                collectionsOverviewVM.SyncWithModel();
            }
        }

        private void eDisplayNewViewOptions()
        {
            List<StoreFrontMenuItem> lstViewOptions = OperationWindow.GetMenuItems();
            ViewOptions.Clear();
            lstViewOptions.ForEach(x => ViewOptions.Add(new MenuItem() { Header = x.MenuName, Command = x.Operation }));
        }

        public void eAddNewCollectionClick(string aszCollectionName)
        {
            ServerInterface.Server.CreateCollection(aszCollectionName);
            Model.SyncCollectionList();
        }

        public void eLoadCollectionFromFile()
        {
            Microsoft.Win32.OpenFileDialog openFile = new Microsoft.Win32.OpenFileDialog();

            openFile.DefaultExt = ".txt";
            openFile.Filter = "Text Files (*.txt)|*.txt";

            Nullable<bool> result = openFile.ShowDialog();
            if (result == true)
            {
                string szSelectedFile = openFile.FileName;
                ServerInterface.Server.LoadCollection(szSelectedFile);
                ServerInterface.Server.SyncServerTask(() => { Model.SyncAllCollections(); Model.SyncCollectionList(); });
            }
        }

        private void eSyncCollections()
        {
            ServerInterface.Server.SyncServerTask(() => { Model.SyncAllCollections(); Model.SyncCollectionList(); });
        }

        private void eCloseCommand(object aoCanExecute)
        {
            Model.CloseApplication();

        }

        private void eDownloadSetsCommand(object aoCanExecute)
        {
            ServerInterface.Server.ImportJSONCollection();
        }
    }
}
