using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views;
using StoreFrontPro.Tools;
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
using StoreFrontPro.Support.LoadingIndicator;

namespace StoreFrontPro
{
   class VMStoreFront : ViewModel<StoreFront>, IVCISupporter
   {
      #region Static Names
      public const string CollectionOverview = "ColOverview";
      public const string DeckboxView = "DBVM";
      public const string CubeView = "CVN";
      #endregion

      #region Bindings
      private bool _MenuEnabled = true;
      public bool MenuEnabled
      {
         get { return _MenuEnabled; }
         set { _MenuEnabled = value; OnPropertyChanged(); }
      }

      public RelayCommand CloseCommand { get; set; }
      public RelayCommand CollectionsOverviewCommand { get; set; }
      public RelayCommand DownloadSetsCommand { get; set; }

      public ObservableCollection<MenuItem> ViewOptions { get; set; } =
         new ObservableCollection<MenuItem>();

      public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } =
         new Dictionary<Type, IViewComponentInterface>();

      private MultiDisplay _OperationWindow;
      public MultiDisplay OperationWindow
      {
         get { return _OperationWindow; }
         set { _OperationWindow = value; OnPropertyChanged(); }
      }
      #endregion

      #region Public Functions
      public VMStoreFront(StoreFront Model, string RoutingName) : base(Model, RoutingName)
      {
         Model.Register(this);

         OperationWindow = new MultiDisplay();
         OperationWindow.DisplayEvent += DisplayEventHandler;

         CloseCommand               = new RelayCommand(eCloseCommand);
         CollectionsOverviewCommand = new RelayCommand((o) => { showCollectionsOverview(); });
         DownloadSetsCommand        = new RelayCommand(eDownloadSetsCommand);

         showCollectionsOverview();
      }

      public void ShowLoadingIndicator()
      {
         MenuEnabled = false;
         OperationWindow.ShowOverlay(new LoadIndicator());
      }

      public void CloseLoadingIndicator()
      {
         MenuEnabled = true;
         OperationWindow.CloseOverlay();
      }
      #endregion

      #region Private Methods
      private void showCollectionsOverview()
      {
         VMCollectionsOverview collectionsOverviewVM = new VMCollectionsOverview(Model.Collections, CollectionOverview);
         OperationWindow.SetNewDisplay(
             Name: "Overview",
             NewDisplay: new VCollectionsOverview(),
             DataContext: collectionsOverviewVM,
             Persist: false);
      }

      private void showCollectionCubeView(CollectionModel CollectionModel)
      {
         VMCollectionCube collectionCubeVM = new VMCollectionCube(CollectionModel, CubeView);
         OperationWindow.SetNewDisplay(
             Name: "Overview",
             NewDisplay: new VCollectionCube(),
             DataContext: collectionCubeVM,
             Persist: false);
      }

      private void showCollectionDeckBox(CollectionModel CollectionModel)
      {
         VMCollectionDeckBox collectionFancyVM = new VMCollectionDeckBox(CollectionModel, DeckboxView);
         OperationWindow.SetNewDisplay(
             Name: "Fancy",
             NewDisplay: new VCollectionDeckBox(),
             DataContext: collectionFancyVM,
             Persist: false);
      }

      private void eDisplayNewViewOptions()
      {
         ViewOptions.Clear();
         List<StoreFrontMenuItem> lstViewOptions = OperationWindow.GetMenuItems();
         lstViewOptions.ForEach(x => ViewOptions.Add(new MenuItem() { Header = x.MenuName, Command = x.Operation }));
      }

      public void eAddNewCollectionClick(string aszCollectionName)
      {
         Model.CreateCollection(aszCollectionName);
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
            Model.LoadCollection(szSelectedFile);
         }
      }

      private void eCloseCommand(object aoCanExecute)
      {
         Model.CloseApplication();

      }

      private void eDownloadSetsCommand(object aoCanExecute)
      {
         Model.LoadLatestJSON();
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {
         
      }
      #endregion

      #region IVCISupporter
      public void DisplayEventHandler(object source, DisplayEventArgs e)
      {
         GetRouter().Call(source.GetType(), this, e.Key, e.Args);
      }

      public InterfaceRouter GetRouter()
      {
         return IRouter;
      }

      static InterfaceRouter _IRouter = null;
      static InterfaceRouter IRouter
      {
         get
         {
            if (_IRouter == null) { BuildInterface(); }
            return _IRouter;
         }
      }

      static void BuildInterface()
      {
         _IRouter = new InterfaceRouter();

         VCICollectionDeckBox CDBIS = new VCICollectionDeckBox(
             SwitchToCube: (x) => { return (x as VMStoreFront).showCollectionCubeView; });
         _IRouter.AddInterface(CDBIS);

         VCICollectionCube CCIS = new VCICollectionCube(
             SwitchToDeckBox: (x)=> { return (x as VMStoreFront).showCollectionDeckBox; });
         _IRouter.AddInterface(CCIS);

         VCICollectionsOverview COIS = new VCICollectionsOverview(
             ViewCollection: (x)=> { return (x as VMStoreFront).showCollectionDeckBox; },
             AddCollection: (x) => { return (x as VMStoreFront).eAddNewCollectionClick; },
             LoadCollection: (x)=> { return (x as VMStoreFront).eLoadCollectionFromFile; });
         _IRouter.AddInterface(COIS);

         VCIMultiDisplay MDIS = new VCIMultiDisplay(
             Change: (x) => { return (x as VMStoreFront).eDisplayNewViewOptions; });
         _IRouter.AddInterface(MDIS);
      }
      #endregion
   }
}
