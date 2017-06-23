using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionsOverview
{
    class VMCollectionsOverview : ViewModel<List<CollectionModel>>
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        private string _SelectedValue = "";
        public string SelectedValue
        {
            get
            {
                return _SelectedValue;
            }

            set
            {
                _SelectedValue = value; // Selection Changed Event Here.
            }
        }

        public ObservableCollection<string> AvailableCollections { get; set; } = new ObservableCollection<string>();

        /// <summary>
        /// The "Model" of this class is a list of collections.
        /// </summary>
        /// <param name="Model"></param>
        public VMCollectionsOverview(List<CollectionModel> Model) : base(Model)
        {
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCCollectionsMenuList(),
                DataContext: this,
                Persist: false);
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            Model.ForEach((x) => { AvailableCollections.Add(x.CollectionName); });
        }
    }
}
