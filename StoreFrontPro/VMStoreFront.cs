using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views;
using StoreFrontPro.Views.CollectionsOverview;
using StoreFrontPro.Views.CollectionViews.Cube;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro
{
    class VMStoreFront : ViewModel<StoreFront>
    {
        public RelayCommand CloseCommand { get; set; }

        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public VMStoreFront(StoreFront Model) : base(Model)
        {
            CloseCommand = new RelayCommand(eCloseCommand);

            showCollectionsOverview();
        }

        public void Notify()
        {
            // do other updates here

            notifyMultiDisplay();
        }

        private void showCollectionsOverview()
        {
            /*
            OperationWindow.DisplayEvent -= viewDisplayEventHandler;
            VMCollectionsOverview collectionsOverviewVM = new VMCollectionsOverview(Model.Collections);
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCollectionsOverview(),
                DataContext: collectionsOverviewVM,
                Persist: false);
            OperationWindow.DisplayEvent += viewDisplayEventHandler; 
            */
            VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(new CollectionModel("Test", new List<Tuple<string, List<Tuple<string, string>>>>()));
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCollectionEditor(),
                DataContext: collectionsOverviewVM,
                Persist: false);
        }

        private void showCollectionCubeView(CollectionModel CollectionModel)
        {
            OperationWindow.DisplayEvent -= viewDisplayEventHandler;
            VMCollectionCube collectionCubeVM = new VMCollectionCube(CollectionModel);
            OperationWindow.SetNewDisplay(
                            Name: "Overview",
                            NewDisplay: new VCollectionCube(),
                            DataContext: collectionCubeVM,
                            Persist: false);
        }

        private void viewDisplayEventHandler(object Source, DisplayEventArgs e)
        {
            if (e.Source == "VCollectionsOverview" && e.Property == "ViewCollection" && e.Event == "Clicked")
            {
                showCollectionCubeView((CollectionModel)e.Get("Collection"));
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

        private void eCloseCommand(object aoCanExecute)
        {
            Model.CloseApplication();
        }
    }
}
