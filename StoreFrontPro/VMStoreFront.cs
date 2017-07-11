﻿using StoreFrontPro.Server;
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
    class VMStoreFront : ViewModel<StoreFront>
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

        public VMStoreFront(StoreFront Model) : base(Model)
        {
            OperationWindow = new MultiDisplay();
            OperationWindow.DisplayEvent += viewDisplayEventHandler;
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

        private void viewDisplayEventHandler(object Source, DisplayEventArgs e)
        {
            if (e.Source == "VMCollectionsOverview")
            {
                if (e.Property == "ViewCollection")
                {
                    showCollectionCubeView((CollectionModel)e.Get("Collection"));
                }
                else if (e.Property == "AddCollection")
                {
                    eAddNewCollectionClick((string)e.Get("NewCollectionName"));
                }
                else if (e.Property == "LoadCollection")
                {
                    eLoadCollectionFromFile();
                }
            }
            else if (e.Source == "MultiDisplay")
            {
                eDisplayNewViewOptions();
            }
            // The expectation is that the source's model is a Collection
            else if (e.Property == "SwitchToDeckbox")
            {
                CollectionModel cm = (Source as ViewModel<CollectionModel>).Model;
                showCollectionDeckBox(cm);
            }
            else if (e.Property == "SwitchToCube")
            {
                CollectionModel cm = (Source as ViewModel<CollectionModel>).Model;
                showCollectionCubeView(cm);
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

        public void eAddNewCollectionClick(string aszCollectionName)
        {
            ServerInterface.Server.CreateCollection(aszCollectionName);
            Model.SyncCollections();
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
                ServerInterface.Server.SyncServerTask(() => { Model.SyncCollections(); });
            }
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
