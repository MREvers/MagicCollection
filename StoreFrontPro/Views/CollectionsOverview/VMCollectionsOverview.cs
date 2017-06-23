using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace StoreFrontPro.Views.CollectionsOverview
{
    class VMCollectionsOverview : ViewModel<List<CollectionModel>>
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
            get
            {
                return _SelectedCollection;
            }

            set
            {
                _SelectedCollection = value; // Selection Changed Event Here.\
                eCollectionPreviewUpdate(value);
            }
        }

        public RelayCommand ViewCollectionCommand { get; set; }

        public ObservableCollection<string> AvailableCollections { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> CollectionPreview { get; set; } = new ObservableCollection<string>();
        #endregion Binding

        #region Public Methods
        /// <summary>
        /// The "Model" of this class is a list of collections.
        /// </summary>
        /// <param name="Model"></param>
        public VMCollectionsOverview(List<CollectionModel> Model) : base(Model)
        {
            ViewCollectionCommand = new RelayCommand(eCollectionViewCommand);

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
        #endregion Public Methods

        #region Event Handlers
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

        }
        #endregion
    }
}
