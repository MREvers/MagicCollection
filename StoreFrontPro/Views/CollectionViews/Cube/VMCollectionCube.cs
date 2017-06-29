using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VMCollectionCube : ViewModel<CollectionModel>, IViewComponent
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();

        public event DisplayEventHandler DisplayEvent;

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

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            List<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();

            StoreFrontMenuItem openCollectionEditor = new StoreFrontMenuItem("Edit Collection", displayCollectionEditorCommand);

            lstRetVal.Add(openCollectionEditor);

            return lstRetVal;
        }

        private void displayEventHandler(object source, DisplayEventArgs e)
        {
            if (e.Source == "VMCollectionEditor" )
            {
                if (e.Property == "AcceptCommand")
                {
                    if (OperationWindow.Display.DataContext is VMCardGroupDisplay)
                    {
                        (OperationWindow.Display.DataContext as VMCardGroupDisplay).SyncWithModel();
                    }
                    OperationWindow.CloseOverlay();
                }
                else if (e.Property == "CancelCommand")
                {
                    OperationWindow.CloseOverlay();
                }
            }
        }

        private void displayCollectionEditorCommand(object canExecute)
        {
            VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(Model);
            OperationWindow.ShowOverlay(new VCollectionEditor() { DataContext = collectionsOverviewVM });
            collectionsOverviewVM.DisplayEvent += displayEventHandler;
        }
    }
}
