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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro
{
    class VMStoreFront : ViewModel<StoreFront>
    {
        public RelayCommand CloseCommand { get; set; }
        public RelayCommand CollectionsOverviewCommand { get; set; }

        public ObservableCollection<MenuItem> ViewOptions { get; set; } = new ObservableCollection<MenuItem>();

        private MultiDisplay _OperationWindow;
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public VMStoreFront(StoreFront Model) : base(Model)
        {
            OperationWindow = new MultiDisplay();
            OperationWindow.DisplayEvent += viewDisplayEventHandler;
            CloseCommand = new RelayCommand(eCloseCommand);
            CollectionsOverviewCommand = new RelayCommand((o) => { showCollectionsOverview(); });

            showCollectionsOverview();
        }

        public void Notify()
        {
            // do other updates here
            notifyMultiDisplay();
        }

        private void showCollectionsOverview()
        {
            VMCollectionsOverview collectionsOverviewVM = new VMCollectionsOverview(Model.Collections);
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCollectionsOverview(),
                DataContext: collectionsOverviewVM,
                Persist: false);
            
            /*
            ServerInterface.Server.GenerateCollectionModel("Collections\\Primary.txt");
            ServerInterface.Server.GetCollectionModel("Primary",
                (o) =>
                {
                    VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(o);
                    OperationWindow.SetNewDisplay(
                        Name: "Overview",
                        NewDisplay: new VCollectionEditor(),
                        DataContext: collectionsOverviewVM,
                        Persist: false);
                }, true);
            */
        }

        private void showCollectionCubeView(CollectionModel CollectionModel)
        {
            
            VMCollectionCube collectionCubeVM = new VMCollectionCube(CollectionModel);
            OperationWindow.SetNewDisplay(
                            Name: "Overview",
                            NewDisplay: new VCollectionCube(),
                            DataContext: collectionCubeVM,
                            Persist: false);
            
            /*
            VMCollectionDeckBox collectionFancyVM = new VMCollectionDeckBox(CollectionModel);
            OperationWindow.SetNewDisplay(
                            Name: "Fancy",
                            NewDisplay: new VCollectionDeckBox(),
                            DataContext: collectionFancyVM,
                            Persist: false);
                            */
        }

        private void viewDisplayEventHandler(object Source, DisplayEventArgs e)
        {
            if (e.Source == "VCollectionsOverview" && e.Property == "ViewCollection" && e.Event == "Clicked")
            {
                showCollectionCubeView((CollectionModel)e.Get("Collection"));
            }
            else if (e.Source == "MultiDisplay")
            {
                eDisplayNewViewOptions();
            }
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

        private void eCloseCommand(object aoCanExecute)
        {
            Model.CloseApplication();
        }
    }
}
