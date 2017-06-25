using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VMCollectionCube : ViewModel<CollectionModel>
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public VMCollectionCube(CollectionModel Model) : base(Model)
        {
            VMCardGroupDisplay collectionCubeVM = new VMCardGroupDisplay(Model.CollectionItems);

            OperationWindow.SetNewDisplay(
                Name: "CollectionView",
                NewDisplay: new VCardGroupDisplay(),
                DataContext: collectionCubeVM,
                Persist: false);
        }
    }
}
