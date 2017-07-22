using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.CollectionViews.Cube;
using StoreFrontPro.Views.Components;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using StoreFrontPro.Tools;
using StoreFrontPro.Views.Components.RequestTextOverlay;

namespace StoreFrontPro.Views.CollectionsOverview
{
    class VMCollectionsOverview : ViewModel<ObservableCollection<CollectionModel>>, IViewComponent, IVCISupporter
    {
        #region Binding
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        private string _SelectedCollection = "";

        public string SelectedCollection
        {
            get { return _SelectedCollection; }
            set { _SelectedCollection = value; eCollectionPreviewUpdate(value); }
        }

        public RelayCommand ViewCollectionCommand { get; set; }
        public RelayCommand AddCollectionCommand { get; set; }
        public RelayCommand LoadCollectionFileCommand { get; set; }

        public ObservableCollection<string> AvailableCollections { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> CollectionPreview { get; set; } = new ObservableCollection<string>();

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();
        #endregion Binding

        #region Events
        public event DisplayEventHandler DisplayEvent;
        #endregion

        #region Public Methods
        /// <summary>
        /// The "Model" of this class is a list of collections.
        /// </summary>
        /// <param name="Model"></param>
        public VMCollectionsOverview(ObservableCollection<CollectionModel> Model) : base(Model)
        {
            ViewCollectionCommand = new RelayCommand(eCollectionViewCommand);
            AddCollectionCommand = new RelayCommand(eAddCollectionCommand);
            LoadCollectionFileCommand = new RelayCommand(eLoadCollectionFileCommand);
            
            VCIRequestText RTIS = new VCIRequestText(
                Accept: eAddCollectionResolved,
                Cancel: eAddCollectionCanceled);
            SupportedInterface.Add(RTIS.GetInterfaceType(), RTIS);

            // Dont use this to set the data context because we dont want the MultiDisplay to hook to this display event.
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCCollectionsMenuList() { DataContext = this},
                Persist: false);
            OperationWindow.DisplayEvent += DisplayEventHandler;
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            AvailableCollections.Clear();
            Model.ForEach((x) => { AvailableCollections.Add(x.CollectionName); });
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            return new List<StoreFrontMenuItem>();
        }
        #endregion Public Methods

        #region Event Handlers
        public void DisplayEventHandler(object source, DisplayEventArgs e)
        {
            if (SupportedInterface.ContainsKey(source.GetType()))
            {
                SupportedInterface?[source.GetType()]?.TryInvoke(e.Key, e.Args);
            }
        }

        private void eCollectionPreviewUpdate(string aszCollectionName)
        {
            if (!string.IsNullOrEmpty(aszCollectionName))
            {
                CollectionPreview.Clear();
                Model.Where(x => x.CollectionName == aszCollectionName)
                    .FirstOrDefault()
                    ?.CollectionItems
                    .ForEach(x => CollectionPreview.Add(x.GetIdealIdentifier()));
            }
        }

        private void eCollectionViewCommand(object aoCanExecute)
        {
            if (!string.IsNullOrEmpty(SelectedCollection))
            {
                DisplayEventArgs eventArgs = new DisplayEventArgs(
                    VCICollectionsOverview.ViewCollection, Model.Where(x => x.CollectionName == SelectedCollection).First());
                DisplayEvent(this, eventArgs);
            }
        }

        private void eAddCollectionCommand(object aoCanExecute)
        {
            VMRequestText requestTextVM = new VMRequestText("");
            VRequestText requestTextV = new VRequestText();
            requestTextV.DataContext = requestTextVM;
            OperationWindow.ShowOverlay(requestTextV);
        }

        private void eAddCollectionCanceled()
        {
            OperationWindow.CloseOverlay();
        }

        private void eAddCollectionResolved(string aszNewCol)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionsOverview.AddCollection, aszNewCol);
            DisplayEvent(this, eventArgs);
            OperationWindow.CloseOverlay();
        }

        private void eLoadCollectionFileCommand(object aoCanExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionsOverview.LoadCollection);
            DisplayEvent(this, eventArgs);
        }

        #endregion
    }
}
