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
    class VMCollectionsOverview : ViewModel<List<CollectionModel>>, IViewComponent
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

        public ObservableCollection<string> AvailableCollections { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> CollectionPreview { get; set; } = new ObservableCollection<string>();
        #endregion Binding

        #region Events
        public event DisplayEventHandler DisplayEvent;
        #endregion

        #region Public Methods
        /// <summary>
        /// The "Model" of this class is a list of collections.
        /// </summary>
        /// <param name="Model"></param>
        public VMCollectionsOverview(List<CollectionModel> Model) : base(Model)
        {
            ViewCollectionCommand = new RelayCommand(eCollectionViewCommand);
            AddCollectionCommand = new RelayCommand(eAddCollectionCommand);

            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCCollectionsMenuList(),
                DataContext: this,
                Persist: false);
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
        private void eDisplayEventHandler(object source, DisplayEventArgs e)
        {
            if (e.Source == "VMRequestText")
            {
                if (e.Property == "AcceptCommand")
                {
                    DisplayEventArgs eventArgs = new DisplayEventArgs("VCollectionsOverview", "AddCollection", "Clicked");
                    eventArgs.Add("NewCollectionName", (string)e.Get("Text"));
                    DisplayEvent(this, eventArgs);
                }
                OperationWindow.CloseOverlay();
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
                DisplayEventArgs eventArgs = new DisplayEventArgs("VCollectionsOverview", "ViewCollection", "Clicked");
                eventArgs.Add(Key: "Collection", Value: Model.Where(x => x.CollectionName == SelectedCollection).First());
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
        #endregion
    }
}
